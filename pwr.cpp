#include "pwr.h"
#include "rcc.h"
#include "utils.h"

using Interface = PWR::Interface;

namespace
{

constexpr auto POWER_INTERFACE_CLOCK_ON = BIT(28);

}

void Interface::enable()
{
    setBit(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON);
}

void Interface::disable()
{
    clearBit(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON);
}

bool Interface::isEnabled()
{
    return isBitSet(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON);
}

bool Interface::disableBackupDomainWriteProtection()
{
    setBit(&Regs->CR, BIT(8)); // Disable write protection for backup domain
    if (!waitBitOn(&Regs->CR, BIT(8), std::chrono::milliseconds(2)))
        return false;
    return true;
}

void Interface::setVoltageScalingMode(uint8_t m)
{
    setBit(&Regs->CR, (m & 0x03) << 14);
}
