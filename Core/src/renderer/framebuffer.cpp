//
// Created by User on 10/13/2025.
//

#include "framebuffer.h"

#include <ranges>

namespace LearnVulkanRAII
{
    // Framebuffer::Framebuffer(FramebufferType framebufferType,
    //                          const GraphicsContext::Shared &graphicsContext,
    //                          const vk::raii::RenderPass &renderPass,
    //                          uint32_t width,
    //                          uint32_t height,
    //                          bool createDepth)
    //     : m_graphicsContext(graphicsContext),
    //     m_renderPass(renderPass),
    //     m_framebufferType(framebufferType),
    //     m_width(width),
    //     m_height(height),
    //     m_hasDepth(createDepth)
    // {
    // }
    //
    // uint32_t Framebuffer::getWidth() const
    // {
    //     return m_width;
    // }
    //
    // uint32_t Framebuffer::getHeight() const
    // {
    //     return m_height;
    // }
    //
    // FramebufferType Framebuffer::getType() const
    // {
    //     return m_framebufferType;
    // }
    //
    // const std::vector<vk::raii::Framebuffer> & Framebuffer::getBuffers() const
    // {
    //     return m_framebuffers;
    // }
    //
    // Framebuffer::Shared Framebuffer::create(const GraphicsContext::Shared& graphicsContext,
    //                                         const vk::raii::RenderPass& renderPass,
    //                                         bool createDepth)
    // {
    //     return SwapchainFramebuffer::makeShared(graphicsContext, renderPass, createDepth);
    // }

    Framebuffer::Framebuffer(const GraphicsContext::Shared &graphicsContext,
        const vk::raii::RenderPass &renderPass,
        const FramebufferSpecification &spec)
            : m_graphicsContext(graphicsContext),
        m_renderPass(renderPass),
        m_spec(spec)
    {
        init();
    }

    void Framebuffer::resize(uint32_t width, uint32_t height)
    {
        m_spec.width = width;
        m_spec.height = height;

        init();
    }

    uint32_t Framebuffer::getWidth() const
    {
        return m_spec.width;
    }

    uint32_t Framebuffer::getHeight() const
    {
        return m_spec.height;
    }

    const vk::raii::Framebuffer& Framebuffer::getBuffer() const
    {
        return *m_framebuffer;
    }

    const std::vector<vk::ClearValue>& Framebuffer::getClearValues() const
    {
        return m_clearValues;
    }

    const FramebufferSpecification& Framebuffer::getFramebufferSpecification() const
    {
        return m_spec;
    }

    void Framebuffer::init()
    {
        // TODO: Need to check for the attachment order from the RenderPass
        auto& device = m_graphicsContext->getDevice();

        m_images.clear();
        m_clearValues.clear();
        m_framebuffer.reset();

        m_clearValues.reserve(m_spec.attachments.size());
        std::vector<vk::ImageView> imageViews;
        imageViews.reserve(m_spec.attachments.size());
        for (const auto& attachment : m_spec.attachments)
        {
            if (attachment.existingImageView)
            {
                imageViews.push_back(attachment.existingImageView);
                m_images.push_back(nullptr);
            }
            else
            {
                ImageSpecification attachmentImageSpecification{
                    attachment.format,
                    vk::Extent3D(m_spec.width, m_spec.height, 1),
                    attachment.usageFlags,
                    attachment.aspectFlags,
                };

                auto image = Image::makeShared(m_graphicsContext, attachmentImageSpecification);
                m_images.push_back(image);
                imageViews.push_back(image->getImageView());
            }

            m_clearValues.push_back(attachment.clearValue);
        }

        vk::FramebufferCreateInfo framebufferCreateInfo{
            {},
            *m_renderPass,
            static_cast<uint32_t>(imageViews.size()),
            imageViews.data(),
            m_spec.width,
            m_spec.height,
            1
        };

        m_framebuffer = device.createFramebuffer(framebufferCreateInfo);
    }

    SwapchainFramebuffer::SwapchainFramebuffer(const GraphicsContext::Shared& graphicsContext,
        const vk::raii::RenderPass& renderPass,
        const Utils::Optional<FramebufferAttachmentInfo>& depthAttachment)
        : m_graphicsContext(graphicsContext),
        m_renderPass(renderPass),
        m_framebufferType(SwapchainFramebufferType::SWAPCHAIN)
    {
        if (depthAttachment.has_value())
        {
            m_spec.attachments.emplace_back(depthAttachment.value());
        }

        init();
    }

