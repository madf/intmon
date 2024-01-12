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
    DIV1 = 0,
    DIV2 = 4,
    DIV3 = 5,
    DIV4 = 6,
    DIV5 = 7
};

inline
uint8_t PREBits(PRE d)
{
    switch (d)
    {
        case PRE::DIV1: return 0x00;
        default:   return 0x04 + std::to_underlying(d);
    };
    return 0x00; // Just in case
}

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

        clearBit(&RCC::Regs->CFGR, 0x03 << 21);
        setBit(&RCC::Regs->CFGR, std::to_underlying(s) << 21);

        clearBit(&RCC::Regs->CFGR, 0x03 << 24);
        setBit(&RCC::Regs->CFGR, PREBits(div) << 24);
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

        clearBit(&RCC::Regs->CFGR, 0x03 << 30);
        setBit(&RCC::Regs->CFGR, std::to_underlying(s) << 30);

        clearBit(&RCC::Regs->CFGR, 0x03 << 27);
        setBit(&RCC::Regs->CFGR, PREBits(div) << 27);
    }
};

}
