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

        vk::PhysicalDeviceFeatures deviceFeatures{};
        vk::DeviceCreateInfo createInfo{
            {},
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data(),
            0,
            nullptr,
            0,
            nullptr,
            &deviceFeatures
        };

        m_device = m_physicalDevice->createDevice(createInfo);
        m_graphicsQueue = m_device->getQueue(graphicsQueueFamilyIndex, 0);
        m_presentQueue = m_device->getQueue(presentQueueFamilyIndex, 0);
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