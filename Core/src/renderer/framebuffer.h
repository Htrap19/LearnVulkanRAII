//
// Created by User on 10/13/2025.
//

#ifndef LEARNVULKANRAII_FRAMEBUFFER_H
#define LEARNVULKANRAII_FRAMEBUFFER_H

#include "base/graphicscontext.h"
#include "base/utils.h"

#include "renderer.h"

namespace LearnVulkanRAII
{
    class Framebuffer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Framebuffer)

    public:
        explicit Framebuffer(const GraphicsContext::Shared& graphicsContext, const vk::raii::RenderPass& renderPass);

    private:
        void init();

    private:
        GraphicsContext::Shared m_graphicsContext;
        const vk::raii::RenderPass& m_renderPass;

        std::vector<vk::raii::Framebuffer> m_framebuffers;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_FRAMEBUFFER_H