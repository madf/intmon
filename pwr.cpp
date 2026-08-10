#include "pwr.h"
#include "rcc.h"
#include "utils.h"

#include <utility> // to_underlying

using Interface = PWR::Interface;

namespace
{

constexpr auto POWER_INTERFACE_CLOCK_ON = BIT(28);

}

void Interface::enable()
{
    RCC::PWREN::set();
}

void Interface::disable()
{
    RCC::PWREN::clear();
}

bool Interface::isEnabled()
{
    return RCC::PWREN::isSet();
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

void Interface::enablePVD(PVDLevel level)
{
    setBit(&Regs->CR, std::to_underlying(level) << 5);
    setBit(&Regs->CR, BIT(4));
}

auto Interface::getPVDStatus() -> PVDStatus
{
    if (!isBitSet(&Regs->CR, BIT(4)))
        return PVDStatus::Disabled;
    if (isBitSet(&Regs->CSR, BIT(2)))
        return PVDStatus::Undervoltage;
    return PVDStatus::Normal;
}
