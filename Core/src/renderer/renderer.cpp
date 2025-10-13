//
// Created by User on 10/6/2025.
//

#include "renderer.h"

#include "renderer/utils/utils.h"

namespace LearnVulkanRAII
{
    Renderer::Renderer(const GraphicsContext::Shared& graphicsContext)
        : m_graphicsContext(graphicsContext)
    {
        init();
    }

    const vk::raii::RenderPass& Renderer::getRenderPass() const
    {
        return *m_renderPass;
    }

    void Renderer::init()
    {
        createRenderPass();
        createGraphicsPipeline();
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

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
            {},
            0,
            nullptr,
            0,
            nullptr
        };

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
} // LearnVulkanRAII