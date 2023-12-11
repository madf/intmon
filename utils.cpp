#include "utils.h"
#include "systick.h"

void spin(uint32_t count)
{
    while (count--)
        asm("nop");
}

bool waitBIT(const volatile uint32_t* reg, uint32_t bit, uint32_t timeout)
{
    const auto start = getTick();
    while (getTick() - start < timeout && (*reg & bit) == 0)
        asm("nop");
    return (*reg & bit) != 0;
}
