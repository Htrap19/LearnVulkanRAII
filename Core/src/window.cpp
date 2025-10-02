//
// Created by User on 9/29/2025.
//

#include "window.h"

#include <glfw/glfw3.h>

namespace LearnVulkanRAII
{
    static bool s_glfwInitialized = false;

    Window::Window(const WindowSpecification& spec)
        : m_spec(spec)
    {
        if (!s_glfwInitialized)
        {
            s_glfwInitialized = true;
            ASSERT(glfwInit(), "Failed to initialize GLFW");
        }

        init();
    }

    const WindowSpecification& Window::getSpecification() const
    {
        return m_spec;
    }

    GLFWwindow* Window::getNativeWindow() const
    {
        return m_window;
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    const LayerStack& Window::getLayerStack() const
    {
        return m_layerStack;
    }

    LayerStack& Window::getLayerStack()
    {
        return m_layerStack;
    }

    void Window::onUpdate()
    {
        m_layerStack.onUpdate();
    }

    Window::Unique Window::create(const WindowSpecification &spec)
    {
        return Utils::makeUnique<Window>(spec);
    }

    void Window::init()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        int width = static_cast<int>(m_spec.width);
        int height = static_cast<int>(m_spec.height);
        GLFWmonitor* monitor = nullptr;

        if (m_spec.fullscreen)
        {
            monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            width = mode->width;
            height = mode->height;
        }

        // Create GLFW window
        m_window = glfwCreateWindow(width, height, m_spec.title.c_str(), monitor, nullptr);
        ASSERT(m_window, "Failed to create GLFW window");

        m_graphicsContext = GraphicsContext::create(this);
    }
} // LearnVulkanRAII