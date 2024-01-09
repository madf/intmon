#pragma once

#include <cstdint>

inline
constexpr uint32_t BIT(uint32_t x) { return 1UL << x; }

// #define BIT(x) (1UL << (x))

void spin(uint32_t count);

bool waitBIT(const volatile uint32_t* reg, uint32_t bit, uint32_t timeout);

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
