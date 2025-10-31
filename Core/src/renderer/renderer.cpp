//
// Created by User on 10/6/2025.
//

#include "renderer.h"

#include "mesh/mesh.h"
#include "renderer/utils/utils.h"

#include <glm/glm.hpp>

#include <algorithm>

namespace LearnVulkanRAII
{
    Renderer::Renderer(const GraphicsContext::Shared& graphicsContext)
        : m_graphicsContext(graphicsContext),
        m_allocationBatchInfo(500, 32)
    {
        init();
    }

    void Renderer::beginFrame(const CameraViewData &cameraData)
    {
        beginFrame(m_defaultFramebuffer, cameraData);
    }

    void Renderer::beginFrame(const SwapchainFramebuffer::Shared& framebuffer, const CameraViewData& cameraData)
    {
        auto& device = m_graphicsContext->getDevice();
        auto& swapchain = m_graphicsContext->getSwapchain();

        auto& framebuffers = framebuffer->getBuffers();
        ASSERT(m_commandPools.size() == framebuffers.size(), "Framebuffer seems incompatible!");
        m_framebuffer = framebuffer;

        auto& frameContext = m_inFlightFrameManager.getCurrentFrameContext();
        auto _ = device.waitForFences(**frameContext.inFlightFence, VK_TRUE, UINT64_MAX);
        device.resetFences(**frameContext.inFlightFence);

        // reset the statistics
        m_stats.reset();

        // reset frame context counts
        frameContext.resetCounts();

        auto [result, imageIndex] =
            swapchain.acquireNextImage(UINT64_MAX, **frameContext.imageAvailableSemaphore);

        ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR,
            "Failed to acquire swapchain image!");

        frameContext.imageIndex = imageIndex;

        // reset the command buffers of this frame
        m_commandPools[imageIndex]->reset();

