#include "timer.h"

#include "systick.h"

void Timer::reset()
{
    m_start = SysTick::getTick();
}

void Timer::expire()
{
    m_start = SysTick::getTick() - m_duration.count() - 1;
}

bool Timer::expired() const
{
    return SysTick::getTick() > m_start + m_duration.count();
}

void Timer::wait() const
{
    while (!expired())
        asm("nop");
}
