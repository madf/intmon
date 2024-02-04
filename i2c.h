#pragma once

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

        void init()
        {
            disable();
            reset();
            setFreq();
            setTRise();
            setCCR();
            setConfig();
            setOwnAddress();
            enable();
        }

        bool start()
        {
            waitBitOff(&m_regs->SR2, BIT(1)); // Wait while BUSY
            setBit(&m_regs->CR1, BIT(8)); // START
            return waitBitOn(&m_regs->SR1, BIT(0)); // Wait START
        }

        void stop()
        {
            setBit(&m_regs->CR1, BIT(9)); // STOP
            // STOP detection is only necessary in SLAVE mode
        }

        bool writeAddress(uint8_t address, ReadWrite rw)
        {
            if (rw == ReadWrite::READ)
                m_regs->DR = (address << 1) + 1;
            else
                m_regs->DR = address << 1;
            if (!waitBitOn(&m_regs->SR1, BIT(1))) // Wait ADDR
                return false;
            // Clear ADDR by reading SR1 and SR2
            volatile uint32_t temp = 0;
            temp = m_regs->SR1;
            temp = m_regs->SR2;
            (void) temp;
            return true;
        }

        bool writeByte(uint8_t value)
        {
            if (!waitBitOn(&m_regs->SR1, BIT(7))) // Wait TxE (maybe still transferring)
                return false;
            m_regs->DR = value;
            return waitBitOn(&m_regs->SR1, BIT(7)); // Wait TxE
        }

        std::pair<bool, uint8_t> readByte(AckNack ack)
        {
            if (ack == AckNack::ACK)
                setBit(&m_regs->CR1, BIT(10));
            else
                clearBit(&m_regs->CR1, BIT(10));
            const auto success = waitBitOff(&m_regs->SR1, BIT(6)); // Wait RxNE
            return {success, m_regs->DR};
        }
    private:
        Regs* m_regs;
        size_t m_num;
        double m_PFreq;
        uint32_t m_speed;

        void disable()
        {
            clearBit(&m_regs->CR1, BIT(0)); // Disable peripheral
        }

        void enable()
        {
            setBit(&m_regs->CR1, BIT(0)); // Disable peripheral
        }

        void reset()
        {
            setBit(&m_regs->CR1, BIT(15));
            clearBit(&m_regs->CR1, BIT(15));
        }

        void setFreq()
        {
            const auto val = static_cast<uint8_t>(m_PFreq);
            clearBit(&m_regs->CR2, 0x0000003F);
            setBit(&m_regs->CR2, val & 0x0000003F);
        }

        void setTRise()
        {
            // Master/Sm
            const auto val = static_cast<uint8_t>(m_PFreq) + 1;
            clearBit(&m_regs->TRISE, 0x0000003F);
            setBit(&m_regs->TRISE, val & 0x0000003F);
        }

        void setCCR()
        {
            // Master/Sm
            const auto val = static_cast<uint16_t>(m_PFreq * 1000000 / (m_speed * 2));
            clearBit(&m_regs->CCR, 0x00000FFF);
            setBit(&m_regs->CCR, val & 0x00000FFF);

            // Set Sm, reset Duty
            clearBit(&m_regs->CCR, 0x0000C000);
        }

        void setConfig()
        {
            // Disable NoStretch and GenericCall
            clearBit(&m_regs->CR1, 0x0000C000);
        }

        void setOwnAddress()
        {
            clearBit(&m_regs->OAR1, 0x00009000); // Set 7-bit addressing mode
            clearBit(&m_regs->OAR1, 0x00003FFF); // ADDR8-9, ADDR and ADDR0 are zero

            clearBit(&m_regs->OAR2, 0x000000FF); // No Dual mode, zero OAR2
        }
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
