#pragma once

#include <cstdint>
#include <cstddef> // size_t

namespace I2C
{

    struct Regs
    {
        volatile uint32_t CR1;   // Control register 1
        volatile uint32_t CR2;   // Control register 2
        volatile uint32_t OAR1;  // Own address register 1
        volatile uint32_t OAR2;  // Own address register 2
        volatile uint32_t DR;    // Data register
        volatile uint32_t SR1;   // Status register 1
        volatile uint32_t SR2;   // Status register 2
        volatile uint32_t CCR;   // Clock control register
        volatile uint32_t TRISE; // Rise time control register
        volatile uint32_t FLTR;  // Filter register
    };

    inline
    constexpr Regs* getRegs(uint8_t num)
    {
        return reinterpret_cast<Regs*>(0x40005400 + 0x400 * num);
    }

    template <size_t Num>
    struct Pins
    {
    };

    template <>
    struct Pins<1>
    {
        using SDA = GPIO::Pin<'B', 7>;
        using SCL = GPIO::Pin<'B', 6>;
        constexpr static uint8_t SDA_AF = 4;
        constexpr static uint8_t SCL_AF = 4;
    };

    template <>
    struct Pins<2>
    {
        using SDA = GPIO::Pin<'B', 3>;
        using SCL = GPIO::Pin<'B', 10>;
        constexpr static uint8_t SDA_AF = 9;
        constexpr static uint8_t SCL_AF = 4;
    };

    template <>
    struct Pins<3>
    {
        using SDA = GPIO::Pin<'B', 4>;
        using SCL = GPIO::Pin<'A', 8>;
        constexpr static uint8_t SDA_AF = 9;
        constexpr static uint8_t SCL_AF = 4;
    };

    template <size_t Num>
    struct Port
    {
        static constexpr auto num = Num;
        using PinsDef = Pins<Num>;
        using SDA = PinsDef::SDA;
        using SCL = PinsDef::SCL;

        static void enable()
        {
            SDA::enable();
            SCL::enable();
            RCC->APB1ENR |= BIT(num + 20);
        }

        static void configure()
        {
            SDA::setMode(GPIO::Mode::AF);
            SDA::setAF(PinsDef::SDA_AF);
            SDA::setOutputType(GPIO::OutputType::OPEN_DRAIN);
            SDA::setPull(GPIO::Pull::UP);
            SDA::setSpeed(GPIO::Speed::VERY_HIGH);

            SCL::setMode(GPIO::Mode::AF);
            SCL::setAF(PinsDef::SCL_AF);
            SCL::setOutputType(GPIO::OutputType::OPEN_DRAIN);
            SCL::setPull(GPIO::Pull::UP);
            SCL::setSpeed(GPIO::Speed::VERY_HIGH);
        }

        static void reset()
        {
            auto* regs = getRegs(num);
            regs->CR1 |= (1 << 15);  // Reset
            regs->CR1 &= ~(1 << 15); // Release
        }

        static void setFreq(uint8_t freq)
        {
            auto* regs = getRegs(num);
            regs->CR2 &= ~0x0000003F;
            regs->CR2 |= freq & 0x0000003F;
        }
    };
}
