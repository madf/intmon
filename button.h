#pragma once

#include "gpio.h"

namespace Buttons
{

template <typename Pin>
class Button
{
    static_assert(GPIO::isPin_v<Pin>, "LED must be driven by a GPIO pin");

    public:
        enum class State { PRESSED, RELEASED, NO_CHANGE };

        Button()
            : m_state(false)
        {
            Pin::enable();
            Pin::setMode(GPIO::Mode::INPUT);
            Pin::setOutputType(GPIO::OutputType::PUSH_PULL);
            Pin::setPull(GPIO::Pull::UP);
            Pin::setSpeed(GPIO::Speed::LOW);
        }

        State get()
        {
            const auto state = Pin::get();
            if (state && !m_state)
            {
                m_state = true;
                return State::PRESSED;
            }
            if (!state && m_state)
            {
                m_state = false;
                return State::RELEASED;
            }
            return State::NO_CHANGE;
        }

    private:
        bool m_state;
};

}
