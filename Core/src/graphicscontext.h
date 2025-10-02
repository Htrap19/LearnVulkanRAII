//
// Created by User on 9/30/2025.
//

#ifndef LEARNVULKANRAII_GRAPHICSCONTEXT_H
#define LEARNVULKANRAII_GRAPHICSCONTEXT_H

#include "utils.h"

#include <vulkan/vulkan_raii.hpp>

namespace LearnVulkanRAII
{
    struct DeviceQueueFamilyIndices
    {
        int graphicsQueueFamilyIndex = -1;
        int presentQueueFamilyIndex = -1;
    };

    class Window;
    class GraphicsContext
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(GraphicsContext)

    public:
        explicit GraphicsContext(Window* window);

        [[nodiscard]] const vk::raii::Instance& getInstance() const;

        static Shared create(Window* window);

    private:
        void init();

        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();

        bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice) const;

    private:
        Window* m_window = nullptr;
        Utils::Optional<vk::raii::Context> m_context;
        Utils::Optional<vk::raii::Instance> m_instance;

        Utils::Optional<vk::raii::SurfaceKHR> m_surface;
        Utils::Optional<vk::raii::PhysicalDevice> m_physicalDevice;
        Utils::Optional<vk::raii::Device> m_device;
        Utils::Optional<vk::raii::Queue> m_graphicsQueue;
        Utils::Optional<vk::raii::Queue> m_presentQueue;

        DeviceQueueFamilyIndices m_queueFamilyIndices;

        friend class Window;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_GRAPHICSCONTEXT_H