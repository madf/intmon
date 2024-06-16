#pragma once

#include "timer.h"

#include <chrono>
#include <cstdint>

class Timer;

inline
consteval uint32_t BIT(uint32_t x) { return 1UL << x; }

void spin(uint32_t count);

bool waitBitOn(const volatile uint32_t* reg, uint32_t bit);
bool waitBitOn(const volatile uint32_t* reg, uint32_t bit, const Timer& timer);

template <class Rep, class Period>
bool waitBitOn(const volatile uint32_t* reg, uint32_t bit, const std::chrono::duration<Rep, Period>& timeout)
{
    return waitBitOn(reg, bit, Timer(timeout));
}

bool waitBitOff(const volatile uint32_t* reg, uint32_t bit);
bool waitBitOff(const volatile uint32_t* reg, uint32_t bit, const Timer& timer);

template <class Rep, class Period>
bool waitBitOff(const volatile uint32_t* reg, uint32_t bit, const std::chrono::duration<Rep, Period>& timeout)
{
    return waitBitOff(reg, bit, Timer(timeout));
}

inline
bool isBitSet(const volatile uint32_t* reg, uint32_t bit)
{
    return (*reg & bit) == bit;
}

inline
void setBit(volatile uint32_t* reg, uint32_t bit)
{
    *reg |= bit;
}

inline
void clearBit(volatile uint32_t* reg, uint32_t bit)
{
    *reg &= ~bit;
}

inline
uint8_t u8(auto v) { return static_cast<uint8_t>(v); }

inline
uint8_t fromBCD(uint8_t v)
{
    return u8(((v & 0xF0) >> 4) * 10 +
              (v & 0x0F));
}

inline
uint8_t toBCD(uint8_t v)
{
    return u8(((v / 10) << 4) +
              v % 10);
}
