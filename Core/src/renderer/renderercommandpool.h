//
// Created by User on 10/28/2025.
//

#ifndef LEARNVULKANRAII_RENDERERCOMMANDPOOL_H
#define LEARNVULKANRAII_RENDERERCOMMANDPOOL_H

#include "base/graphicscontext.h"
#include "base/utils.h"

#include <vulkan/vulkan_raii.hpp>

#include <vector>

namespace LearnVulkanRAII
{
    class RendererCommandPool
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(RendererCommandPool)

    public:
        RendererCommandPool() = default;
        explicit RendererCommandPool(const GraphicsContext::Shared& graphicsContext);

        vk::raii::CommandBuffer& getNextAvailablePrimaryCommandBuffer();
        vk::raii::CommandBuffer& getPrimaryCommandBuffer(size_t commandBufferIndex);
        vk::raii::CommandBuffer& getNextAvailableSecondaryCommandBuffer();
        vk::raii::CommandBuffer& getSecondaryCommandBuffer(size_t commandBufferIndex);

        [[nodiscard]] const std::vector<vk::raii::CommandBuffer>& getPrimaryCommandBuffers() const;
        [[nodiscard]] const std::vector<vk::CommandBuffer>& getRawPrimaryCommandBuffers() const;

        void reset();

    private:
        void init();
        [[nodiscard]] vk::raii::CommandBuffer allocateCommandBuffer(vk::CommandBufferLevel level) const;

    private:
        GraphicsContext::Shared m_graphicsContext;
        Utils::Optional<vk::raii::CommandPool> m_commandPool;

        std::vector<vk::raii::CommandBuffer> m_primaryCommandBuffers;
        std::vector<vk::CommandBuffer> m_rawPrimaryCommandBuffers;
        size_t m_primaryNextAvailable = 0;

        std::vector<vk::raii::CommandBuffer> m_secondaryCommandBuffers;
        std::vector<vk::CommandBuffer> m_rawSecondaryCommandBuffers;
        size_t m_secondaryNextAvailable = 0;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_RENDERERCOMMANDPOOL_H