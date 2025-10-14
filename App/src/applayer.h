//
// Created by User on 10/3/2025.
//

#ifndef LEARNVULKANRAII_APPLAYER_H
#define LEARNVULKANRAII_APPLAYER_H

#include "base/layer.h"

#include "event/windowevent.h"
#include "event/keyevent.h"
#include "event/mouseevent.h"

#include "renderer/framebuffer.h"
#include "renderer/renderer.h"

using namespace LearnVulkanRAII;

class AppWindow;
class AppLayer : public Layer
{
public:
    DEFINE_SMART_POINTER_HELPERS(AppLayer);

public:
    explicit AppLayer(AppWindow* parent);

    void onAttach() override;
    void onDetach() override;

    void onUpdate() override;
    void onEvent(Event& e) override;

private:
    bool onWindowResize(WindowResizeEvent& e);
    bool onWindowMoved(WindowMovedEvent& e);
    bool onWindowFocus(WindowFocusEvent& e);
    bool onWindowLostFocus(WindowLostFocusEvent& e);
    bool onWindowMaximized(WindowMaximizedEvent& e);
    bool onWindowMinimized(WindowMinimizedEvent& e);
    bool onWindowRestored(WindowRestoredEvent& e);
    bool onWindowClosed(WindowCloseEvent& e);

    bool onKeyPressed(KeyPressedEvent& e);
    bool onKeyReleased(KeyReleasedEvent& e);
    bool onKeyTyped(KeyTypedEvent& e);

    bool onMouseButtonPressed(MouseButtonPressedEvent& e);
    bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
    bool onMouseMoved(MouseMovedEvent& e);
    bool onMouseScrolled(MouseScrolledEvent& e);

private:
    AppWindow* m_parent;

    Renderer::Shared m_renderer;
    Framebuffer::Shared m_framebuffer;
};

#endif //LEARNVULKANRAII_APPLAYER_H