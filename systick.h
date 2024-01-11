#pragma once

#include <chrono>
#include <cstdint>

namespace SysTick
{

struct Type
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
};

inline Type* const Regs = reinterpret_cast<Type*>(0xe000e010);

void init(uint32_t ticks);

uint32_t getTick();

void delayMS(unsigned ms);

template <class Rep, class Period>
inline
void delay(std::chrono::duration<Rep, Period> d)
{
    delayMS(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
}

}
