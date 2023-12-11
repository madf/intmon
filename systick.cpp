#include "systick.h"

#include "rcc.h"
#include "utils.h"

namespace
{

volatile uint32_t s_ticks; // volatile is important!!

}

void SysTickInit(uint32_t ticks)
{
    if ((ticks - 1) > 0xffffff) return;  // Systick timer is 24 bit
    SysTick->LOAD = ticks - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = BIT(0) | BIT(1) | BIT(2);  // Enable systick
    RCC->APB2ENR |= BIT(14);                   // Enable SYSCFG
}

extern "C"
void SysTick_Handler(void)
{
    s_ticks++;
}

void delay(unsigned ms)
{
    uint32_t until = s_ticks + ms;
    while (s_ticks < until) (void) 0;
}

uint32_t getTick()
{
    return s_ticks;
}
