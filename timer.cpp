#include "timer.h"

#include "systick.h"

void Timer::reset()
{
    m_start = SysTick::getTick();
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
