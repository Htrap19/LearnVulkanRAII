//
// Created by User on 10/3/2025.
//

#include "applayer.h"
#include "appwindow.h"

#include "renderer/renderer.h"

#include <print>
#include <functional>

static Mesh s_quadMesh1;
static Mesh s_quadMesh2;
static Transform s_quadMesh1Transform;
static Transform s_quadMesh2Transform;

AppLayer::AppLayer(AppWindow* parent)
    : m_parent(parent)
{
    ASSERT(parent, "Parent can't be nullptr!");
    std::println(__FUNCTION__);

    m_renderer = m_parent->getRenderer();
    m_framebuffer = Framebuffer::makeShared(m_parent->getGraphicsContext(), m_renderer->getRenderPass());

    s_quadMesh1.vertices = {
        // Left Quad
        Vertex{{-0.5f,  -0.5f, 0.0f}}, // 0 - Bottom left
        Vertex{{-0.25f, -0.5f, 0.0f}}, // 1 - Bottom right
        Vertex{{-0.25f,  0.5f, 0.0f}}, // 2 - Top right
        Vertex{{-0.5f,   0.5f, 0.0f}}, // 3 - Top left
    };

    s_quadMesh1.indices = {
        0, 1, 2,   // First triangle (bottom-left to top-right)
        2, 3, 0    // Second triangle
    };

    s_quadMesh2.vertices = {
        // Right Quad
        Vertex{{0.5f, -0.5f, 0.0f}}, // 0 - Bottom left
        Vertex{{0.25f,-0.5f, 0.0f}}, // 1 - Bottom right
        Vertex{{0.25f, 0.5f, 0.0f}}, // 2 - Top right
        Vertex{{0.5f,  0.5f, 0.0f}}, // 3 - Top left
    };

    s_quadMesh2.indices = {
        0, 2, 1,   // First triangle (CW)
        0, 3, 2    // Second triangle (CW)
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
    cm.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cm.projection[1][1] *= -1; // Invert Y for Vulkan

    static float angle = 30.0f;
    s_quadMesh1Transform.rotate.x += glm::radians(angle * ts);
    s_quadMesh2Transform.rotate.x -= glm::radians(angle * ts);

    m_renderer->beginFrame(m_framebuffer, cm);
    m_renderer->drawMesh(s_quadMesh1, s_quadMesh1Transform);
    m_renderer->drawMesh(s_quadMesh2, s_quadMesh2Transform);
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
    m_framebuffer->resize(e.getWidth(), e.getHeight());
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
