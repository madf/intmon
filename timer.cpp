#include "timer.h"

#include "systick.h"

void Timer::reset()
{
    m_start = getTick();
}

bool Timer::expired()
{
    return getTick() > m_start + m_duration.count();
}
