#include "rtc.h"

#include <utility>

using Device = RTC::Device;

bool Device::init(const Config& config)
{
    setClockSource(config.clockSource);
    enable();

    auto atExit = std::experimental::scope_exit(wpEnable);
    wpDisable();

    if (!enterInit())
        return false;

    setFormat(config.format);
    setOutput(config.output);
    setPolarity(config.polarity);

    if (!exitInit())
        return false;

    setOutputType(config.outputType);

    return true;
}

void Device::setClockSource(ClockSource cs)
{
    // Reset backup domain
    setBit(&RCC->BDCR, BIT(16));
    clearBit(&RCC->BDCR, BIT(16));

    clearBit(&RCC->BDCR, 0x03 << 8);
    setBit(&RCC->BDCR, std::to_underlying(cs) << 8);
}

void Device::setFormat(Format f)
{
    clearBit(&Regs->CR, BIT(6));
    if (f == Format::AmPm)
        setBit(&Regs->CR, BIT(6));
}

void Device::setOutput(Output o)
{
    clearBit(&Regs->CR, 0x03 << 21);
    setBit(&Regs->CR, std::to_underlying(o) << 21);
}

void Device::setPolarity(Polarity p)
{
    clearBit(&Regs->CR, BIT(20));
    if (p == Polarity::Low)
        setBit(&Regs->CR, BIT(20));
}

void Device::setOutputType(OutputType t)
{
    clearBit(&Regs->TAFCR, BIT(18));
    if (t == OutputType::PushPull)
        setBit(&Regs->TAFCR, BIT(18));
}

void Device::enterInit()
{
}
