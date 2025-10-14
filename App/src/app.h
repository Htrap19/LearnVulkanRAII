#ifndef LEARNVULKANRAII_APP_H
#define LEARNVULKANRAII_APP_H

#include "applayer.h"

#include "base/application.h"
#include "renderer/renderer.h"

class App final : public Application
{
public:
    DEFINE_SMART_POINTER_HELPERS(App);

public:
    App()
    {
        WindowSpecification windowSpec;
        windowSpec.width = 1280;
        windowSpec.height = 720;
        windowSpec.title = "LearnVulkanRAII";
        windowSpec.fullscreen = false;

        auto& window = createWindow(windowSpec);
        m_mainWindow = window.get(); // Not safe but will work for now
        m_renderer = Renderer::makeShared(window->getGraphicsContext());

        auto& layerStack = window->getLayerStack();

        AppLayer::Shared appLayer = AppLayer::makeShared(this);
        layerStack.pushLayer(appLayer);
    }

    Renderer::Shared getRenderer() { return m_renderer; }
    Window* getWindow() const { return m_mainWindow; }

private:
    Window* m_mainWindow = nullptr;
    Renderer::Shared m_renderer;
};

Application::Unique createApplication(int argc, char* argv[])
{
    return App::makeUnique();
}

#endif //LEARNVULKANRAII_APP_H