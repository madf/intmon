#include "systick.h"

#include "rcc.h"
#include "utils.h"

namespace
{

volatile uint32_t s_ticks; // volatile is important!!

}

void SysTick::init(uint32_t ticks)
{
    if ((ticks - 1) > 0xffffff) return;  // Systick timer is 24 bit
    Regs->LOAD = ticks - 1;
    Regs->VAL = 0;
    setBit(&Regs->CTRL, BIT(0) | BIT(1) | BIT(2)); // Enable systick
    setBit(&RCC::Regs->APB2ENR, BIT(14));          // Enable SYSCFG
}

extern "C"
void SysTick_Handler(void)
{
    s_ticks++;
}

void SysTick::delayMS(unsigned ms)
{
    uint32_t until = s_ticks + ms;
    while (s_ticks < until) (void) 0;
}

uint32_t SysTick::getTick()
{
    return s_ticks;
}
