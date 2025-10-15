//
// Created by User on 9/29/2025.
//

#include "window.h"

#include "event/windowevent.h"
#include "event/keyevent.h"
#include "event/mouseevent.h"

#include <glfw/glfw3.h>

namespace LearnVulkanRAII
{
    static bool s_glfwInitialized = false;

    Window::Window(const WindowSpecification& spec)
    {
        m_data.width = spec.width;
        m_data.height = spec.height;
        m_data.title = spec.title;
        m_data.fullscreen = spec.fullscreen;
        m_data.onEvent = std::bind(&Window::onEvent, this, std::placeholders::_1);

        if (!s_glfwInitialized)
        {
            s_glfwInitialized = true;
            ASSERT(glfwInit(), "Failed to initialize GLFW");
        }

        init();
    }

    uint32_t Window::getWidth() const
    {
        return m_data.width;
    }

    uint32_t Window::getHeight() const
    {
        return m_data.height;
    }

    const std::string& Window::getTitle() const
    {
        return m_data.title;
    }

    bool Window::getIsFullscreen() const
    {
        return m_data.fullscreen;
    }

    const glm::ivec2& Window::getWindowPosition() const
    {
        return m_data.windowPos;
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

    GraphicsContext::Shared Window::getGraphicsContext() const
    {
        return m_graphicsContext;
    }

    void Window::onUpdate()
    {
        glm::ivec2 currWinPos;
        glfwGetWindowPos(m_window, &currWinPos.x, &currWinPos.y);
        if (currWinPos != m_data.windowPos)
        {
            m_data.windowPos = currWinPos;

            WindowMovedEvent e;
            onEvent(e);
        }

        float time = glfwGetTime();
        Timestep timestep = time - m_lastFrameTime;
        m_lastFrameTime = time;

        m_layerStack.onUpdate(timestep);
    }

    Window::Unique Window::create(const WindowSpecification &spec)
    {
        return Utils::makeUnique<Window>(spec);
    }

    void Window::onEvent(Event &e)
    {
        m_layerStack.onEvent(e);
    }

    void Window::init()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        int width = static_cast<int>(m_data.width);
        int height = static_cast<int>(m_data.height);
        GLFWmonitor* monitor = nullptr;

        if (m_data.fullscreen)
        {
            monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            width = mode->width;
            height = mode->height;
        }

        // Create GLFW window
        m_window = glfwCreateWindow(width, height, m_data.title.c_str(), monitor, nullptr);
        ASSERT(m_window, "Failed to create GLFW window");

        glm::ivec2 windowPos;
        glfwGetWindowPos(m_window, &windowPos.x, & windowPos.y);
        m_data.windowPos = windowPos;

        glfwSetWindowUserPointer(m_window, &m_data);

        // Assign callbacks
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            windowData->width = static_cast<uint32_t>(width);
            windowData->height = static_cast<uint32_t>(height);

            WindowResizeEvent e(windowData->width, windowData->height);
            windowData->onEvent(e);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            WindowCloseEvent e;
            windowData->onEvent(e);
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (focused)
            {
                WindowFocusEvent e;
                windowData->onEvent(e);
            }
            else
            {
                WindowLostFocusEvent e;
                windowData->onEvent(e);
            }
        });

        glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (iconified)
            {
                WindowMinimizedEvent e;
                windowData->onEvent(e);
            }
            else
            {
                WindowRestoredEvent e;
                windowData->onEvent(e);
            }
        });

        glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, int maximized)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (maximized)
            {
                WindowMaximizedEvent e;
                windowData->onEvent(e);
            }
            else
            {
                WindowRestoredEvent e;
                windowData->onEvent(e);
            }
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent e(scancode, false);
                    windowData->onEvent(e);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent e(scancode);
                    windowData->onEvent(e);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent e(scancode, true);
                    windowData->onEvent(e);
                    break;
                }

                default:
                    break;;
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int key)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            KeyTypedEvent e(key);
            windowData->onEvent(e);
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseMovedEvent e(static_cast<float>(xpos), static_cast<float>(ypos));
            windowData->onEvent(e);
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent e(static_cast<float>(xOffset), static_cast<float>(yOffset));
            windowData->onEvent(e);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            auto* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent e(button);
                    windowData->onEvent(e);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent e(button);
                    windowData->onEvent(e);
                    break;
                }

                default: break;
            }
        });

        m_graphicsContext = GraphicsContext::create(this);
    }
} // LearnVulkanRAII