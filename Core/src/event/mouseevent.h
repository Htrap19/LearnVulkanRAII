//
// Created by User on 10/6/2025.
//

#ifndef LEARNVULKANRAII_MOUSEEVENT_H
#define LEARNVULKANRAII_MOUSEEVENT_H

#include "event.h"

#include "base/inputcodes.h"

#include <sstream>

namespace LearnVulkanRAII
{
    class MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_x(x), m_y(y)
        {}

        float getX() const { return m_x; }
        float getY() const { return m_y; }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getX() << ", " << getY();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float m_x, m_y;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : m_xOffset(xOffset), m_yOffset(yOffset)
        {}

        float getXOffset() const { return m_xOffset; }
        float getYOffset() const { return m_yOffset; }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getXOffset() << ", " << getYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float m_xOffset, m_yOffset;
    };

    class MouseButtonEvent : public Event
    {
    public:
        MouseCode getMouseCode() const { return m_mouseCode; }

        EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput)
    protected:
        explicit MouseButtonEvent(const MouseCode mouseCode)
            : m_mouseCode(mouseCode)
        {}

    private:
        MouseCode m_mouseCode;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        explicit MouseButtonPressedEvent(const MouseCode mouseCode)
            : MouseButtonEvent(mouseCode)
        {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getMouseCode();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        explicit MouseButtonReleasedEvent(const MouseCode mouseCode)
            : MouseButtonEvent(mouseCode)
        {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getMouseCode();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}

#endif //LEARNVULKANRAII_MOUSEEVENT_H