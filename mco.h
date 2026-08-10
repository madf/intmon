#pragma once

#include "gpio.h"
#include "rcc.h"
#include "utils.h"

#include <cstdint>

namespace MCO
{

template <uint8_t N>
struct Port
{
};

enum class PRE : uint8_t {
    DIV1 = 0x00,
    DIV2 = 0x04,
    DIV3 = 0x05,
    DIV4 = 0x06,
    DIV5 = 0x07
};

template <>
struct Port<1>
{
    using Pin = GPIO::Pin<'A', 8>;

    enum class Source : uint8_t {
        HSI = 0x00,
        LSE = 0x01,
        HSE = 0x02,
        PLL = 0x03
    };

    static void enable(Source s, PRE div)
    {
        Pin::enable();

        Pin::setMode(GPIO::Mode::AF);
        Pin::setOutputType(GPIO::OutputType::PUSH_PULL);
        Pin::setPull(GPIO::Pull::NO);
        Pin::setSpeed(GPIO::Speed::VERY_HIGH);
        Pin::setAF(0);

        RCC::MCO1::write(std::to_underlying(s));
        RCC::MCO1PRE::write(std::to_underlying(div));
    }
};

template <>
struct Port<2>
{
    using Pin = GPIO::Pin<'C', 9>;

    enum class Source : uint8_t {
        SysClock = 0x00,
        PLLI2S   = 0x01,
        HSE = 0x02,
        PLL = 0x03
    };

    static void enable(Source s, PRE div)
    {
        Pin::enable();

        Pin::setMode(GPIO::Mode::AF);
        Pin::setOutputType(GPIO::OutputType::PUSH_PULL);
        Pin::setPull(GPIO::Pull::NO);
        Pin::setSpeed(GPIO::Speed::VERY_HIGH);
        Pin::setAF(0);

        RCC::MCO2::write(std::to_underlying(s));
        RCC::MCO2PRE::write(std::to_underlying(div));
    }
};

}
