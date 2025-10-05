//
// Created by User on 10/6/2025.
//

#ifndef LEARNVULKANRAII_KEYEVENT_H
#define LEARNVULKANRAII_KEYEVENT_H

#include "event.h"

#include "base/inputcodes.h"

#include <sstream>

namespace LearnVulkanRAII
{
    class KeyEvent : public Event
    {
    public:
        KeyCode getKeyCode() const { return m_keyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        explicit KeyEvent(const KeyCode keyCode)
            : m_keyCode(keyCode)
        {}

    protected:
        KeyCode m_keyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        explicit KeyPressedEvent(KeyCode keyCode, bool isRepeat = false)
            : KeyEvent(keyCode), m_isRepeat(isRepeat)
        {}

        bool getIsRepeat() const { return m_isRepeat; }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getKeyCode() << "(repeat = " << std::boolalpha << getIsRepeat() << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_isRepeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        explicit KeyReleasedEvent(KeyCode keyCode)
            : KeyEvent(keyCode)
        {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getKeyCode();
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        explicit KeyTypedEvent(KeyCode keyCode)
            : KeyEvent(keyCode)
        {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << getName() << ": " << getKeyCode();
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };

}

#endif //LEARNVULKANRAII_KEYEVENT_H