//
// Created by User on 10/23/2025.
//

#ifndef LEARNVULKANRAII_IMAGE_H
#define LEARNVULKANRAII_IMAGE_H

#include "base/utils.h"
#include "base/graphicscontext.h"

namespace LearnVulkanRAII
{
    struct ImageSpecification
    {
        vk::Format imageFormat;
        vk::Extent3D extent;
        vk::ImageUsageFlags usageFlags;
        vk::ImageAspectFlags aspectFlags;
        vk::ImageType imageType = vk::ImageType::e2D;
        vk::ImageViewType imageViewType = vk::ImageViewType::e2D;
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive;
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
        vk::ImageLayout finalLayout = vk::ImageLayout::eUndefined;
    };

    class Image
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Image)

    public:
        Image(const GraphicsContext::Shared& graphicsContext, const ImageSpecification& spec);

        void resize(uint32_t width, uint32_t height);

        const vk::raii::Image& getImage() const;
        const vk::raii::ImageView& getImageView() const;
        const vk::raii::DeviceMemory& getDeviceMemory() const;

    private:
        void init();

    private:
        GraphicsContext::Shared m_graphicsContext;
        ImageSpecification m_spec;

        Utils::Optional<vk::raii::Image> m_image;
        Utils::Optional<vk::raii::DeviceMemory> m_deviceMemory;
        Utils::Optional<vk::raii::ImageView> m_imageView;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_IMAGE_H