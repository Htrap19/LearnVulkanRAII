//
// Created by User on 10/6/2025.
//

#include "renderer.h"

#include "mesh/mesh.h"
#include "renderer/utils/utils.h"

#include <glm/glm.hpp>

namespace LearnVulkanRAII
{
    Renderer::Renderer(const GraphicsContext::Shared& graphicsContext)
        : m_graphicsContext(graphicsContext),
        m_allocationBatchInfo(500)
    {
        init();
    }

    void Renderer::beginFrame(const Framebuffer::Shared& framebuffer, const CameraViewData& cameraData)
    {
        auto& framebuffers = framebuffer->getBuffers();
        ASSERT(m_commandBuffers.size() == framebuffers.size(), "Framebuffer seems incompatible!");
        m_framebuffer = framebuffer;
        m_cameraViewData = cameraData;
    }

    void Renderer::endFrame()
    {
        drawFrame();
    }

    void Renderer::drawMesh(const Mesh& mesh)
    {
        if (m_localAllocation.getCurrentFaceCounts() + mesh.getFaceCount() > m_allocationBatchInfo.batchSize)
        {
            // flush
            drawFrame();
        }

        size_t indexOffset = m_localAllocation.currentVertexCount;
        memcpy((m_localAllocation.vertices + indexOffset), mesh.vertices.data(), mesh.getVerticesSizeInBytes());
        m_localAllocation.currentVertexCount += mesh.getVerticesCount();

        for (const auto idx : mesh.indices)
        {
            m_localAllocation.indices[m_localAllocation.currentIndexCount++] = idx + indexOffset;
        }
    }

    void Renderer::resize(uint32_t width, uint32_t height)
    {
        // As per current implementation, there is nothing much to handle on resize.
        // However, this function is kept for future use
    }

    void Renderer::setBatchSize(size_t batchSize)
    {
        auto& device = m_graphicsContext->getDevice();
        device.waitIdle();
        m_allocationBatchInfo = AllocationBatchInfo(batchSize);

        createBuffers(); // re-create the buffers
    }

    size_t Renderer::getBatchSize() const
    {
        return m_allocationBatchInfo.batchSize;
    }

    const vk::raii::RenderPass& Renderer::getRenderPass() const
    {
        return *m_renderPass;
    }

    void Renderer::init()
    {
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        allocateCommandBuffers();
        createSyncObjects();

        createBuffers();
        createDescriptorPool();
        allocateDescriptorSets();
    }

    void Renderer::createRenderPass()
    {
        auto swapchainImageFormat = m_graphicsContext->getSwapchainImageFormat();
        auto& device = m_graphicsContext->getDevice();

        vk::AttachmentDescription colorAttachment = {
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

        vk::AttachmentReference colorAttachmentRef = {
            0, vk::ImageLayout::eColorAttachmentOptimal
        };

        vk::SubpassDescription subpass = {
            {},
            vk::PipelineBindPoint::eGraphics,
            0,
            nullptr,
            1,
            &colorAttachmentRef,
        };

        vk::SubpassDependency dependency = {
            VK_SUBPASS_EXTERNAL,
            0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
        };

        vk::RenderPassCreateInfo renderPassInfo = {
            {},
            1,
            &colorAttachment,
            1,
            &subpass,
            1,
            &dependency,
        };

        m_renderPass = device.createRenderPass(renderPassInfo);
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

        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
            {},
            1,
            &cameraViewDataBinding
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

        vk::VertexInputAttributeDescription vertexInputAttributeDescription{
            0,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, position)
        };

        vertexInputInfo.setVertexBindingDescriptions(vertexInputBindingDescription);
        vertexInputInfo.setVertexAttributeDescriptions(vertexInputAttributeDescription);

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
            vk::FrontFace::eClockwise,
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
            nullptr,
            &colorBlending,
            &dynamicStateCreateInfo,
            **m_pipelineLayout,
            **m_renderPass,
            0,
            vk::Pipeline(),
            -1
        };

