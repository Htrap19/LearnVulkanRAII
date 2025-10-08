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

        const std::string vertexGlslSrc = Utils::readFile("Core/resources/shaders/renderer.vertex.glsl");
        const std::string fragmentGlslSrc = Utils::readFile("Core/resources/shaders/renderer.fragment.glsl");

        vk::raii::ShaderModule vertexShaderModule = Utils::createShaderModule(device,
                                                                              vk::ShaderStageFlagBits::eVertex,
                                                                              vertexGlslSrc);
        vk::raii::ShaderModule fragmentShaderModule = Utils::createShaderModule(device,
                                                                                vk::ShaderStageFlagBits::eFragment,
                                                                                fragmentGlslSrc);
    }
} // LearnVulkanRAII