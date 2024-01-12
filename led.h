#pragma once

#include "gpio.h"

namespace LEDs
{

template <typename Pin>
struct LED
{
    static_assert(GPIO::isPin_v<Pin>, "LED must be driven by a GPIO pin");

    LED()
        : state(false)
    {
        Pin::enable();
        Pin::setMode(GPIO::Mode::OUTPUT);
    }

    void set(bool s)
    {
        state = s;
        Pin::set(state);
    }

    void flip()
    {
        state = !state;
        Pin::set(state);
    }

    bool state;
};

}
