#pragma once

#include "gpio.h"
#include "timer.h"

namespace Buttons
{

class ButtonBase
{
    public:
        enum class State { PRESSED, RELEASED, NO_CHANGE };
        enum class ToggleOn { PRESS, RELEASE };

    protected:
        bool m_pressed;
        Timer m_protection;

        ButtonBase() : m_pressed(false), m_protection(std::chrono::milliseconds(10)) {}

        void setPressed(bool v)
        {
            m_pressed = v;
            m_protection.reset();
        }
};

/*
 * Tempkate parameters:
 * Pin      - GPIO pin to serve the button;
 * toggleOn - button state on which we toggle the internal state;
 * pull     - whether the GPIO pin should be pulled up or down.
 */
template <typename Pin, ButtonBase::ToggleOn toggleOn = ButtonBase::ToggleOn::PRESS, GPIO::Pull pull = GPIO::Pull::UP>
struct Button : ButtonBase
{
    static_assert(GPIO::isPin_v<Pin>, "Button must be driven by a GPIO pin");

    Button()
    {
        Pin::enable();
        Pin::setMode(GPIO::Mode::INPUT);
        Pin::setOutputType(GPIO::OutputType::PUSH_PULL);
        Pin::setPull(pull);
        Pin::setSpeed(GPIO::Speed::LOW);
    }

    State get()
    {
        // Protective 10 ms period when the change is prohibited,
        // this is needed to handle noise.
        if (!m_protection.expired())
            return State::NO_CHANGE;

        // When the pin is pulled UP, high state means the button
        // is not pressed and we need to invert the state. If the
        // pin is pulled DOWN, high state means the button is pressed,
        // no inversion needed.
        const auto state = pull == GPIO::Pull::UP && toggleOn == ToggleOn::PRESS ? !Pin::get() : Pin::get();
        if (state && !m_pressed)
        {
            setPressed(true);
            return State::PRESSED;
        }
        if (!state && m_pressed)
        {
            setPressed(false);
            return State::RELEASED;
        }
        return State::NO_CHANGE;
    }
};

}
