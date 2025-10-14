//
// Created by User on 10/14/2025.
//

#ifndef LEARNVULKANRAII_APPWINDOW_H
#define LEARNVULKANRAII_APPWINDOW_H

#include "applayer.h"

#include "base/window.h"

using namespace LearnVulkanRAII;

class AppWindow final : public Window
{
public:
    DEFINE_SMART_POINTER_HELPERS(AppWindow)

public:
    explicit AppWindow(const WindowSpecification& spec);

    Renderer::Shared getRenderer();

protected:
    void onEvent(Event &e) override;

    bool onWindowClose(WindowCloseEvent& e);
    bool onWindowResize(WindowResizeEvent& e);

private:
    Renderer::Shared m_renderer;
};


#endif //LEARNVULKANRAII_APPWINDOW_H