        // Copy the per-frame camera view data
        void* data = m_cameraViewDataBuffer->map();
        std::memcpy(data, &cameraData, sizeof(CameraViewData));
        m_cameraViewDataBuffer->unmap();
    }

    void Renderer::endFrame()
    {
        auto& frameContext = m_inFlightFrameManager.getCurrentFrameContext();
        frameContext.isLastDrawCall = true;

        draw();
        presentFrame();
        m_inFlightFrameManager.nextFrame();
    }

    void Renderer::drawMesh(const Mesh& mesh, const Transform& transform)
    {
        if (m_localTransferSpace.getCurrentFaceCounts() + mesh.getFaceCount() > m_allocationBatchInfo.batchSize ||
            m_localTransferSpace.currentObjectMetadataCount >= m_allocationBatchInfo.modelCount)
        {
            // flush
            draw();
        }

        // TODO: Need to find a better way instead of just copying memory on each draw call
        size_t indexOffset = m_localTransferSpace.currentVertexCount;
        memcpy((m_localTransferSpace.vertices + indexOffset),
            mesh.vertices.data(),
            mesh.getVerticesSizeInBytes());

        std::fill_n(m_localTransferSpace.internalVertices + indexOffset,
            sizeof(InternalVertex) * mesh.getVerticesCount(),
            InternalVertex{ static_cast<int>(m_localTransferSpace.currentObjectMetadataCount) });

        m_localTransferSpace.currentVertexCount += mesh.getVerticesCount();

        for (const auto idx : mesh.indices)
        {
            m_localTransferSpace.indices[m_localTransferSpace.currentIndexCount++] = idx + indexOffset;
        }

        m_localTransferSpace.objectMetadata[m_localTransferSpace.currentObjectMetadataCount++].model = transform.toMat4();
    }

    void Renderer::resize(uint32_t width, uint32_t height)
    {
        // As per current implementation, there is nothing much to handle on resize.
        // However, this function is kept for future use
        m_defaultFramebuffer->resize(width, height);
    }

    void Renderer::setBatchSize(size_t batchSize)
    {
        auto& device = m_graphicsContext->getDevice();
        device.waitIdle();
        size_t currentModelCount = m_allocationBatchInfo.modelCount;
        m_allocationBatchInfo = BatchAllocationInfo(batchSize, currentModelCount);

        allocateLocalTransferSpace();
        createBuffers(); // re-create the buffers
    }

    size_t Renderer::getBatchSize() const
    {
        return m_allocationBatchInfo.batchSize;
    }

    const RendererStatistics& Renderer::getStats() const
    {
        return m_stats;
    }

    const vk::raii::RenderPass& Renderer::getRenderPass() const
    {
        return *m_firstRenderPass;
    }

    void Renderer::init()
    {
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createCommandPools();
        createSyncObjects();

        allocateLocalTransferSpace();
        createBuffers();
        createDescriptorPool();
        allocateDescriptorSets();
        createDefaultFramebuffer();
    }

    void Renderer::createRenderPass()
    {
        auto swapchainImageFormat = m_graphicsContext->getSwapchainImageFormat();
        auto& device = m_graphicsContext->getDevice();

        vk::AttachmentDescription colorAttachment{
            {},
            swapchainImageFormat,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR
        };

        vk::Format depthFormat = m_graphicsContext->findDepthFormat();
        vk::AttachmentDescription depthAttachment{
            {},
            depthFormat,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        };

        std::array attachments{colorAttachment, depthAttachment};

        vk::AttachmentReference colorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};
        vk::AttachmentReference depthAttachmentRef{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

        vk::SubpassDescription subpass{
            {},
            vk::PipelineBindPoint::eGraphics,
            0,
            nullptr,
            1,
            &colorAttachmentRef,
            nullptr,
            &depthAttachmentRef,
        };

        vk::SubpassDependency dependency{
            VK_SUBPASS_EXTERNAL,
            0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
        };

        vk::RenderPassCreateInfo renderPassInfo{
            {},
            static_cast<uint32_t>(attachments.size()),
            attachments.data(),
            1,
            &subpass,
            1,
            &dependency,
        };

        m_firstRenderPass = device.createRenderPass(renderPassInfo);

        colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
        colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;

        depthAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
        depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        attachments = {colorAttachment, depthAttachment};

        m_batchRenderPass = device.createRenderPass(renderPassInfo);
    }

    void Renderer::createDescriptorSetLayout()
    {
        auto& device = m_graphicsContext->getDevice();

        vk::DescriptorSetLayoutBinding cameraViewDataBinding{
            0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex,
        };

        vk::DescriptorSetLayoutBinding objectMetadataBinding{
            1,
            vk::DescriptorType::eStorageBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        };

        std::array layoutBindings{ cameraViewDataBinding, objectMetadataBinding };

        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
            {},
            static_cast<uint32_t>(layoutBindings.size()),
            layoutBindings.data()
        };

        m_descriptorSetLayout = device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
    }

    void Renderer::createGraphicsPipeline()
    {
        auto& device = m_graphicsContext->getDevice();

        const std::string vertexGlslSrc = Utils::readFile("Core/resources/shaders/renderer.vertex.glsl");
        const std::string fragmentGlslSrc = Utils::readFile("Core/resources/shaders/renderer.fragment.glsl");

        vk::raii::ShaderModule vertexShaderModule = Utils::createShaderModule(device,
            vk::ShaderStageFlagBits::eVertex,
            vertexGlslSrc);
        vk::raii::ShaderModule fragmentShaderModule = Utils::createShaderModule(device,
            vk::ShaderStageFlagBits::eFragment,
            fragmentGlslSrc);

        vk::PipelineShaderStageCreateInfo vertexShaderStage{
            {}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main"
        };
        vk::PipelineShaderStageCreateInfo fragmentShaderStage{
            {}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main"
        };
        vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStage, fragmentShaderStage };

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

        vk::VertexInputBindingDescription vertexInputBindingDescription{
            0,
            sizeof(Vertex),
        };

        vk::VertexInputBindingDescription internalVertexInputBindingDescription{
            1,
            sizeof(InternalVertex),
        };

        std::array vertexInputBindings{ vertexInputBindingDescription, internalVertexInputBindingDescription };

        vk::VertexInputAttributeDescription vertexInputAttributeDescription{
            0,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, position)
        };

        vk::VertexInputAttributeDescription internalVertexInputAttributeDescription{
            1,
            1,
            vk::Format::eR32Sint,
            0
        };

        std::array vertexInputAttributeDescriptions{ vertexInputAttributeDescription, internalVertexInputAttributeDescription };

        vertexInputInfo.setVertexBindingDescriptions(vertexInputBindings);
        vertexInputInfo.setVertexAttributeDescriptions(vertexInputAttributeDescriptions);

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
            {},
            vk::PrimitiveTopology::eTriangleList,
            VK_FALSE
        };

        vk::PipelineViewportStateCreateInfo viewportState{
            {},
            1,
            nullptr,
            1,
            nullptr
        };

        vk::PipelineRasterizationStateCreateInfo rasterizer{
            {},
            VK_FALSE,
            VK_FALSE,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            vk::FrontFace::eCounterClockwise,
            VK_FALSE,
            0,
            0,
            0,
            1.0
        };

        vk::PipelineMultisampleStateCreateInfo multisampling{
            {},
            vk::SampleCountFlagBits::e1,
            VK_FALSE
        };

        vk::PipelineDepthStencilStateCreateInfo depthStencilState{
            {},
            VK_TRUE,
            VK_TRUE,
            vk::CompareOp::eLess,
            VK_FALSE,
            VK_FALSE,
            {},
            {},
            0.0f,
            1.0f
        };

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            VK_FALSE,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA
        };

        vk::PipelineColorBlendStateCreateInfo colorBlending{
            {}, VK_FALSE,
            vk::LogicOp::eCopy,
            1,
            &colorBlendAttachment
        };

        std::array dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
        dynamicStateCreateInfo.setDynamicStates(dynamicStates);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayouts(**m_descriptorSetLayout);

        m_pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
            {},
            2,
            shaderStages,
            &vertexInputInfo,
            &inputAssembly,
            nullptr,
            &viewportState,
            &rasterizer,
            &multisampling,
            &depthStencilState,
            &colorBlending,
            &dynamicStateCreateInfo,
            **m_pipelineLayout,
            **m_firstRenderPass,
            0,
            vk::Pipeline(),
            -1
        };

        m_graphicsPipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }

    void Renderer::createCommandPools()
    {
        m_commandPools.clear();

        // Create renderer command pool per swapchain image
        auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();
        m_commandPools.reserve(swapchainImageViews.size());

        for (size_t i = 0; i < swapchainImageViews.size(); ++i)
        {
            m_commandPools.emplace_back(RendererCommandPool::makeShared(m_graphicsContext));
        }
    }

    void Renderer::createSyncObjects()
    {
        auto& device = m_graphicsContext->getDevice();
        auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();

        m_inFlightFrameManager = InFlightFrameManager(static_cast<uint32_t>(swapchainImageViews.size()));

        vk::FenceCreateInfo fenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled };

        for (size_t i = 0; i < swapchainImageViews.size(); i++)
        {
            auto& frame = m_inFlightFrameManager.frames.emplace_back();
            frame.imageAvailableSemaphore = device.createSemaphore({});
            frame.renderFinishedSemaphore = device.createSemaphore({});
            frame.inFlightFence = device.createFence(fenceCreateInfo);

            // TODO: This needs to be properly handled, because we can have n number of draw calls in a single frame.
            frame.batchSemaphores.reserve(m_allocationBatchInfo.modelCount);
            for (size_t j = 0; j < m_allocationBatchInfo.modelCount; j++)
            {
                frame.batchSemaphores.emplace_back(device, vk::SemaphoreCreateInfo{});
            }
        }
    }

    void Renderer::allocateLocalTransferSpace()
    {
        // Initialize local buffer allocation
        m_localTransferSpace.setBatchInfo(m_allocationBatchInfo);
    }

    void Renderer::createBuffers()
    {
        // vertex buffer
        vk::DeviceSize bufferSize = m_allocationBatchInfo.getVerticesSizeInBytes();
        m_vertexBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        // index buffer
        bufferSize = m_allocationBatchInfo.getIndicesSizeInBytes();
        m_indexBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eIndexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        // camera view data uniform buffer
        bufferSize = sizeof(CameraViewData);
        m_cameraViewDataBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        // object metadata storage buffer
        bufferSize = sizeof(ObjectMetadata) * m_allocationBatchInfo.modelCount;
        m_objectMetadataBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eStorageBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        // internal vertex buffer
        bufferSize = m_allocationBatchInfo.getInternalVertexSizeInBytes();
        m_internalVertexBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    }

    void Renderer::createDescriptorPool()
    {
        auto& device = m_graphicsContext->getDevice();

        vk::DescriptorPoolSize cameraViewDataPoolSize{
            vk::DescriptorType::eUniformBuffer,
            1,
        };

        vk::DescriptorPoolSize objectMetadataPoolSize{
            vk::DescriptorType::eStorageBuffer,
            1
        };

        std::array poolSizes{ cameraViewDataPoolSize, objectMetadataPoolSize };

        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            1,
            static_cast<uint32_t>(poolSizes.size()),
            poolSizes.data()
        };

        m_descriptorPool = device.createDescriptorPool(descriptorPoolCreateInfo);
    }

    void Renderer::allocateDescriptorSets()
    {
        auto& device = m_graphicsContext->getDevice();

        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{
            **m_descriptorPool,
            1,
            &**m_descriptorSetLayout,
        };

        m_descriptorSet = device.allocateDescriptorSets(descriptorSetAllocateInfo);
        ASSERT(m_descriptorSet.size(), "Failed to allocate descriptor sets!");

        vk::DescriptorBufferInfo cameraViewDataBufferInfo{
            *m_cameraViewDataBuffer->getNativeBuffer(),
            0,
            sizeof(CameraViewData)
        };

        vk::WriteDescriptorSet cameraViewDataWriteDescriptorSet{
            *m_descriptorSet[0],
            0,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            nullptr,
            &cameraViewDataBufferInfo
        };

        vk::DescriptorBufferInfo objectMetadataBufferInfo{
            *m_objectMetadataBuffer->getNativeBuffer(),
            0,
            sizeof(ObjectMetadata) * m_allocationBatchInfo.modelCount
        };

        vk::WriteDescriptorSet objectMetadataWriteDescriptorSet{
            *m_descriptorSet[0],
            1,
            0,
            1,
            vk::DescriptorType::eStorageBuffer,
            nullptr,
            &objectMetadataBufferInfo
        };

        std::array descriptorWrites{ objectMetadataWriteDescriptorSet, cameraViewDataWriteDescriptorSet };

        device.updateDescriptorSets(descriptorWrites, nullptr);
    }

    void Renderer::createDefaultFramebuffer()
    {
        vk::Format depthFormat = m_graphicsContext->findDepthFormat();
        FramebufferAttachmentInfo depthAttachmentInfo{
            depthFormat,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::ImageAspectFlagBits::eDepth,
            vk::ClearValue( vk::ClearDepthStencilValue(1.0f, 0) )
        };
        m_defaultFramebuffer = SwapchainFramebuffer::makeShared(m_graphicsContext,
            *m_firstRenderPass,
            depthAttachmentInfo);
    }

    void Renderer::recordCommands(const vk::raii::CommandBuffer& cb, const Framebuffer::Shared& fb) const
    {
        const auto& frameContext = m_inFlightFrameManager.getCurrentFrameContext();

        cb.reset();

        vk::CommandBufferBeginInfo beginInfo{};
        cb.begin(beginInfo);

        auto swapchainExtent = m_graphicsContext->getSwapchainExtent();

        vk::Viewport viewport{
            0,
            0,
            static_cast<float>(swapchainExtent.width),
            static_cast<float>(swapchainExtent.height),
            0.0f,
            1.0f
        };

        vk::Rect2D scissor{
            {0, 0},
            swapchainExtent
        };

        cb.setViewport(0, viewport);
        cb.setScissor(0, scissor);

        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.framebuffer = *(fb->getBuffer());
        renderPassInfo.renderArea = vk::Rect2D{ { 0, 0 }, swapchainExtent };

        if (frameContext.drawCallCount == 0) // Very first draw call
        {
            const auto& clearValues = fb->getClearValues();
            renderPassInfo.renderPass = **m_firstRenderPass;
            renderPassInfo.setClearValues(clearValues);
        }
        else
        {
            renderPassInfo.renderPass = **m_batchRenderPass;
        }

        cb.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        cb.bindPipeline(vk::PipelineBindPoint::eGraphics, **m_graphicsPipeline);

        vk::Buffer vertexBuffers[] = { *m_vertexBuffer->getNativeBuffer(), *m_internalVertexBuffer->getNativeBuffer() };
        vk::DeviceSize offsets[] = { 0, 0 };
        cb.bindVertexBuffers(0, vertexBuffers, offsets);

        cb.bindIndexBuffer(*m_indexBuffer->getNativeBuffer(), 0, vk::IndexType::eUint32);

        cb.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            **m_pipelineLayout,
            0,
            *m_descriptorSet[0],
            nullptr);

        cb.drawIndexed(static_cast<uint32_t>(m_localTransferSpace.currentIndexCount),
            1, 0, 0, 0);

        cb.endRenderPass();
        cb.end();
    }

    void Renderer::draw()
    {
        auto& graphicsQueue = m_graphicsContext->getGraphicsQueue();
        auto& frameContext = m_inFlightFrameManager.getCurrentFrameContext();
        auto& framebuffers = m_framebuffer->getBuffers();

        // Record the commands
        const auto& frameCommandPool = m_commandPools[frameContext.imageIndex];
        auto& fb = framebuffers[frameContext.imageIndex];
        const auto& cb = m_commandPools[frameContext.imageIndex]->getNextAvailablePrimaryCommandBuffer();
        recordCommands(cb, fb);

        // Copy from local allocation to the dedicated vk buffers
        void* data = m_vertexBuffer->map(m_localTransferSpace.getCurrentVerticesSizeInBytes(), 0);
        memcpy(data, m_localTransferSpace.vertices, m_localTransferSpace.getCurrentVerticesSizeInBytes());
        m_vertexBuffer->unmap();

        data = m_indexBuffer->map(m_localTransferSpace.getCurrentIndicesSizeInBytes(), 0);
        memcpy(data, m_localTransferSpace.indices, m_localTransferSpace.getCurrentIndicesSizeInBytes());
        m_indexBuffer->unmap();

        data = m_objectMetadataBuffer->map(m_localTransferSpace.getCurrentObjectMetadataSizeInBytes(), 0);
        memcpy(data, m_localTransferSpace.objectMetadata, m_localTransferSpace.getCurrentObjectMetadataSizeInBytes());
        m_objectMetadataBuffer->unmap();

        data = m_internalVertexBuffer->map(m_localTransferSpace.getCurrentIntervalVerticesSizeInBytes(), 0);
        memcpy(data, m_localTransferSpace.internalVertices, m_localTransferSpace.getCurrentIntervalVerticesSizeInBytes());
        m_internalVertexBuffer->unmap();

        vk::SubmitInfo submitInfo{};

        vk::Semaphore waitSemaphore, signalSemaphore;
        if (frameContext.drawCallCount == 0 && frameContext.isLastDrawCall) // First and last draw call
        {
            waitSemaphore = **frameContext.imageAvailableSemaphore;
            signalSemaphore = **frameContext.renderFinishedSemaphore;
        }
        else if (frameContext.drawCallCount == 0) // Very first draw call but not the last
        {
            waitSemaphore = **frameContext.imageAvailableSemaphore;
            signalSemaphore = *frameContext.batchSemaphores[frameContext.drawCallCount];
        }
        else if (frameContext.isLastDrawCall) // Very last draw call
        {
            waitSemaphore = *frameContext.batchSemaphores[frameContext.drawCallCount - 1];
            signalSemaphore = **frameContext.renderFinishedSemaphore;
        }
        else
        {
            waitSemaphore = *frameContext.batchSemaphores[frameContext.drawCallCount - 1];
            signalSemaphore = *frameContext.batchSemaphores[frameContext.drawCallCount];;
        }

        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.setWaitDstStageMask(waitStages);
        submitInfo.setWaitSemaphores(waitSemaphore);
        submitInfo.setCommandBuffers(*cb);
        submitInfo.setSignalSemaphores(signalSemaphore);

        vk::Fence inFlightFence = VK_NULL_HANDLE;
        if (frameContext.isLastDrawCall)
        {
            inFlightFence = **frameContext.inFlightFence;
        }
        graphicsQueue.submit(submitInfo, inFlightFence);

        // Update the frame context counts
        frameContext.drawCallCount++;

        // Update the statistics
        m_stats.totalDrawCallsCount++;
        m_stats.totalVertexCount += m_localTransferSpace.currentVertexCount;
        m_stats.totalIndexCount += m_localTransferSpace.currentIndexCount;

        // Reset local allocation counts
        m_localTransferSpace.resetCurrentCounts();
    }

    void Renderer::presentFrame()
    {
        auto& swapchain = m_graphicsContext->getSwapchain();
        auto& presentQueue = m_graphicsContext->getPresentQueue();
        const auto& frameContext = m_inFlightFrameManager.getCurrentFrameContext();

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(**frameContext.renderFinishedSemaphore);
        presentInfo.setSwapchains(*swapchain);
        presentInfo.setImageIndices(frameContext.imageIndex);

        auto _ = presentQueue.presentKHR(presentInfo);
    }
} // LearnVulkanRAII