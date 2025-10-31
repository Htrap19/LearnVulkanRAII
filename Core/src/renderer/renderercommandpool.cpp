//
// Created by User on 10/28/2025.
//

#include "renderercommandpool.h"

namespace LearnVulkanRAII
{
    RendererCommandPool::RendererCommandPool(const GraphicsContext::Shared& graphicsContext)
        : m_graphicsContext(graphicsContext)
    {
        init();
    }

    vk::raii::CommandBuffer& RendererCommandPool::getNextAvailablePrimaryCommandBuffer()
    {
        if (m_primaryNextAvailable < m_primaryCommandBuffers.size())
        {
            return m_primaryCommandBuffers[m_primaryNextAvailable++];
        }

        auto commandBuffer = allocateCommandBuffer(vk::CommandBufferLevel::ePrimary);

        m_primaryNextAvailable++;
        return m_primaryCommandBuffers.emplace_back(std::move(commandBuffer));
    }

    vk::raii::CommandBuffer& RendererCommandPool::getPrimaryCommandBuffer(size_t commandBufferIndex)
    {
        ASSERT(commandBufferIndex < m_primaryCommandBuffers.size(), "Command buffer index out of range!");
        return m_primaryCommandBuffers[commandBufferIndex];
    }

    vk::raii::CommandBuffer& RendererCommandPool::getNextAvailableSecondaryCommandBuffer()
    {
        if (m_secondaryNextAvailable < m_secondaryCommandBuffers.size())
        {
            return m_secondaryCommandBuffers[m_secondaryNextAvailable++];
        }

        auto commandBuffer = allocateCommandBuffer(vk::CommandBufferLevel::eSecondary);

        m_secondaryNextAvailable++;
        return m_secondaryCommandBuffers.emplace_back(std::move(commandBuffer));
    }

    vk::raii::CommandBuffer& RendererCommandPool::getSecondaryCommandBuffer(size_t commandBufferIndex)
    {
        ASSERT(commandBufferIndex < m_secondaryCommandBuffers.size(), "Command buffer index out of range!");
        return m_secondaryCommandBuffers[commandBufferIndex];
    }

    void RendererCommandPool::reset()
    {
        m_primaryNextAvailable = 0;
        m_secondaryNextAvailable = 0;
        m_commandPool->reset();
    }

    void RendererCommandPool::init()
    {
        auto& device = m_graphicsContext->getDevice();
        auto [graphicsQueueFamilyIndex, _] = m_graphicsContext->getQueueFamilyIndices();

        vk::CommandPoolCreateInfo poolInfo{
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            static_cast<uint32_t>(graphicsQueueFamilyIndex)
        };

        m_commandPool = device.createCommandPool(poolInfo);
    }

    vk::raii::CommandBuffer RendererCommandPool::allocateCommandBuffer(vk::CommandBufferLevel level) const
    {
        auto& device = m_graphicsContext->getDevice();

        vk::CommandBufferAllocateInfo allocateInfo{ **m_commandPool, level, 1 };

        auto commandBuffers = device.allocateCommandBuffers(allocateInfo);
        ASSERT(commandBuffers.size(), "Failed to allocate command buffer!");

        return std::move(commandBuffers[0]);
    }
} // LearnVulkanRAII