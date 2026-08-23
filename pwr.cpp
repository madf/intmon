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
    DBP::set(); // Disable write protection for backup domain
    if (!DBP::waitOn(std::chrono::milliseconds(2))) {
        return false;
    }
    return true;
}

void Interface::setVoltageScalingMode(uint8_t m)
{
    VOS::write(m);
}

void Interface::enablePVD(PVDLevel level)
{
    PLS::write(std::to_underlying(level));
    PVDE::set();
}

auto Interface::getPVDStatus() -> PVDStatus
{
    if (!PVDE::isSet())
        return PVDStatus::Disabled;
    if (PVDO::isSet())
        return PVDStatus::Undervoltage;
    return PVDStatus::Normal;
}
