#include "rtc.h"

#include "rcc.h"
#include "utils.h"

#include <utility>
#include <experimental/scope>

using Device = RTC::Device;

namespace
{

constexpr uint32_t RTC_CLOCK_SELECTION_MASK = 0x00000300;

constexpr auto INIT_MODE       = BIT(7);
constexpr auto INIT_MODE_IS_ON = BIT(6);
constexpr auto REGISTER_SYNC   = BIT(5);

constexpr uint32_t YEAR_MASK  = 0x00FF0000;
constexpr uint32_t MONTH_MASK = 0x00001F00;
constexpr uint32_t DAY_MASK   = 0x0000003F;
constexpr uint32_t DATE_MASK  = YEAR_MASK | MONTH_MASK | DAY_MASK;

constexpr uint32_t HOUR_MASK   = 0x003F0000;
constexpr uint32_t MINUTE_MASK = 0x00007F00;
constexpr uint32_t SECOND_MASK = 0x0000007F;
constexpr uint32_t TIME_MASK   = HOUR_MASK | MINUTE_MASK | SECOND_MASK;

}

bool Device::init(const Config& config)
{
    setClockSource(config.clockSource);
    enable();

    auto wpEnabler = std::experimental::scope_exit(wpEnable);
    wpDisable();

    if (!enterInit())
        return false;

    setFormat(config.format);
    setOutput(config.output);
    setPolarity(config.polarity);

    if (!exitInit())
        return false;

    setOutputType(config.outputType);

    return resync();
}

bool Device::resync()
{
    clearBit(&Regs->ISR, REGISTER_SYNC);
    return waitBitOn(&Regs->ISR, REGISTER_SYNC);
}

void Device::enable()
{
    setBit(&RCC::Regs->BDCR, BIT(15));
}

void Device::setClockSource(ClockSource cs)
{
    // Store old register state
    // After resset register goes to 0x00000000
    const auto old = RCC::Regs->BDCR & ~RTC_CLOCK_SELECTION_MASK;
    // Reset backup domain
    setBit(&RCC::Regs->BDCR, BIT(16));
    clearBit(&RCC::Regs->BDCR, BIT(16));
    // Restore register state
    RCC::Regs->BDCR = old;
    // If LSE was enabled, wait its activation
    if (isBitSet(&RCC::Regs->BDCR, BIT(0))) // If LSE was enabled
        waitBitOn(&RCC::Regs->BDCR, BIT(1)); // Wait LSE

    clearBit(&RCC::Regs->BDCR, RTC_CLOCK_SELECTION_MASK);
    setBit(&RCC::Regs->BDCR, std::to_underlying(cs) << 8);
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

bool Device::enterInit()
{
    if (isBitSet(&Regs->ISR, INIT_MODE_IS_ON))
        return true;
    setBit(&Regs->ISR, INIT_MODE);
    return waitBitOn(&Regs->ISR, INIT_MODE_IS_ON);
}

bool Device::exitInit()
{
    if (!isBitSet(&Regs->ISR, INIT_MODE_IS_ON))
        return true;
    clearBit(&Regs->ISR, INIT_MODE);
    return waitBitOff(&Regs->ISR, INIT_MODE_IS_ON);
}

void Device::wpEnable()
{
    Regs->WPR = 0xFF;
}

void Device::wpDisable()
{
    Regs->WPR = 0xCA;
    Regs->WPR = 0x53;
}

auto Device::getDate() -> Date
{
    const auto dr = Regs->DR;
    return {fromBCD(u8((dr & YEAR_MASK) >> 16)),
            fromBCD(u8((dr & MONTH_MASK) >> 8)),
            fromBCD(u8(dr & DAY_MASK))};
}

auto Device::getTime() -> Time
{
    const auto tr = Regs->TR;
    return {fromBCD(u8((tr & HOUR_MASK) >> 16)),
            fromBCD(u8((tr & MINUTE_MASK) >> 8)),
            fromBCD(u8(tr & SECOND_MASK))};
}

bool Device::set(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss)
{
    auto atExit = std::experimental::scope_exit(wpEnable);
    wpDisable();

    if (!enterInit())
        return false;

    setDateImpl(y, m, d);
    setTimeImpl(hh, mm, ss);

    if (!exitInit())
        return false;

    return resync();
}

bool Device::setDate(uint16_t y, uint8_t m, uint8_t d)
{
    auto atExit = std::experimental::scope_exit(wpEnable);
    wpDisable();

    if (!enterInit())
        return false;

    setDateImpl(y, m, d);

    if (!exitInit())
        return false;

    return resync();
}

bool Device::setTime(uint8_t h, uint8_t m, uint8_t s)
{
    auto atExit = std::experimental::scope_exit(wpEnable);
    wpDisable();

    if (!enterInit())
        return false;

    setTimeImpl(h, m, s);

    if (!exitInit())
        return false;

    return resync();
}

void Device::setDateImpl(uint16_t y, uint8_t m, uint8_t d)
{
    const auto dr = (toBCD(static_cast<uint8_t>(y - 2000)) << 16) |
                    (toBCD(m) << 8)                               |
                    toBCD(d);
    clearBit(&Regs->DR, DATE_MASK);
    setBit(&Regs->DR, dr);
}

void Device::setTimeImpl(uint8_t h, uint8_t m, uint8_t s)
{
    const auto tr = (toBCD(h) << 16) |
                    (toBCD(m) << 8)  |
                    toBCD(s);
    clearBit(&Regs->TR, TIME_MASK);
    setBit(&Regs->TR, tr);
}
