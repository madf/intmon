#pragma once

#include "button.h"
#include "gpio.h"

#include <optional>

class Keyboard
{
    public:
        enum class Action { Menu, Plus, Minus, Esc };
        enum class LEDAction { On, Off };

        struct Event
        {
            std::optional<Action> action;
            std::optional<LEDAction> ledAction;
        };

        Event get();
        bool menuState() const { return m_menuBtn.isPressed(); }
        bool plusState() const { return m_plusBtn.isPressed(); }
        bool minusState() const { return m_minusBtn.isPressed(); }
        bool escState() const { return m_escBtn.isPressed(); }

    private:
        using MenuBtn  = Buttons::Button<GPIO::Pin<'B', 2>>;
        using PlusBtn  = Buttons::Button<GPIO::Pin<'B', 3>>;
        using MinusBtn = Buttons::Button<GPIO::Pin<'B', 4>>;
        using EscBtn   = Buttons::Button<GPIO::Pin<'B', 5>>;

        bool m_ledState = false;

        MenuBtn  m_menuBtn;
        PlusBtn  m_plusBtn;
        MinusBtn m_minusBtn;
        EscBtn   m_escBtn;

        Event makeEvent(Action a);
};
