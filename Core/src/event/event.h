//
// Created by User on 10/5/2025.
//

#ifndef LEARNVULKANRAII_EVENT_H
#define LEARNVULKANRAII_EVENT_H

#include "../base/utils.h"

namespace LearnVulkanRAII
{
    enum class EventType
    {
        NONE = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowMaximized, WindowMinimized, WindowRestored,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory
    {
        NONE = 0,
        EventCategoryWindow         = BIT(0),
        EventCategoryInput          = BIT(1),
        EventCategoryKeyboard       = BIT(2),
        EventCategoryMouse          = BIT(3),
        EventCategoryMouseButton    = BIT(4)
    };

#define EVENT_CLASS_TYPE(type) \
    static EventType getStaticType() { return EventType::type; } \
    virtual EventType getType() const override { return getStaticType(); } \
    virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual int getCategoryFlags() const override { return category; }

    class Event
    {
    public:
        bool handled = false;

    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType getType() const = 0;
        [[nodiscard]] virtual const char* getName() const = 0;
        [[nodiscard]] virtual int getCategoryFlags() const = 0;
        [[nodiscard]] virtual std::string toString() const { return getName(); }

        [[nodiscard]] bool isInCategory(const EventCategory category) const
        {
            return getCategoryFlags() & category;
        }
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event& e)
            : m_event(e)
        {
        }

        template <typename T, typename F>
        bool dispatch(const F& func)
        {
            if (m_event.getType() == T::getStaticType())
            {
                m_event.handled = func(static_cast<T&>(m_event));
                return true;
            }

            return false;
        }

    private:
        Event& m_event;
    };

} // LearnVulkanRAII

#endif //LEARNVULKANRAII_EVENT_H