//
// Created by User on 10/13/2025.
//

#ifndef LEARNVULKANRAII_FRAMEBUFFER_H
#define LEARNVULKANRAII_FRAMEBUFFER_H

#include "base/graphicscontext.h"
#include "base/utils.h"

#include "image.h"

#include <vulkan/vulkan_raii.hpp>

namespace LearnVulkanRAII
{
    enum class SwapchainFramebufferType
    {
        None = 0,
        SWAPCHAIN,
        OFFSCREEN
    };

    struct FramebufferAttachmentInfo
    {
        vk::Format format;
        vk::ImageUsageFlags usageFlags;
        vk::ImageAspectFlags aspectFlags;
        vk::ClearValue clearValue{};

        vk::ImageView existingImageView = nullptr;
    };

    struct FramebufferSpecification
    {
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<FramebufferAttachmentInfo> attachments{};
    };

    class Framebuffer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Framebuffer)

    public:
        Framebuffer(const GraphicsContext::Shared& graphicsContext,
            const vk::raii::RenderPass& renderPass,
            const FramebufferSpecification& spec);

        void resize(uint32_t width, uint32_t height);

        [[nodiscard]] uint32_t getWidth() const;
        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] const vk::raii::Framebuffer& getBuffer() const;
        [[nodiscard]] const std::vector<vk::ClearValue>& getClearValues() const;

        const FramebufferSpecification& getFramebufferSpecification() const;

    private:
        void init();

    private:
        GraphicsContext::Shared m_graphicsContext;
        const vk::raii::RenderPass& m_renderPass;
        FramebufferSpecification m_spec;

        std::vector<Image::Shared> m_images;
        std::vector<vk::ClearValue> m_clearValues;

        Utils::Optional<vk::raii::Framebuffer> m_framebuffer;
    };

    class SwapchainFramebuffer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(SwapchainFramebuffer)

    public:
        SwapchainFramebuffer(const GraphicsContext::Shared& graphicsContext,
            const vk::raii::RenderPass& renderPass,
            const Utils::Optional<FramebufferAttachmentInfo>& depthAttachment = Utils::NullOptional);

        SwapchainFramebuffer(const GraphicsContext::Shared& graphicsContext,
            const vk::raii::RenderPass& renderPass,
            const FramebufferSpecification& spec);

        void resize(uint32_t width, uint32_t height);

        [[nodiscard]] uint32_t getWidth() const;
        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] const std::vector<Framebuffer::Shared>& getBuffers() const;

    private:
        void init();

    private:
        GraphicsContext::Shared m_graphicsContext;
        const vk::raii::RenderPass& m_renderPass;
        SwapchainFramebufferType m_framebufferType = SwapchainFramebufferType::None;
        FramebufferSpecification m_spec;

        std::vector<Framebuffer::Shared> m_framebuffers;
    };

    // class SwapchainFramebuffer;
    // class OffscreenFramebuffer;
    // class Framebuffer
    // {
    // public:
    //     DEFINE_SMART_POINTER_HELPERS(Framebuffer)
    //
    // public:
    //     virtual ~Framebuffer() = default;
    //
    //     virtual void resize(uint32_t width, uint32_t height) = 0;
    //
    //     [[nodiscard]] virtual uint32_t getWidth() const;
    //     [[nodiscard]] virtual uint32_t getHeight() const;
    //
    //     [[nodiscard]] virtual const std::vector<vk::raii::Framebuffer>& getBuffers() const;
    //
    //     static Shared create(const GraphicsContext::Shared& graphicsContext,
    //         const vk::raii::RenderPass& renderPass,
    //         bool createDepth = false);
    //
    // protected:
    //     Framebuffer(FramebufferType framebufferType,
    //         const GraphicsContext::Shared& graphicsContext,
    //         const vk::raii::RenderPass& renderPass,
    //         uint32_t width,
    //         uint32_t height,
    //         bool createDepth = false);
    //
    // protected:
    //     GraphicsContext::Shared m_graphicsContext;
    //     const vk::raii::RenderPass& m_renderPass;
    //     FramebufferType m_framebufferType = FramebufferType::None;
    //     uint32_t m_width = 0;
    //     uint32_t m_height = 0;
    //     bool m_hasDepth = false;
    //     std::vector<vk::raii::Framebuffer> m_framebuffers;
    // };
    //
    // class SwapchainFramebuffer : public Framebuffer
    // {
    // public:
    //     DEFINE_SMART_POINTER_HELPERS(SwapchainFramebuffer)
    //
    //     SwapchainFramebuffer(const GraphicsContext::Shared& graphicsContext,
    //         const vk::raii::RenderPass& renderPass,
    //         bool createDepth = false);
    //
    //     void resize(uint32_t width, uint32_t height) override;
    //
    // private:
    //     void init();
    //
    // private:
    //     std::vector<Image::Shared> m_depthImages;
    // };
    //
    // class OffscreenFramebuffer : public Framebuffer
    // {
    // public:
    //     DEFINE_SMART_POINTER_HELPERS(OffscreenFramebuffer)
    //
    //     OffscreenFramebuffer(const GraphicsContext::Shared& graphicsContext,
    //         const vk::raii::RenderPass& renderPass,
    //         uint32_t width,
    //         uint32_t height,
    //         const std::vector<FramebufferAttachmentInfo>& attachments);
    //
    //     void resize(uint32_t width, uint32_t height) override;
    //
    // private:
    //     void init();
    //     static bool isDepthFormat(vk::Format format);
    //
    // private:
    //     std::vector<FramebufferAttachmentInfo> m_attachments;
    //     std::vector<std::vector<Image::Shared>> m_images; // attachment image per swapchain image
    // };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_FRAMEBUFFER_H