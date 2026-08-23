#include "timer.h"

#include "systick.h"

void Timer::reset()
{
    m_start = SysTick::Interface::getTick();
}

void Timer::expire()
{
    // m_start may wrap forward if the expression gets negative, but
    // it's okay, because the checking expression will wrap backward
    // in the `expired` call.
    m_start = SysTick::Interface::getTick() - static_cast<uint32_t>(m_duration.count()) - 1;
}

bool Timer::expired() const
{
    // The right hand side of the `>` may wrap backward if the `m_start`
    // is big.
    return SysTick::Interface::getTick() > m_start + m_duration.count();
}

void Timer::wait() const
{
    while (!expired())
        asm("nop");
}
