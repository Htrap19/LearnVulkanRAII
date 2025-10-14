//
// Created by User on 10/6/2025.
//

#include "renderer.h"

#include "mesh/mesh.h"
#include "renderer/utils/utils.h"

// TODO: Need to remove this, and use the Mesh for drawing
static std::vector vertices = {
    0.0f, -0.5f,  // vertex 1 pos
    0.5f,  0.5f,  // vertex 2 pos
   -0.5f,  0.5f   // vertex 3 pos
};

namespace LearnVulkanRAII
{
    Renderer::Renderer(const GraphicsContext::Shared& graphicsContext)
        : m_graphicsContext(graphicsContext)
    {
        init();
    }

    void Renderer::beginFrame(const Framebuffer::Shared& framebuffer)
    {
        auto& framebuffers = framebuffer->getBuffers();
        ASSERT(m_commandBuffers.size() == framebuffers.size(), "Framebuffer seems incompatible!");

        for (size_t i = 0; i < m_commandBuffers.size(); i++)
        {
            auto& cb = m_commandBuffers[i];
            auto& fb = framebuffers[i];

            recordCommandBuffer(cb, fb);
        }
    }

    void Renderer::endFrame()
    {
        drawFrame();
    }

    const vk::raii::RenderPass& Renderer::getRenderPass() const
    {
        return *m_renderPass;
    }

    void Renderer::init()
    {
        createRenderPass();
        createGraphicsPipeline();
        allocateCommandBuffers();
        createSyncObjects();

        createVertexBuffer();
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

    void Renderer::createGraphicsPipeline()
    {
        auto& device = m_graphicsContext->getDevice();
        auto swapchainImageExtent = m_graphicsContext->getSwapchainExtent();

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
            sizeof(glm::vec3)
        };

        vk::VertexInputAttributeDescription vertexInputAttributeDescription{
            0,
            0,
            vk::Format::eR32G32B32Sfloat
        };

        vertexInputInfo.setVertexBindingDescriptions(vertexInputBindingDescription);
        vertexInputInfo.setVertexAttributeDescriptions(vertexInputAttributeDescription);

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
            {},
            vk::PrimitiveTopology::eTriangleList,
            VK_FALSE
        };

        vk::Viewport viewport{
            0.0f, 0.0f,
            static_cast<float>(swapchainImageExtent.width),
            static_cast<float>(swapchainImageExtent.height),
            0.0f, 1.0f
        };

        vk::Rect2D scissor{
            {0, 0},
            swapchainImageExtent
        };

        vk::PipelineViewportStateCreateInfo viewportState{
            {},
            1,
            &viewport, 1,
            &scissor
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

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        m_pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

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
            nullptr,
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

    void Renderer::createVertexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        m_vertexBuffer = Buffer::create(
            m_graphicsContext,
            bufferSize,
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        void* data = m_vertexBuffer->map();
        memcpy(data, vertices.data(), bufferSize);
        m_vertexBuffer->unmap();
    }

    void Renderer::recordCommandBuffer(const vk::raii::CommandBuffer& cb, const vk::raii::Framebuffer& fb) const
    {
        vk::CommandBufferBeginInfo beginInfo{};
        cb.begin(beginInfo);

        auto swapchainExtent = m_graphicsContext->getSwapchainExtent();

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

        cb.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

        cb.endRenderPass();
        cb.end();
    }

    void Renderer::drawFrame() const
    {
        auto& device = m_graphicsContext->getDevice();
        auto& swapchain = m_graphicsContext->getSwapchain();
        auto& graphicsQueue = m_graphicsContext->getGraphicsQueue();
        auto& presentQueue = m_graphicsContext->getPresentQueue();

        auto _ = device.waitForFences(**m_inFlightFence, VK_TRUE, UINT64_MAX);
        device.resetFences(**m_inFlightFence);

        auto [result, imageIndex] =
            swapchain.acquireNextImage(UINT64_MAX, **m_imageAvailableSemaphore);

        ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR,
               "Failed to acquire swapchain image!");

        vk::SubmitInfo submitInfo{};

        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.setWaitDstStageMask(waitStages);
        submitInfo.setWaitSemaphores(**m_imageAvailableSemaphore);
        submitInfo.setCommandBuffers(*m_commandBuffers[imageIndex]);
        submitInfo.setSignalSemaphores(**m_renderFinishedSemaphore);

        graphicsQueue.submit(submitInfo, **m_inFlightFence);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(**m_renderFinishedSemaphore);
        presentInfo.setSwapchains(*swapchain);
        presentInfo.setImageIndices(imageIndex);

        _ = presentQueue.presentKHR(presentInfo);
    }
} // LearnVulkanRAII