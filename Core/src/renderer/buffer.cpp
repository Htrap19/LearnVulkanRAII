//
// Created by User on 10/14/2025.
//

#include "buffer.h"

namespace LearnVulkanRAII
{
    Buffer::Buffer(const GraphicsContext::Shared& graphicsContext,
                   vk::DeviceSize bufferSize,
                   vk::BufferUsageFlags usage,
                   vk::MemoryPropertyFlags memoryPropertyFlags,
                   vk::SharingMode sharingMode)
        : m_graphicsContext(graphicsContext),
        m_bufferSize(bufferSize),
        m_usage(usage),
        m_memoryPropertyFlags(memoryPropertyFlags),
        m_sharingMode(sharingMode)
    {
        init();
    }

    void* Buffer::map() const
    {
        auto& device = m_graphicsContext->getDevice();

        vk::MemoryMapInfo memoryMapInfo{
            {},
            **m_bufferMemory,
            0,
            m_bufferSize
        };

        return device.mapMemory2(memoryMapInfo);
    }

    void Buffer::unmap() const
    {
        auto& device = m_graphicsContext->getDevice();

        vk::MemoryUnmapInfo memoryUnmapInfo{
            {},
            **m_bufferMemory
        };
        device.unmapMemory2(memoryUnmapInfo);
    }

    const vk::raii::Buffer& Buffer::getNativeBuffer() const
    {
        return *m_buffer;
    }

    Buffer::Shared Buffer::create(const GraphicsContext::Shared& graphicsContext,
                                  vk::DeviceSize bufferSize,
                                  vk::BufferUsageFlags usage,
                                  vk::MemoryPropertyFlags memoryPropertyFlags,
                                  vk::SharingMode sharingMode)
    {
        return makeShared(graphicsContext,
                      bufferSize,
                      usage,
                      memoryPropertyFlags,
                      sharingMode);
    }

    void Buffer::init()
    {
        auto& device = m_graphicsContext->getDevice();

        vk::BufferCreateInfo bufferCreateInfo{
            {},
            m_bufferSize,
            m_usage,
            m_sharingMode
        };

        m_buffer = device.createBuffer(bufferCreateInfo);

        auto memRequirements = device.getBufferMemoryRequirements({ &bufferCreateInfo });
        uint32_t memoryType = findMemoryType(memRequirements.memoryRequirements.memoryTypeBits, m_memoryPropertyFlags);

        vk::MemoryAllocateInfo allocInfo{
            memRequirements.memoryRequirements.size,
            memoryType
        };
        m_bufferMemory = device.allocateMemory(allocInfo);

        vk::BindBufferMemoryInfo bindBufferMemoryInfo{
            **m_buffer,
            **m_bufferMemory,
            0
        };

        device.bindBufferMemory2(bindBufferMemoryInfo);
    }

    uint32_t Buffer::findMemoryType(uint32_t typeFilters, vk::MemoryPropertyFlags properties) const
    {
        auto& physicalDevice = m_graphicsContext->getPhysicalDevice();
        auto memProperties = physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilters & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        ASSERT(false, "Failed to find suitable memory type!");
        return 0;
    }
} // LearnVulkanRAII