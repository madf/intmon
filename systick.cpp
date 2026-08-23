#include "systick.h"

#include "rcc.h"
#include "utils.h"
#include "asm.h"

using Interface = SysTick::Interface;

namespace
{

volatile uint32_t s_ticks; // Volatile is important

}

void Interface::init(uint32_t ticks)
{
    if ((ticks - 1) > 0xffffff)
        return;  // Systick timer is 24 bit
    RELOAD::write(ticks - 1);
    CURRENT::write(0);
    groupWrite<ENABLE, TICKINT, CLKSOURCE>(true, true, true); // Enable systick
    RCC::SYSCFGEN::set(); // Enable SYSCFG
}

extern "C"
void SysTick_Handler(void)
{
    s_ticks = s_ticks + 1;
}

void Interface::delayMS(unsigned ms)
{
    uint32_t until = s_ticks + ms;
    while (s_ticks < until)
        ASM_NOP();
}

void Interface::delayUS(uint8_t ahbFreq, unsigned us)
{
    const auto start = CURRENT::read();
    us = us % 1000;
    while (CURRENT::read() - start < ahbFreq * us)
        ASM_NOP();
}

void Interface::delayUS(unsigned us)
{
    delayUS(static_cast<uint8_t>(RELOAD::read() / 1000), us);
}

uint32_t Interface::getTick()
{
    return s_ticks;
}
