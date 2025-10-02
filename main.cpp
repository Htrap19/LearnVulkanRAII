#include <print>

#include "application.h"

using namespace LearnVulkanRAII;

class AppLayer : public Layer
{
public:
    DEFINE_SMART_POINTER_HELPERS(AppLayer);

public:
    AppLayer()
    {
        std::println(__FUNCTION__);
    }

    virtual void onAttach() override
    {
        std::println(__FUNCTION__);
    }

    virtual void onDetach() override
    {
        std::println(__FUNCTION__);
    }

    virtual void onUpdate() override
    {
        std::print(".");
    }
};

class MyApp : public Application
{
public:
    DEFINE_SMART_POINTER_HELPERS(MyApp);

public:
    MyApp()
    {
        WindowSpecification windowSpec;
        windowSpec.width = 1280;
        windowSpec.height = 720;
        windowSpec.title = "LearnVulkanRAII";
        windowSpec.fullscreen = false;

        auto& window = createWindow(windowSpec);
        m_mainWindow = window.get(); // Not safe but will work for now

        auto& layerStack = window->getLayerStack();

        AppLayer::Shared appLayer = AppLayer::makeShared();
        layerStack.pushLayer(appLayer);
    }

private:
    Window* m_mainWindow = nullptr;
};

Application::Unique createApplication(int argc, char* argv[])
{
    return MyApp::makeUnique();
}