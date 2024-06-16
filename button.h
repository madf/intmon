#pragma once

#include "gpio.h"

namespace Buttons
{

template <typename Pin>
struct Button
{
    static_assert(GPIO::isPin_v<Pin>, "LED must be driven by a GPIO pin");

    static void init()
    {
        Pin::enable();
        Pin::setMode(GPIO::Mode::INPUT);
        Pin::setOutputType(GPIO::OutputType::PUSH_PULL);
        Pin::setPull(GPIO::Pull::UP);
        Pin::setSpeed(GPIO::Speed::LOW);
    }

    static bool isPressed()
    {
        return Pin::get();
    }
};

}
