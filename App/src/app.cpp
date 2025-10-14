//
// Created by User on 10/14/2025.
//

#include "app.h"

#include "appwindow.h"

App::App()
{
    WindowSpecification windowSpec;
    windowSpec.width = 1280;
    windowSpec.height = 720;
    windowSpec.title = "LearnVulkanRAII";
    windowSpec.fullscreen = false;

    addWindow(AppWindow::makeUnique(windowSpec));
}

Application::Unique createApplication(int argc, char* argv[])
{
    return App::makeUnique();
}
