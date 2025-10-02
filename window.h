//
// Created by User on 9/29/2025.
//

#ifndef LEARNVULKANRAII_WINDOW_H
#define LEARNVULKANRAII_WINDOW_H

#include <memory>
#include <string>

#include "utils.h"
#include "graphicscontext.h"
#include "layerstack.h"

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

    public:
        explicit Window(const WindowSpecification& spec);

        [[nodiscard]] const WindowSpecification& getSpecification() const;
        [[nodiscard]] GLFWwindow* getNativeWindow() const;
        [[nodiscard]] bool shouldClose() const;

        const LayerStack& getLayerStack() const;
        LayerStack& getLayerStack();

        void onUpdate();

        static Unique create(const WindowSpecification& spec);

    private:
        void init();

    private:
        WindowSpecification m_spec;

        GLFWwindow* m_window = nullptr;
        GraphicsContext::Shared m_graphicsContext;

        LayerStack m_layerStack;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_WINDOW_H