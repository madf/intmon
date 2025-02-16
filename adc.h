#pragma once

#include "systick.h" // delayUS
#include "utils.h"

#include <chrono>
#include <array>
#include <utility>
#include <cstdint>

namespace ADC
{

constexpr uint32_t PRESCALER_MASK  = 0x00030000;
constexpr uint32_t RESOLUTION_MASK = 0x03000000;
constexpr uint32_t MODE_MASK       = 0x00000002;
constexpr uint32_t SCAN_MASK       = 0x00000100;
constexpr uint32_t DISC_MODE_MASK  = 0x0000E000 | 0x00000800;
constexpr uint32_t ALIGN_MASK      = 0x00000800;

constexpr auto     STAB_DELAY      = std::chrono::microseconds(3);

struct Type
{
    volatile uint32_t SR; // Status
    volatile uint32_t CR1; // Control 1
    volatile uint32_t CR2; // Control 2
    volatile uint32_t SMPR1; // Sample time 1
    volatile uint32_t SMPR2; // Sample time 2
    volatile std::array<uint32_t, 4> JOFR; // Injected channel data offset
    volatile uint32_t HTR; // Watchdog higher threshold
    volatile uint32_t LTR; // Watchdog lower threshold
    volatile uint32_t SQR1; // Regular sequence 1
    volatile uint32_t SQR2; // Regular sequence 2
    volatile uint32_t SQR3; // Regular sequence 3
    volatile uint32_t JSQR; // Injected sequence
    volatile std::array<uint32_t, 4> JDR; // Injected data
    volatile uint32_t DR; // Regular data
};

namespace Common
{

struct Type
{
    volatile uint32_t CSR; // Common status
    volatile uint32_t CCR; // Common control
    volatile uint32_t CDR; // Common data
};

inline Type* const Regs = reinterpret_cast<Type*>(0x40012300);

}

enum class PRE : uint8_t {
    DIV1 = 0,
    DIV2 = 1,
    DIV4 = 2,
    DIV8 = 4
};

enum class IntChannel : uint8_t {
    OFF,
    TSVREF,
    VBAT
};

inline
void init(PRE prescaler)
{
    // Set prescaler
    clearBit(&Common::Regs->CCR, PRESCALER_MASK);
    setBit(&Common::Regs->CCR, std::to_underlying(prescaler) << 16);

    // Enable ADC clock
    setBit(&RCC::Regs->APB2ENR, BIT(8));
    std::ignore = isBitSet(&RCC::Regs->APB2ENR, BIT(8));
}

inline
void setIntChannel(IntChannel v)
{
    constexpr auto VBatBit = BIT(22); // VBat internal channel (18)
    constexpr auto TSVRefBit = BIT(23); // Temperature sensor (18) and VRef (17) internal channels
    constexpr auto mask = TSVRefBit | VBatBit;
    clearBit(&Common::Regs->CCR, mask);
    switch (v)
    {
        case IntChannel::TSVREF: setBit(&Common::Regs->CCR, TSVRefBit); break;
        case IntChannel::VBAT: setBit(&Common::Regs->CCR, VBatBit); break;
        case IntChannel::OFF: break;
    };
}

enum class Res : uint8_t {
    RES6B  = 4,
    RES8B  = 2,
    RES10B = 1,
    RES12B = 0
};

enum class Mode : uint8_t {
    Single,
    Continuous
};

enum class Scan : uint8_t {
    Enable,
    Disable
};

struct DiscMode
{
    static constexpr DiscMode enabled(uint8_t chs) { return {chs}; }
    static constexpr DiscMode disabled() { return {0}; }
    uint8_t channels;
};

enum class Alignment : uint8_t {
    Left,
    Right
};

enum class SamplingTime : uint8_t {
    CYC3   = 0,
    CYC15  = 1,
    CYC28  = 2,
    CYC56  = 3,
    CYC84  = 4,
    CYC112 = 5,
    CYC144 = 6,
    CYC480 = 7
};

enum class Channel : uint8_t {
    CH0       = 0,
    CH1       = 1,
    CH2       = 2,
    CH3       = 3,
    CH4       = 4,
    CH5       = 5,
    CH6       = 6,
    CH7       = 7,
    CH8       = 8,
    CH9       = 9,
    CH10      = 10,
    CH11      = 11,
    CH12      = 12,
    CH13      = 13,
    CH14      = 14,
    CH15      = 15,
    CH16      = 16,
    CH17      = 17,
    CH18      = 18,
    CHTEMP    = 16,
    CHVREFINT = 17,
    CHVBAT    = 18
};

struct Config
{
    Res resolution = Res::RES12B;
    Mode mode = Mode::Single;
    Scan scan = Scan::Disable;
    DiscMode discMode = DiscMode::disabled();
    Alignment alignment = Alignment::Right;
};

struct ADC1
{
    inline static Type* const Regs = reinterpret_cast<Type*>(0x40012000);
};

template <typename T>
struct isADC : std::false_type {};

template <>
struct isADC<ADC1> : std::true_type {};

template <typename T>
inline constexpr bool isADC_v = isADC<T>::value;

class Device
{
    public:
        template <typename T>
        static Device create()
        {
            static_assert(isADC_v<T>, "Not an ADC");
            return Device(T::Regs);
        }

