//
// Created by User on 10/14/2025.
//

#ifndef LEARNVULKANRAII_BUFFER_H
#define LEARNVULKANRAII_BUFFER_H

#include "base/utils.h"
#include "base/graphicscontext.h"

namespace LearnVulkanRAII
{
    class Buffer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Buffer)

    public:
        Buffer(const GraphicsContext::Shared& graphicsContext,
            vk::DeviceSize bufferSize,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags memoryPropertyFlags,
            vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

        void* map() const;
        void* map(vk::DeviceSize bufferSize, vk::DeviceSize offset) const;
        void unmap() const;

        const vk::raii::Buffer& getNativeBuffer() const;

        static Shared create(const GraphicsContext::Shared& graphicsContext,
            vk::DeviceSize bufferSize,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags memoryPropertyFlags,
            vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

    private:
        void init();

        uint32_t findMemoryType(uint32_t typeFilters, vk::MemoryPropertyFlags properties) const;

    private:
        GraphicsContext::Shared m_graphicsContext;
        vk::DeviceSize m_bufferSize;
        vk::BufferUsageFlags m_usage;
        vk::MemoryPropertyFlags m_memoryPropertyFlags;
        vk::SharingMode m_sharingMode;

        Utils::Optional<vk::raii::Buffer> m_buffer;
        Utils::Optional<vk::raii::DeviceMemory> m_bufferMemory;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_BUFFER_H