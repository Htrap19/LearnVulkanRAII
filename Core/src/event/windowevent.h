//
// Created by User on 10/5/2025.
//

#ifndef LEARNVULKANRAII_WINDOWEVENT_H
#define LEARNVULKANRAII_WINDOWEVENT_H

#include "event.h"

#include <sstream>

namespace LearnVulkanRAII
{
    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : m_width(width), m_height(height)
        {
        }

        [[nodiscard]] uint32_t getWidth() const { return m_width; }
        [[nodiscard]] uint32_t getHeight() const { return m_height; }

        [[nodiscard]] std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getWidth() << ", " << getHeight();
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)

    private:
        uint32_t m_width, m_height;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowMovedEvent : public Event
    {
    public:
        WindowMovedEvent() = default;

        EVENT_CLASS_TYPE(WindowMoved)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowLostFocusEvent : public Event
    {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowLostFocus)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowMaximizedEvent : public Event
    {
    public:
        WindowMaximizedEvent() = default;

        EVENT_CLASS_TYPE(WindowMaximized)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowMinimizedEvent : public Event
    {
    public:
        WindowMinimizedEvent() = default;

        EVENT_CLASS_TYPE(WindowMinimized)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowRestoredEvent : public Event
    {
    public:
        WindowRestoredEvent() = default;

        EVENT_CLASS_TYPE(WindowRestored)
        EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

}

#endif //LEARNVULKANRAII_WINDOWEVENT_H