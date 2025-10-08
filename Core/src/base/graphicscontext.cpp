//
// Created by User on 9/30/2025.
//

#include "graphicscontext.h"

#include "window.h"

#include <glfw/glfw3.h>

#include <set>

namespace LearnVulkanRAII
{
    GraphicsContext::GraphicsContext(Window* window)
        : m_window(window)
    {
        init();
    }

    const vk::raii::Instance& GraphicsContext::getInstance() const
    {
        return *m_instance;
    }

    const vk::raii::PhysicalDevice& GraphicsContext::getPhysicalDevice() const
    {
        return *m_physicalDevice;
    }

    const vk::raii::SurfaceKHR& GraphicsContext::getSurface() const
    {
        return *m_surface;
    }

    const vk::raii::Device & GraphicsContext::getDevice() const
    {
        return *m_device;
    }

    DeviceQueueFamilyIndices GraphicsContext::getQueueFamilyIndices() const
    {
        return m_queueFamilyIndices;
    }

    vk::Format GraphicsContext::getSwapchainImageFormat() const
    {
        return m_swapchainImageFormat;
    }

    vk::SurfaceCapabilitiesKHR GraphicsContext::getSurfaceCapabilities() const
    {
        return m_physicalDevice->getSurfaceCapabilitiesKHR(**m_surface);
    }

    GraphicsContext::Shared GraphicsContext::create(Window* window)
    {
        return Utils::makeShared<GraphicsContext>(window);
    }

    void GraphicsContext::init()
    {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapchain();
        createImageViews();
    }

    void GraphicsContext::createInstance()
    {
        const std::string& title = m_window->getTitle();
        vk::ApplicationInfo appInfo{
            title.c_str(),
            VK_MAKE_API_VERSION(0, 1, 0, 0),
            "LearnVulkanRAII Engine",
            VK_MAKE_API_VERSION(0, 1, 0, 0),
            VK_API_VERSION_1_4,
        };

        vk::InstanceCreateInfo createInfo{
            {},
            &appInfo
        };
        uint32_t glfwRequiredExtensionCount = 0;
        const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);
        createInfo.enabledExtensionCount = glfwRequiredExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwRequiredExtensions;

        m_context = vk::raii::Context();
        m_instance = vk::raii::Instance(*m_context, createInfo);
    }

    void GraphicsContext::createSurface()
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        const auto result = glfwCreateWindowSurface(**m_instance,
                                                    m_window->getNativeWindow(),
                                                    nullptr,
                                                    &surface);
        ASSERT(result == VK_SUCCESS, "Failed to create GLFW window surface");

        m_surface = vk::raii::SurfaceKHR(*m_instance, surface);
    }

    void GraphicsContext::pickPhysicalDevice()
    {
        auto devices = m_instance->enumeratePhysicalDevices();
        ASSERT(devices.size(), "No vulkan supported devices found");

        for (auto& device : devices)
        {
            if (isDeviceSuitable(device))
            {
                m_physicalDevice = std::move(device);
                break;
            }
        }

        ASSERT(m_physicalDevice.has_value(), "No suitable device found");
        LOG("Selected Device: {}", m_physicalDevice->getProperties().deviceName.data());
    }

    void GraphicsContext::createLogicalDevice()
    {
        auto queueFamilies = m_physicalDevice->getQueueFamilyProperties();

        int graphicsQueueFamilyIndex = -1;
        int presentQueueFamilyIndex = -1;

        for (size_t i = 0; i < queueFamilies.size(); ++i)
        {
            auto& qf = queueFamilies[i];
            if ((qf.queueFlags & vk::QueueFlagBits::eGraphics) && graphicsQueueFamilyIndex == -1)
            {
                graphicsQueueFamilyIndex = i;
            }

            VkBool32 supportsPresent = m_physicalDevice->getSurfaceSupportKHR(i, **m_surface);
            if (supportsPresent && presentQueueFamilyIndex == -1)
            {
                presentQueueFamilyIndex = i;
            }

            if (graphicsQueueFamilyIndex != -1 && presentQueueFamilyIndex != -1)
            {
                break;
            }
        }

        m_queueFamilyIndices = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

        std::set uniqueQueueFamilies = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };
        float priority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(uniqueQueueFamilies.size());

        for (const int qf: uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo queueCreateInfo{
                {},
                static_cast<uint32_t>(qf),
                1,
                &priority
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }

        static std::vector deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        vk::PhysicalDeviceFeatures deviceFeatures{};
        vk::DeviceCreateInfo createInfo{
            {},
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data(),
            0,
            nullptr,
            static_cast<uint32_t>(deviceExtensions.size()),
            deviceExtensions.data(),
            &deviceFeatures
        };

        m_device = m_physicalDevice->createDevice(createInfo);
        m_graphicsQueue = m_device->getQueue(graphicsQueueFamilyIndex, 0);
        m_presentQueue = m_device->getQueue(presentQueueFamilyIndex, 0);
    }

    void GraphicsContext::createSwapchain()
    {
        vk::SurfaceCapabilitiesKHR caps = getSurfaceCapabilities();

        vk::Extent2D extent = { m_window->getWidth(), m_window->getHeight() };
        if (caps.currentExtent.width != UINT32_MAX)
        {
            extent = caps.currentExtent;
        }

        uint32_t imageCount = caps.minImageCount + 1;
        if (imageCount > 0 && imageCount > caps.maxImageCount)
        {
            imageCount = caps.maxImageCount;
        }
        vk::Format imageFormat = vk::Format::eB8G8R8A8Unorm;

        vk::SwapchainCreateInfoKHR swapchainCreateInfo{
                {},
                **m_surface,
                imageCount,
                imageFormat,
                vk::ColorSpaceKHR::eSrgbNonlinear,
                extent,
                1,
                vk::ImageUsageFlagBits::eColorAttachment
            };

        auto [graphicsQueueFamilyIndex, presentQueueFamilyIndex] = m_queueFamilyIndices;
        if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
        {
            uint32_t queueFamilyIndices[] = {
                static_cast<uint32_t>(graphicsQueueFamilyIndex),
                static_cast<uint32_t>(presentQueueFamilyIndex)
            };

            swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            swapchainCreateInfo.setQueueFamilyIndices(queueFamilyIndices);
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        swapchainCreateInfo.preTransform = caps.currentTransform;
        swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
        swapchainCreateInfo.clipped = VK_TRUE;

        m_swapchain = m_device->createSwapchainKHR(swapchainCreateInfo);
        m_swapchainImages = m_swapchain->getImages();
        m_swapchainImageFormat = imageFormat;
        m_swapchainExtent = extent;
    }

    void GraphicsContext::createImageViews()
    {
        m_swapchainImageViews.clear();
        for (auto& img : m_swapchainImages)
        {
            vk::ImageViewCreateInfo viewCreateInfo{
                {},
                img,
                vk::ImageViewType::e2D,
                m_swapchainImageFormat,
                {},
                { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
            };

            m_swapchainImageViews.push_back(m_device->createImageView(viewCreateInfo));
        }
    }

    bool GraphicsContext::isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice) const
    {
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();

        bool foundGraphics = false;
        bool foundPresent = false;

        for (uint32_t i = 0; i < queueFamilies.size(); ++i)
        {
            const auto& qf = queueFamilies[i];
            if (qf.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                foundGraphics = true;
            }

            VkBool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, *m_surface.value());
            if (presentSupport)
            {
                foundPresent = true;
            }

            if (foundGraphics && foundPresent)
            {
                break;
            }
        }

        return foundGraphics && foundPresent;
    }

} // LearnVulkanRAII