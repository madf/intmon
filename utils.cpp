#include "utils.h"

void spin(uint32_t count)
{
    while (count--)
        asm("nop");
}

bool waitBitOn(const volatile uint32_t* reg, uint32_t bit)
{
    while (!isBitSet(reg, bit))
        asm("nop");
    return isBitSet(reg, bit);
}

bool waitBitOn(const volatile uint32_t* reg, uint32_t bit, const Timer& timer)
{
    while (!timer.expired() && !isBitSet(reg, bit))
        asm("nop");
    return isBitSet(reg, bit);
}

bool waitBitOff(const volatile uint32_t* reg, uint32_t bit)
{
    while (isBitSet(reg, bit))
        asm("nop");
    return !isBitSet(reg, bit);
}

bool waitBitOff(const volatile uint32_t* reg, uint32_t bit, const Timer& timer)
{
    while (!timer.expired() && isBitSet(reg, bit))
        asm("nop");
    return !isBitSet(reg, bit);
}
