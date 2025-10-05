//
// Created by User on 9/29/2025.
//

#include "application.h"

#include <glfw/glfw3.h>

namespace LearnVulkanRAII
{
    Window::Unique& Application::createWindow(const WindowSpecification &spec)
    {
        return m_windows.emplace_back(Utils::makeUnique<Window>(spec));
    }

    void Application::addWindow(Window::Unique &&window)
    {
        m_windows.emplace_back(std::move(window));
    }

    void Application::run()
    {
        while (m_isRunning)
        {
            glfwPollEvents();
            bool isAllWindowClosed = true;

            for (const auto& window : m_windows)
            {
                bool shouldClose = window->shouldClose();
                if (!shouldClose)
                {
                    isAllWindowClosed = false;
                }

                if (shouldClose)
                    continue;

                window->onUpdate();
            }

            if (isAllWindowClosed)
            {
                m_isRunning = false;
            }
        }
    }
} // LearnVulkanRAII

extern LearnVulkanRAII::Application::Unique createApplication(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    auto app = createApplication(argc, argv);
    app->run();

    return 0;
}