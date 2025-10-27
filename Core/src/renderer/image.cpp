//
// Created by User on 10/23/2025.
//

#include "image.h"

#include <vector>

namespace LearnVulkanRAII
{
    Image::Image(const GraphicsContext::Shared& graphicsContext, const ImageSpecification& spec)
        : m_graphicsContext(graphicsContext),
        m_spec(spec)
    {
        init();
    }

    void Image::resize(uint32_t width, uint32_t height)
    {
        m_spec.extent.width = width;
        m_spec.extent.height = height;

        init();
    }

    const vk::raii::Image& Image::getImage() const
    {
        return *m_image;
    }

    const vk::raii::ImageView& Image::getImageView() const
    {
        return *m_imageView;
    }

    const vk::raii::DeviceMemory& Image::getDeviceMemory() const
    {
        return *m_deviceMemory;
    }

    void Image::init()
    {
        auto& device = m_graphicsContext->getDevice();

        vk::ImageCreateInfo createInfo{
            {},
            m_spec.imageType,
            m_spec.imageFormat,
            m_spec.extent,
            1, 1,
            vk::SampleCountFlagBits::e1,
            m_spec.tiling,
            m_spec.usageFlags,
            m_spec.sharingMode,
            0,
            nullptr
        };

        m_image = device.createImage(createInfo);

        // Allocate memory
        vk::MemoryRequirements memRequirements = m_image->getMemoryRequirements();
        uint32_t memoryTypeIndex = m_graphicsContext->findMemoryType(memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eDeviceLocal);

        vk::MemoryAllocateInfo allocateInfo{
            memRequirements.size,
            memoryTypeIndex
        };

        m_deviceMemory = device.allocateMemory(allocateInfo);
        vk::BindImageMemoryInfo bindImageMemoryInfo{
            **m_image,
            **m_deviceMemory,
            0
        };
        device.bindImageMemory2(bindImageMemoryInfo);

        // Create image view
        vk::ImageViewCreateInfo viewCreateInfo{
            {},
            **m_image,
            m_spec.imageViewType,
            m_spec.imageFormat,
            {},
            {m_spec.aspectFlags, 0, 1, 0, 1}
        };

        m_imageView = device.createImageView(viewCreateInfo);
    }
} // LearnVulkanRAII