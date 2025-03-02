#include "gpio.h"
#include "mco.h"
#include "led.h"
#include "keyboard.h"
#include "screen.h"
#include "i2c.h"
#include "display.h"
#include "rtc.h"
#include "adc.h"
#include "bme280.h"
#include "ina219.h"
#include "systick.h"
#include "timer.h"
#include "clocks.h"
#include "utils.h"

#include <chrono>

extern "C"
void SystemInit()
{
}

using MCO1 = MCO::Port<1>;
using HSE = Clocks::HSE<25.0>;
using LSE = Clocks::LSE<32.768>;
using PLL = Clocks::PLL<HSE, 25, 336, 4, 7>;
using SysClock = Clocks::SysClock<PLL, Clocks::HPRE::DIV2, Clocks::PPRE::DIV2, Clocks::PPRE::DIV1>;
using ADCInput = GPIO::Pin<'A', 0>;

int main()
{
    LSE::enable();
    SysClock::enable();
    SysTick::init(SysClock::AHBFreq * 1000); // MHz to ms

    MCO1::enable(MCO1::Source::HSE, MCO::PRE::DIV5);

    ADC::init(ADC::PRE::DIV1);
    ADC::setIntChannel(ADC::IntChannel::TSVREF);

    ADCInput::enable();
    ADCInput::setMode(GPIO::Mode::ANALOG);
    ADCInput::setPull(GPIO::Pull::NO);

    RTC::Device::init();

    Screen screen(SysClock::APB1Freq);

    screen.run();
    return 0;
}