        m_graphicsPipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }

    void Renderer::allocateCommandBuffers()
    {
        auto& commandPool = m_graphicsContext->getCommandPool();
        auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();
        auto& device = m_graphicsContext->getDevice();

        vk::CommandBufferAllocateInfo allocInfo{
            *commandPool,
            vk::CommandBufferLevel::ePrimary,
            static_cast<uint32_t>(swapchainImageViews.size())
        };

        m_commandBuffers = device.allocateCommandBuffers(allocInfo);
    }

    void Renderer::createSyncObjects()
    {
        auto& device = m_graphicsContext->getDevice();

        m_imageAvailableSemaphore = device.createSemaphore({});
        m_renderFinishedSemaphore = device.createSemaphore({});

        vk::FenceCreateInfo fenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled };
        m_inFlightFence = device.createFence(fenceCreateInfo);
    }

    void Renderer::createBuffers()
    {
        // Initialize local buffer allocation
        m_localAllocation.setBatchInfo(m_allocationBatchInfo);

        // vertex buffer
        vk::DeviceSize bufferSize = m_allocationBatchInfo.getVerticesSize();
        m_vertexBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        // index buffer
        bufferSize = m_allocationBatchInfo.getIndicesSize();
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
    }

    void Renderer::createDescriptorPool()
    {
        auto& device = m_graphicsContext->getDevice();

        vk::DescriptorPoolSize descriptorPoolSize{
            vk::DescriptorType::eUniformBuffer,
            1,
        };

        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            1,
            1,
            &descriptorPoolSize
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

        vk::DescriptorBufferInfo descriptorBufferInfo{
            *m_cameraViewDataBuffer->getNativeBuffer(),
            0,
            sizeof(CameraViewData)
        };

        vk::WriteDescriptorSet writeDescriptorSet{
            *m_descriptorSet[0],
            0,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            nullptr,
            &descriptorBufferInfo
        };

        device.updateDescriptorSets(writeDescriptorSet, nullptr);
    }

    void Renderer::recordCommandBuffer(const vk::raii::CommandBuffer& cb, const vk::raii::Framebuffer& fb) const
    {
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

        vk::ClearValue clearValue{
            vk::ClearColorValue{ std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f} }
        };

        vk::RenderPassBeginInfo renderPassInfo{
            **m_renderPass,
            *fb,
            { { 0, 0 }, swapchainExtent },
            1,
            &clearValue
        };

        cb.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        cb.bindPipeline(vk::PipelineBindPoint::eGraphics, **m_graphicsPipeline);

        vk::Buffer vertexBuffers[] = { *m_vertexBuffer->getNativeBuffer() };
        vk::DeviceSize offsets[] = { 0 };
        cb.bindVertexBuffers(0, vertexBuffers, offsets);

        cb.bindIndexBuffer(*m_indexBuffer->getNativeBuffer(), 0, vk::IndexType::eUint32);

        cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, **m_pipelineLayout, 0, *m_descriptorSet[0], nullptr);

        cb.drawIndexed(static_cast<uint32_t>(m_localAllocation.currentIndexCount),
            1, 0, 0, 0);

        cb.endRenderPass();
        cb.end();
    }

    void Renderer::drawFrame()
    {
        // Draw a frame
        auto& device = m_graphicsContext->getDevice();
        auto& swapchain = m_graphicsContext->getSwapchain();
        auto& graphicsQueue = m_graphicsContext->getGraphicsQueue();
        auto& presentQueue = m_graphicsContext->getPresentQueue();
        auto& framebuffers = m_framebuffer->getBuffers();

        auto _ = device.waitForFences(**m_inFlightFence, VK_TRUE, UINT64_MAX);
        device.resetFences(**m_inFlightFence);

        // Copy from local allocation to the dedicated vk buffers
        void* data = m_vertexBuffer->map(m_localAllocation.getCurrentVerticesSizeInBytes(), 0);
        memcpy(data, m_localAllocation.vertices, m_localAllocation.getCurrentVerticesSizeInBytes());
        m_vertexBuffer->unmap();

        data = m_indexBuffer->map(m_localAllocation.getCurrentIndicesSizeInBytes(), 0);
        memcpy(data, m_localAllocation.indices, m_localAllocation.getCurrentIndicesSizeInBytes());
        m_indexBuffer->unmap();

        data = m_cameraViewDataBuffer->map();
        memcpy(data, &m_cameraViewData, sizeof(CameraViewData));
        m_cameraViewDataBuffer->unmap();

        auto [result, imageIndex] =
            swapchain.acquireNextImage(UINT64_MAX, **m_imageAvailableSemaphore);

        ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR,
            "Failed to acquire swapchain image!");

        auto& cb = m_commandBuffers[imageIndex];
        auto& fb = framebuffers[imageIndex];
        recordCommandBuffer(cb, fb);

        vk::SubmitInfo submitInfo{};

        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.setWaitDstStageMask(waitStages);
        submitInfo.setWaitSemaphores(**m_imageAvailableSemaphore);
        submitInfo.setCommandBuffers(*cb);
        submitInfo.setSignalSemaphores(**m_renderFinishedSemaphore);

        graphicsQueue.submit(submitInfo, **m_inFlightFence);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(**m_renderFinishedSemaphore);
        presentInfo.setSwapchains(*swapchain);
        presentInfo.setImageIndices(imageIndex);

        _ = presentQueue.presentKHR(presentInfo);

        // Reset local allocation counts
        m_localAllocation.resetCurrentCounts();
    }
} // LearnVulkanRAII