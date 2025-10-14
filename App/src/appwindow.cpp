//
// Created by User on 10/14/2025.
//

#include "appwindow.h"

AppWindow::AppWindow(const WindowSpecification &spec): Window(spec)
{
    m_renderer = Renderer::makeShared(getGraphicsContext());

    auto& layerStack = getLayerStack();

    AppLayer::Shared appLayer = AppLayer::makeShared(this);
    layerStack.pushLayer(appLayer);
}

Renderer::Shared AppWindow::getRenderer()
{ return m_renderer; }

void AppWindow::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(EVENT_FN(&AppWindow::onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(EVENT_FN(&AppWindow::onWindowResize));

    Window::onEvent(e);
}

bool AppWindow::onWindowClose(WindowCloseEvent& e)
{
    auto& device = getGraphicsContext()->getDevice();
    device.waitIdle();
    return false;
}

bool AppWindow::onWindowResize(WindowResizeEvent &e)
{
    getGraphicsContext()->resize(e.getWidth(), e.getHeight());
    m_renderer->resize(e.getWidth(), e.getHeight());
    return false;
}
