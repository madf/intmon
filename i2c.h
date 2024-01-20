#pragma once

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

template <size_t Num, double PFreq, uint32_t Speed>
class Port
{
    public:
        static constexpr auto num = Num;
        static constexpr auto speed = Speed;
        static constexpr auto regs = getRegs(num);

        using PinsDef = Pins<Num>;
        using SDA = PinsDef::SDA;
        using SCL = PinsDef::SCL;

        static void init()
        {
            // GPIO
            enableGPIO();
            configureGPIO();

            // I2C
            clearBit(&regs->CR1, BIT(0)); // Disable peripheral
            reset();
            setFreq();
            setTRise();
            setCCR();
            setConfig();
            setOwnAddress();
            setBit(&regs->CR1, BIT(0)); // Enable peripheral
        }

        static bool start();
        static bool stop();

        static bool writeAddress(uint8_t address, ReadWrite rw);

        static bool writeByte(uint8_t value);
        static std::pair<bool, uint8_t> readByte(AckNack ack);

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

        static void reset()
        {
            setBit(&regs->CR1, BIT(15));
            clearBit(&regs->CR1, BIT(15));
        }

        static void setFreq()
        {
            const auto val = static_cast<uint8_t>(PFreq);
            clearBit(&regs->CR2, 0x0000003F);
            setBit(&regs->CR2, val & 0x0000003F);
        }

        static void setTRise()
        {
            // Master/Sm
            const auto val = static_cast<uint8_t>(PFreq) + 1;
            clearBit(&regs->TRISE, 0x0000003F);
            setBit(&regs->TRISE, val & 0x0000003F);
        }

        static void setCCR()
        {
            // Master/Sm
            const auto val = static_cast<uint16_t>(PFreq * 1000 / (speed * 2));
            clearBit(&regs->CCR, 0x00000FFF);
            setBit(&regs->CCR, val & 0x00000FFF);

            // Set Sm, reset Duty
            clearBit(&regs->CCR, 0x0000C000);
        }

        static void setConfig()
        {
            // Disable NoStretch and GenericCall
            clearBit(&regs->CR1, 0x0000C000);
        }

        static void setOwnAddress()
        {
            clearBit(&regs->OAR1, 0x00009000); // Set 7-bit addressing mode
            clearBit(&regs->OAR1, 0x00003FFF); // ADDR8-9, ADDR and ADDR0 are zero

            clearBit(&regs->OAR2, 0x000000FF); // No Dual mode, zero OAR2
        }
};

template <typename T>
struct isPort : std::false_type {};

template <size_t Num, double PFreq, uint32_t Speed>
struct isPort<Port<Num, PFreq, Speed>> : std::true_type {};

template <typename T>
inline constexpr bool isPort_v = isPort<T>::value;

}
