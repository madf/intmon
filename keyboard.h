#pragma once

#include "button.h"
#include "gpio.h"

#include <optional>

class Keyboard
{
    public:
        enum class Action { Enter, Plus, Minus, Exit };
        enum class LEDAction { On, Off };

        struct Event
        {
            std::optional<Action> action;
            std::optional<LEDAction> ledAction;
        };

        Event get();
        /*
        bool enterState() const { return m_enterBtn.isPressed(); }
        bool plusState() const { return m_plusBtn.isPressed(); }
        bool minusState() const { return m_minusBtn.isPressed(); }
        bool exitState() const { return m_exitBtn.isPressed(); }
        */

    private:
        using EnterBtn  = Buttons::Button<GPIO::Pin<'B', 2>>;
        using PlusBtn  = Buttons::Button<GPIO::Pin<'B', 3>>;
        using MinusBtn = Buttons::Button<GPIO::Pin<'B', 4>>;
        using ExitBtn   = Buttons::Button<GPIO::Pin<'B', 5>>;

        bool m_ledState = false;

        EnterBtn m_enterBtn;
        PlusBtn  m_plusBtn;
        MinusBtn m_minusBtn;
        ExitBtn  m_exitBtn;

        Event makeEvent(Action a);
};
