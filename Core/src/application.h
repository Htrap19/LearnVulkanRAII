//
// Created by User on 9/29/2025.
//

#ifndef LEARNVULKANRAII_APPLICATION_H
#define LEARNVULKANRAII_APPLICATION_H

#include "window.h"

#include <vector>

namespace LearnVulkanRAII
{
    class Application
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Application)

    public:
        Application() = default;
        virtual ~Application() = default;

        Window::Unique& createWindow(const WindowSpecification& spec);
        void addWindow(Window::Unique&& window);

        void run();

    private:
        std::vector<Window::Unique> m_windows;
        bool m_isRunning = true;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_APPLICATION_H