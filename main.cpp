#include "gpio.h"
#include "mco.h"
#include "led.h"
#include "i2c.h"
#include "display.h"
#include "bme280.h"
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
using I2C1 = I2C::Port<1>;

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

    auto port = I2C1(SysClock::APB1Freq, 100000);
    Display display(port, 0x3C);
    display.init();
    Timer::wait(std::chrono::milliseconds(10));
    BME280 sensor(port, 0x76);
    sensor.init();

    const auto tinyFont = Font::font6x8();

    Timer timer(std::chrono::seconds(1));
    size_t i = 0;
    for (;;) {
        if (timer.expired())
        {
            uint32_t h = 0;
            uint32_t p = 0;
            int32_t t = 0;
            if (!sensor.readData(h, p, t))
            {
                display.printAt(75, 2,  tinyFont, "Sensor");
                display.printAt(75, 12, tinyFont, "failure");
                display.update();
                continue;
            }
            h /= 1024;
            p /= 25600;
            t /= 10;
            // Convert hPa to mmhg
            p *= 75;
            p /= 100;
            const auto temp = std::to_string(t / 10) + "." + std::to_string(t % 10);

            display.clear();
            display.printAt(75, 2,  tinyFont, temp);
            display.printAt(75, 12, tinyFont, std::to_string(p));
            display.printAt(75, 22, tinyFont, std::to_string(h));
            display.printAt(107, 2,  tinyFont, "C");
            display.printAt(100, 12, tinyFont, "mmhg");
            display.printAt(107, 22, tinyFont, "%");
            display.update();
            led.flip();
            //LED2::set(on2);
            timer.reset();
        }
    }
    return 0;
}
