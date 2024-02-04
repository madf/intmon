#pragma once

#include "gpio.h"
#include "rcc.h"

#include <utility>
#include <cstdint>
#include <cstddef> // size_t

namespace I2C
{

enum class ReadWrite { READ, WRITE };
enum class AckNack { ACK, NACK };

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
    return reinterpret_cast<Regs*>(0x40005400 + 0x400 * (num - 1));
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

class PortBase
{
    public:
        PortBase(uint8_t num, double pFreq, uint32_t speed)
            : m_regs(getRegs(num)),
              m_num(num),
              m_PFreq(pFreq),
              m_speed(speed)
        {
        }

        PortBase(const PortBase&) = delete;
        PortBase(PortBase&&) = default;

        PortBase& operator=(const PortBase&) = delete;
        PortBase& operator=(PortBase&&) = default;

        void init();

        bool start();
        void stop();

        bool writeAddress(uint8_t address, ReadWrite rw);
        bool writeByte(uint8_t value);
        std::pair<bool, uint8_t> readByte(AckNack ack);

    private:
        Regs* m_regs;
        size_t m_num;
        double m_PFreq;
        uint32_t m_speed;

        void disable();
        void enable();

        void reset();

        void setFreq();
        void setTRise();
        void setCCR();
        void setConfig();
        void setOwnAddress();
};

template <uint8_t Num>
class Port : public PortBase
{
    public:
        static constexpr auto num = Num;

        using PinsDef = Pins<Num>;
        using SDA = PinsDef::SDA;
        using SCL = PinsDef::SCL;

        Port(double pFreq, uint32_t speed)
            : PortBase(num, pFreq, speed)
        {
            // GPIO
            enableGPIO();
            configureGPIO();

            // I2C
            init();
        }

    private:
        static void enableGPIO()
        {
            SDA::enable();
            SCL::enable();
            setBit(&RCC::Regs->APB1ENR, BIT(num + 20)); // Enable clock
        }

        static void configureGPIO()
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
};

template <typename T>
struct isPort : std::false_type {};

template <uint8_t Num>
struct isPort<Port<Num>> : std::true_type {};

template <typename T>
inline constexpr bool isPort_v = isPort<T>::value;

}
