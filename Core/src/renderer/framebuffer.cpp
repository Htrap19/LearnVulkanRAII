//
// Created by User on 10/13/2025.
//

#include "framebuffer.h"

namespace LearnVulkanRAII
{
    Framebuffer::Framebuffer(const GraphicsContext::Shared &graphicsContext, const vk::raii::RenderPass& renderPass)
        : m_graphicsContext(graphicsContext),
        m_renderPass(renderPass)
    {
        init();
    }

    void Framebuffer::init()
    {
        auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();
        auto swapchainImageExtent = m_graphicsContext->getSwapchainExtent();
        auto& device = m_graphicsContext->getDevice();

        m_framebuffers.clear();

        for (auto& iv : swapchainImageViews)
        {
            vk::ImageView ivRaw = *iv;
            vk::FramebufferCreateInfo framebufferCreateInfo{
                {},
                *m_renderPass,
                1,
                &ivRaw,
                swapchainImageExtent.width,
                swapchainImageExtent.height,
                1
            };
            m_framebuffers.emplace_back(device, framebufferCreateInfo);
        }
    }
} // LearnVulkanRAII