#pragma once

#include "rcc.h"
#include "utils.h"

#include <cstdint>
#include <utility> // std::to_underlying
#include <type_traits>

namespace GPIO
{

struct Regs
{
    volatile uint32_t MODER;   // Pin mode
    volatile uint32_t OTYPER;  // Output type
    volatile uint32_t OSPEEDR; // Output speed
    volatile uint32_t PUPDR;   // Pull up/down
    volatile uint32_t IDR;     // Input data
    volatile uint32_t ODR;     // Output data
    volatile uint32_t BSRR;    // Bit set
    volatile uint32_t LCKR;    // Lock
    volatile uint32_t AFR[2];  // Alternative function
};

inline
constexpr uint16_t PINCode(char bank, uint16_t num) { return static_cast<uint16_t>(((bank - 'A') << 8U) | num); }

inline
constexpr uint8_t PINNo(uint16_t code) { return static_cast<uint8_t>(code & 0xFF); }

inline
constexpr uint8_t PINBank(uint16_t code) { return static_cast<uint8_t>(code >> 8); }

// #define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
// #define PINNO(pin) (pin & 255)
// #define PINBANK(pin) (pin >> 8)

inline
constexpr Regs* getRegs(uint8_t bank)
{
    return reinterpret_cast<Regs*>(0x40020000 + 0x400 * bank);
}

enum class Mode : uint8_t
{
    INPUT  = 0x00,
    OUTPUT = 0x01,
    AF     = 0x02,
    ANALOG = 0x03
};

enum class OutputType
{
    PUSH_PULL,
    OPEN_DRAIN
};

enum class Pull : uint8_t
{
    NO       = 0x00, // No pull
    UP       = 0x01, // Pull up
    DOWN     = 0x02, // Pull down
    RESERVED = 0x03  // Reserved
};

enum class Speed : uint8_t
{
    LOW       = 0x00,
    MEDIUM    = 0x01,
    HIGH      = 0x02,
    VERY_HIGH = 0x03
};

template <char Bank, uint16_t N>
struct Pin
{
    constexpr static uint16_t code = PINCode(Bank, N);
    constexpr static uint8_t number = PINNo(code); // Pin number within bank
    constexpr static uint8_t bank = PINBank(code); // Pin bank number

    static void enable()
    {
        if constexpr (bank < 5)
            RCC::Regs->AHB1ENR |= BIT(bank); // GPIOA - GPIOE
        else
            RCC::Regs->AHB1ENR |= BIT(7);    // Special case for GPIOH
    }

    static void setMode(Mode mode)
    {
        const auto m = std::to_underlying(mode);
        auto* regs = getRegs(bank);
        regs->MODER &= ~(3U << (number * 2));    // Clear existing setting
        regs->MODER |= (m & 3U) << (number * 2); // Set new mode
    }
    static void setOutputType(OutputType t)
    {
        auto* regs = getRegs(bank);
        regs->OTYPER &= ~(1U << number);    // Clear to default (push/pull)
        if (t == OutputType::OPEN_DRAIN)
            regs->OTYPER |= (1U << number); // Set to 1 for open-drain
    }
    static void setPull(Pull pull)
    {
        const auto p = std::to_underlying(pull);
        auto* regs = getRegs(bank);
        regs->PUPDR &= ~(3U << (number * 2));    // Clear existing setting
        regs->PUPDR |= (p & 3U) << (number * 2); // Set new mode
    }
    static void setSpeed(Speed speed)
    {
        const auto s = std::to_underlying(speed);
        auto* regs = getRegs(bank);
        regs->OSPEEDR &= ~(3U << (number * 2));    // Clear existing setting
        regs->OSPEEDR |= (s & 3U) << (number * 2); // Set new mode
    }
    static void setAF(uint8_t af)
    {
        auto* regs = getRegs(bank);
        regs->AFR[number >> 3] &= ~(15UL << ((number & 7) * 4));
        regs->AFR[number >> 3] |= static_cast<uint32_t>(af) << ((number & 7) * 4);
    }
    static void set(bool val)
    {
        auto* regs = getRegs(bank);
        regs->BSRR = (1U << number) << (val ? 0x00 : 0x10);
    }
};

template <typename T>
struct isPin : std::false_type {};

template <char Bank, uint16_t N>
struct isPin<Pin<Bank, N>> : std::true_type {};

template <typename T>
inline constexpr bool isPin_v = isPin<T>::value;

}