    SwapchainFramebuffer::SwapchainFramebuffer(const GraphicsContext::Shared &graphicsContext,
        const vk::raii::RenderPass &renderPass,
        const FramebufferSpecification &spec)
        : m_graphicsContext(graphicsContext),
        m_renderPass(renderPass),
        m_framebufferType(SwapchainFramebufferType::OFFSCREEN),
        m_spec(spec)
    {
        init();
    }

    void SwapchainFramebuffer::resize(uint32_t width, uint32_t height)
    {
        m_spec.width = width;
        m_spec.height = height;

        init();
    }

    uint32_t SwapchainFramebuffer::getWidth() const
    {
        return m_spec.width;
    }

    uint32_t SwapchainFramebuffer::getHeight() const
    {
        return m_spec.height;
    }

    const std::vector<Framebuffer::Shared>& SwapchainFramebuffer::getBuffers() const
    {
        return m_framebuffers;
    }

    void SwapchainFramebuffer::init()
    {
        m_framebuffers.clear();

        auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();
        auto swapchainImageFormat = m_graphicsContext->getSwapchainImageFormat();

        // Generate specifications
        std::vector<FramebufferSpecification> swapchainFramebufferSpecifications;
        swapchainFramebufferSpecifications.reserve(swapchainImageViews.size());
        if (m_framebufferType == SwapchainFramebufferType::SWAPCHAIN)
        {
            auto swapchainExtent = m_graphicsContext->getSwapchainExtent();
            m_spec.width = swapchainExtent.width;
            m_spec.height = swapchainExtent.height;

            for (const auto & swapchainImageView : swapchainImageViews)
            {
                FramebufferSpecification spec{
                    m_spec.width,
                    m_spec.height,
                };

                // Create a swapchain image view attachment info
                vk::ImageView ivRaw = *swapchainImageView;
                FramebufferAttachmentInfo swapchainImageAttachmentInfo;
                swapchainImageAttachmentInfo.format = swapchainImageFormat;
                swapchainImageAttachmentInfo.usageFlags = vk::ImageUsageFlagBits::eColorAttachment;
                swapchainImageAttachmentInfo.aspectFlags = vk::ImageAspectFlagBits::eColor;
                swapchainImageAttachmentInfo.clearValue = vk::ClearValue{
                    vk::ClearColorValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f })
                };
                swapchainImageAttachmentInfo.existingImageView = ivRaw;

                // TODO: Somehow need to handle the order of the attachments based on the RenderPass attachment order
                spec.attachments.emplace_back(swapchainImageAttachmentInfo);
                spec.attachments.insert(spec.attachments.end(),
                    m_spec.attachments.begin(),
                    m_spec.attachments.end());

                swapchainFramebufferSpecifications.push_back(spec);
            }
        }
        else if (m_framebufferType == SwapchainFramebufferType::OFFSCREEN)
        {
            std::ranges::fill(swapchainFramebufferSpecifications, m_spec);
        }

        // Create the framebuffers
        for (auto& swapchainFramebufferSpecification : swapchainFramebufferSpecifications)
        {
            m_framebuffers.push_back(
                Framebuffer::makeShared(m_graphicsContext, m_renderPass, swapchainFramebufferSpecification));
        }
    }

    // // -- SwapchainFramebuffer --
    // SwapchainFramebuffer::SwapchainFramebuffer(const GraphicsContext::Shared &graphicsContext,
    //     const vk::raii::RenderPass &renderPass,
    //     bool createDepth)
    //     : Framebuffer(FramebufferType::SWAPCHAIN, graphicsContext, renderPass, 0, 0, createDepth)
    // {
    //     auto swapchainExtent = m_graphicsContext->getSwapchainExtent();
    //     m_width = swapchainExtent.width;
    //     m_height = swapchainExtent.height;
    //
    //     init();
    // }
    //
    // void SwapchainFramebuffer::resize(uint32_t width, uint32_t height)
    // {
    //     m_width = width;
    //     m_height = height;
    //
    //     init();
    // }
    //
    // void SwapchainFramebuffer::init()
    // {
    //     auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();
    //     auto& device = m_graphicsContext->getDevice();
    //
    //     m_depthImages.clear();
    //     m_framebuffers.clear();
    //
    //     if (m_hasDepth)
    //     {
    //         vk::Format depthFormat = m_graphicsContext->findDepthFormat();
    //         ImageSpecification depthImageSpecification{
    //             depthFormat,
    //             vk::Extent3D{ m_width, m_height, 1 },
    //             vk::ImageUsageFlagBits::eDepthStencilAttachment,
    //             vk::ImageAspectFlagBits::eDepth
    //         };
    //
    //         m_depthImages.reserve(swapchainImageViews.size());
    //         for (size_t i = 0; i < swapchainImageViews.size(); i++)
    //         {
    //             m_depthImages.push_back(Image::makeShared(m_graphicsContext, depthImageSpecification));
    //         }
    //     }
    //
    //     for (size_t i = 0; i < swapchainImageViews.size(); i++)
    //     {
    //         vk::ImageView ivRaw = *swapchainImageViews[i];
    //
    //         uint32_t imageViewCount = 1;
    //         std::array<vk::ImageView, 2> imageViews = { ivRaw, {} };
    //         if (m_hasDepth)
    //         {
    //             imageViews[1] = (*(m_depthImages[i]->getImageView()));
    //             imageViewCount++;
    //         }
    //
    //         vk::FramebufferCreateInfo framebufferCreateInfo{
    //             {},
    //             *m_renderPass,
    //             imageViewCount,
    //             imageViews.data(),
    //             m_width,
    //             m_height,
    //             1
    //         };
    //         m_framebuffers.emplace_back(device, framebufferCreateInfo);
    //     }
    // }
    //
    // // -- OffscreenFramebuffer --
    // OffscreenFramebuffer::OffscreenFramebuffer(const GraphicsContext::Shared &graphicsContext,
    //                                            const vk::raii::RenderPass &renderPass,
    //                                            uint32_t width,
    //                                            uint32_t height,
    //                                            const std::vector<FramebufferAttachmentInfo> &attachments)
    //     : Framebuffer(FramebufferType::OFFSCREEN, graphicsContext, renderPass, width, height),
    //     m_attachments(attachments)
    // {
    //     for (auto& attachment : attachments)
    //     {
    //         m_hasDepth = isDepthFormat(attachment.format);
    //         if (m_hasDepth)
    //             break;
    //     }
    // }
    //
    // void OffscreenFramebuffer::resize(uint32_t width, uint32_t height)
    // {
    //     m_width = width;
    //     m_height = height;
    //
    //     init();
    // }
    //
    // void OffscreenFramebuffer::init()
    // {
    //     auto& swapchainImageViews = m_graphicsContext->getSwapchainImageViews();
    //     auto& device = m_graphicsContext->getDevice();
    //
    //     m_images.clear();
    //     m_framebuffers.clear();
    //
    //     m_images.resize(swapchainImageViews.size());
    //     for (size_t i = 0; i < swapchainImageViews.size(); i++)
    //     {
    //         auto& images = m_images[i];
    //
    //         std::vector<vk::ImageView> imageViews;
    //         imageViews.reserve(m_attachments.size());
    //         for (const auto& attachment : m_attachments)
    //         {
    //             ImageSpecification attachmentImageSpecification{
    //                 attachment.format,
    //                 vk::Extent3D(m_width, m_height, 1),
    //                 attachment.usageFlags,
    //                 attachment.aspectFlags,
    //             };
    //
    //             auto image = Image::makeShared(m_graphicsContext, attachmentImageSpecification);
    //             images.push_back(image);
    //             imageViews.push_back(image->getImageView());
    //         }
    //
    //         vk::FramebufferCreateInfo framebufferCreateInfo{
    //             {},
    //             *m_renderPass,
    //             static_cast<uint32_t>(imageViews.size()),
    //             imageViews.data(),
    //             m_width,
    //             m_height,
    //             1
    //         };
    //         m_framebuffers.emplace_back(device, framebufferCreateInfo);
    //     }
    // }
    //
    // bool OffscreenFramebuffer::isDepthFormat(vk::Format format)
    // {
    //     switch (format)
    //     {
    //         case vk::Format::eD16Unorm:
    //         case vk::Format::eX8D24UnormPack32:
    //         case vk::Format::eD32Sfloat:
    //         case vk::Format::eD16UnormS8Uint:
    //         case vk::Format::eD24UnormS8Uint:
    //         case vk::Format::eD32SfloatS8Uint:
    //             return true;
    //     }
    //
    //     return false;
    // }
} // LearnVulkanRAII