//
// Created by User on 10/6/2025.
//

#ifndef LEARNVULKANRAII_RENDERER_H
#define LEARNVULKANRAII_RENDERER_H

#include "base/utils.h"
#include "base/graphicscontext.h"

#include "framebuffer.h"

#include <vulkan/vulkan_raii.hpp>

namespace LearnVulkanRAII
{
    class Renderer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Renderer)

    public:
        explicit Renderer(const GraphicsContext::Shared& graphicsContext);

        void beginFrame(const Framebuffer::Shared& framebuffer);
        void endFrame();

        void onUpdate();

        [[nodiscard]] const vk::raii::RenderPass& getRenderPass() const;

        // TODO: Need to create a 'create' function for renderer
        // static Shared create(...);

    private:
        void init();

        void createRenderPass();
        void createGraphicsPipeline();
        void allocateCommandBuffers();
        void createSyncObjects();

        void createVertexBuffer();
        void recordCommandBuffer(const vk::raii::CommandBuffer& cb, const vk::raii::Framebuffer& fb) const;

        void drawFrame() const;

        uint32_t findMemoryType(uint32_t typeFilters, vk::MemoryPropertyFlags properties) const;

    private:
        GraphicsContext::Shared m_graphicsContext;

        Utils::Optional<vk::raii::RenderPass> m_renderPass;
        Utils::Optional<vk::raii::PipelineLayout> m_pipelineLayout;
        Utils::Optional<vk::raii::Pipeline> m_graphicsPipeline;
        std::vector<vk::raii::CommandBuffer> m_commandBuffers;
        Utils::Optional<vk::raii::Semaphore> m_imageAvailableSemaphore;
        Utils::Optional<vk::raii::Semaphore> m_renderFinishedSemaphore;
        Utils::Optional<vk::raii::Fence> m_inFlightFence;

        // TODO: Need to move this into it's own dedicated class
        Utils::Optional<vk::raii::Buffer> m_vertexBuffer;
        Utils::Optional<vk::raii::DeviceMemory> m_vertexBufferMemory;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_RENDERER_H