//
// Created by User on 10/3/2025.
//

#include "applayer.h"
#include "appwindow.h"

#include "renderer/renderer.h"

#include <print>
#include <functional>

static Mesh s_cubeMesh;
static Transform s_cubeMeshTransform;

AppLayer::AppLayer(AppWindow* parent)
    : m_parent(parent)
{
    ASSERT(parent, "Parent can't be nullptr!");
    std::println(__FUNCTION__);

    m_renderer = m_parent->getRenderer();

    s_cubeMesh.vertices = {
        // Front face
        Vertex{{-0.5f, -0.5f,  0.5f}}, // 0 - Bottom left front
        Vertex{{ 0.5f, -0.5f,  0.5f}}, // 1 - Bottom right front
        Vertex{{ 0.5f,  0.5f,  0.5f}}, // 2 - Top right front
        Vertex{{-0.5f,  0.5f,  0.5f}}, // 3 - Top left front

        // Back face
        Vertex{{-0.5f, -0.5f, -0.5f}}, // 4 - Bottom left back
        Vertex{{ 0.5f, -0.5f, -0.5f}}, // 5 - Bottom right back
        Vertex{{ 0.5f,  0.5f, -0.5f}}, // 6 - Top right back
        Vertex{{-0.5f,  0.5f, -0.5f}}, // 7 - Top left back
    };

    s_cubeMesh.indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Back face
        5, 4, 7,
        7, 6, 5,

        // Left face
        4, 0, 3,
        3, 7, 4,

        // Top face
        3, 2, 6,
        6, 7, 3,

        // Bottom face
        4, 5, 1,
        1, 0, 4
    };
}

void AppLayer::onAttach()
{
    std::println(__FUNCTION__);
}

void AppLayer::onDetach()
{
    std::println(__FUNCTION__);
}

void AppLayer::onUpdate(Timestep ts)
{
    CameraViewData cm;
    cm.projection = glm::perspective(glm::radians(45.0f), m_parent->getAspectRatio(), 0.1f, 100.0f);
    cm.view = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cm.projection[1][1] *= -1; // Invert Y for Vulkan

    static float angle = 30.0f;
    s_cubeMeshTransform.rotate.y += glm::radians(angle * ts);

    m_renderer->beginFrame(cm);
    m_renderer->drawMesh(s_cubeMesh, s_cubeMeshTransform);
    m_renderer->endFrame();
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
