#include "gpio.h"
#include "mco.h"
#include "led.h"
#include "i2c.h"
#include "display.h"
#include "systick.h"
#include "timer.h"
#include "clocks.h"
#include "utils.h"

#include <chrono>

extern "C"
void SystemInit()
{
}

using LED = LEDs::LED<GPIO::Pin<'C', 13>>; // Blue LED
//using LED2 = GPIO::Pin<'A', 0>;           // Red LED
using MCO1 = MCO::Port<1>;
using HSE = Clocks::HSE<25.0>;
using LSE = Clocks::LSE<32.768>;
using PLL = Clocks::PLL<HSE, 25, 336, 4, 7>;
using SysClock = Clocks::SysClock<PLL, Clocks::HPRE::DIV2, Clocks::PPRE::DIV2, Clocks::PPRE::DIV1>;
using I2C1 = I2C::Port<1, SysClock::APB1Freq, 100000>;

int main()
{
    LSE::enable();
    SysClock::enable();
    //SysTick::init(16000000 / 1000);         // 16 MHz, tick every 1 ms
    SysTick::init(SysClock::AHBFreq * 1000); // MHz to ms
    //HSE::enable();
    //PLL::enable();
    LED led;
    //LED2::enable();
    //LED2::setMode(GPIO::Mode::OUTPUT);    // Set blue LED to output mode

    MCO1::enable(MCO1::Source::HSE, MCO::PRE::DIV5);

    Display<I2C1> display(0x3C);

    Timer timer(std::chrono::seconds(1));
    size_t i = 0;
    for (;;) {
        if (timer.expired())
        {
            display.printAt(0, 0, Font::font16x26(), std::to_string(i++));
            led.flip();
            //LED2::set(on2);
            timer.reset();
        }
    }
    return 0;
}
