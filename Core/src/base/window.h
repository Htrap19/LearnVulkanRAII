//
// Created by User on 9/29/2025.
//

#ifndef LEARNVULKANRAII_WINDOW_H
#define LEARNVULKANRAII_WINDOW_H

#include "utils.h"
#include "graphicscontext.h"
#include "layerstack.h"
#include "event/event.h"

#include <functional>
#include <memory>
#include <string>

#include <glm/glm.hpp>

struct GLFWwindow;
namespace LearnVulkanRAII
{
    struct WindowSpecification
    {
        uint32_t width, height;
        bool fullscreen;
        std::string title;
    };

    class Window
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Window)

        using EventCallbackType = std::function<void(Event&)>;

    public:
        explicit Window(const WindowSpecification& spec);
        virtual ~Window() = default;

        [[nodiscard]] uint32_t getWidth() const;
        [[nodiscard]] uint32_t getHeight() const;
        [[nodiscard]] const std::string& getTitle() const;
        [[nodiscard]] bool getIsFullscreen() const;
        [[nodiscard]] const glm::ivec2& getWindowPosition() const;

        [[nodiscard]] GLFWwindow* getNativeWindow() const;
        [[nodiscard]] bool shouldClose() const;

        [[nodiscard]] const LayerStack& getLayerStack() const;
        [[nodiscard]] LayerStack& getLayerStack();

        static Unique create(const WindowSpecification& spec);

    protected:
        virtual void onUpdate();
        virtual void onEvent(Event& e);

    private:
        struct WindowData
        {
            uint32_t width, height;
            bool fullscreen;
            std::string title;
            glm::ivec2 windowPos;

            EventCallbackType onEvent;
        };

    private:
        void init();

    private:
        WindowData m_data;

        GLFWwindow* m_window = nullptr;
        GraphicsContext::Shared m_graphicsContext;

        LayerStack m_layerStack;

        friend class Application;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_WINDOW_H