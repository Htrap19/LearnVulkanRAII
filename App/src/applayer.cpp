//
// Created by User on 10/3/2025.
//

#include "applayer.h"

#include <print>
#include <functional>

AppLayer::AppLayer()
{
    std::println(__FUNCTION__);
}

void AppLayer::onAttach()
{
    std::println(__FUNCTION__);
}

void AppLayer::onDetach()
{
    std::println(__FUNCTION__);
}

void AppLayer::onUpdate()
{
    Layer::onUpdate();
}

void AppLayer::onEvent(Event &e)
{
    Layer::onEvent(e);

    EventDispatcher dispatcher(e);
    // window events
    dispatcher.dispatch<WindowResizeEvent>(EVENT_FN(&AppLayer::onWindowResize));
    dispatcher.dispatch<WindowMovedEvent>(EVENT_FN(&AppLayer::onWindowMoved));
    dispatcher.dispatch<WindowFocusEvent>(EVENT_FN(&AppLayer::onWindowFocus));
    dispatcher.dispatch<WindowLostFocusEvent>(EVENT_FN(&AppLayer::onWindowLostFocus));
    dispatcher.dispatch<WindowMaximizedEvent>(EVENT_FN(&AppLayer::onWindowMaximized));
    dispatcher.dispatch<WindowMinimizedEvent>(EVENT_FN(&AppLayer::onWindowMinimized));
    dispatcher.dispatch<WindowRestoredEvent>(EVENT_FN(&AppLayer::onWindowRestored));
    dispatcher.dispatch<WindowCloseEvent>(EVENT_FN(&AppLayer::onWindowClosed));

    // key events
    dispatcher.dispatch<KeyPressedEvent>(EVENT_FN(&AppLayer::onKeyPressed));
    dispatcher.dispatch<KeyReleasedEvent>(EVENT_FN(&AppLayer::onKeyReleased));
    dispatcher.dispatch<KeyTypedEvent>(EVENT_FN(&AppLayer::onKeyTyped));

    // mouse events
    dispatcher.dispatch<MouseMovedEvent>(EVENT_FN(&AppLayer::onMouseMoved));
    dispatcher.dispatch<MouseScrolledEvent>(EVENT_FN(&AppLayer::onMouseScrolled));
    dispatcher.dispatch<MouseButtonPressedEvent>(EVENT_FN(&AppLayer::onMouseButtonPressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(EVENT_FN(&AppLayer::onMouseButtonReleased));
}

bool AppLayer::onWindowResize(WindowResizeEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowMoved(WindowMovedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowFocus(WindowFocusEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowLostFocus(WindowLostFocusEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowMaximized(WindowMaximizedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowMinimized(WindowMinimizedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowRestored(WindowRestoredEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onWindowClosed(WindowCloseEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onKeyPressed(KeyPressedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onKeyReleased(KeyReleasedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onKeyTyped(KeyTypedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onMouseButtonPressed(MouseButtonPressedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onMouseButtonReleased(MouseButtonReleasedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onMouseMoved(MouseMovedEvent &e)
{
    std::println("{}", e.toString());
    return false;
}

bool AppLayer::onMouseScrolled(MouseScrolledEvent &e)
{
    std::println("{}", e.toString());
    return false;
}