        void init(const Config& config)
        {
            setResolution(config.resolution);
            setMode(config.mode);
            setScan(config.scan);
            setDiscMode(config.discMode);
            setAlignment(config.alignment);
        }

        void configureChannel(Channel ch, SamplingTime st)
        {
            setSamplingTime(ch, st);
            clearBit(&m_regs->SQR3, 0x1F);
            setBit(&m_regs->SQR3, std::to_underlying(ch));
            clearBit(&m_regs->SQR1, 0x0F << 20);
        }

        bool start()
        {
            setBit(&m_regs->CR2, BIT(0));
            SysTick::delayUS(STAB_DELAY.count());
            if (!isBitSet(&m_regs->CR2, BIT(0)))
                return false;
            return true;
        }

        uint32_t read()
        {
            setBit(&m_regs->CR2, BIT(30));
            waitBitOn(&m_regs->SR, BIT(1));
            return m_regs->DR & 0x0000FFFF;
        }

        void stop()
        {
            clearBit(&m_regs->CR2, BIT(0));
        }

    private:
        Type* m_regs;

        explicit Device(Type* regs) : m_regs(regs) {}

        void setResolution(Res resolution)
        {
            clearBit(&m_regs->CR1, RESOLUTION_MASK);
            setBit(&m_regs->CR1, std::to_underlying(resolution) << 24);
        }

        void setMode(Mode mode)
        {
            clearBit(&m_regs->CR2, MODE_MASK);
            if (mode == Mode::Continuous)
                setBit(&m_regs->CR2, BIT(2));
        }

        void setScan(Scan scan)
        {
            clearBit(&m_regs->CR1, SCAN_MASK);
            if (scan == Scan::Enable)
                setBit(&m_regs->CR1, BIT(8));
        }

        void setDiscMode(DiscMode mode)
        {
            clearBit(&m_regs->CR1, DISC_MODE_MASK);
            if (mode.channels != 0)
            {
                setBit(&m_regs->CR1, mode.channels << 13);
                setBit(&m_regs->CR1, BIT(11));
            }
        }

        void setAlignment(Alignment alignment)
        {
            clearBit(&m_regs->CR2, ALIGN_MASK);
            if (alignment == Alignment::Left)
                setBit(&m_regs->CR2, BIT(11));
        }

        void setSamplingTime(Channel ch, SamplingTime st)
        {
            const auto c = std::to_underlying(ch);
            if (c < 10)
            {
                clearBit(&m_regs->SMPR2, 0x07 << (c * 3));
                setBit(&m_regs->SMPR2, std::to_underlying(st) << (c * 3));
            }
            else
            {
                clearBit(&m_regs->SMPR1, 0x07 << ((c - 10) * 3));
                setBit(&m_regs->SMPR1, std::to_underlying(st) << ((c - 10) * 3));
            }
        }
};

}
