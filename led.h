#pragma once

#include "gpio.h"

namespace LEDs
{

struct LEDBase
{
    protected:
        bool m_state;

        LEDBase() : m_state(false) {}
};

template <typename Pin>
struct LED : LEDBase
{
    static_assert(GPIO::isPin_v<Pin>, "LED must be driven by a GPIO pin");

    LED()
    {
        Pin::enable();
        Pin::setMode(GPIO::Mode::OUTPUT);
    }

    void set(bool s)
    {
        m_state = s;
        Pin::set(m_state);
    }

    void flip()
    {
        m_state = !m_state;
        Pin::set(m_state);
    }
};

}
