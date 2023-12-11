#include "gpio.h"
#include "i2c.h"
#include "systick.h"
#include "timer.h"
#include "utils.h"

#include <chrono>

extern "C"
void SystemInit()
{
}

using LED1 = GPIO::Pin<'C', 13>;          // Blue LED
using LED2 = GPIO::Pin<'A', 0>;           // Red LED
using I2C1 = I2C::Port<1>;

int main()
{
    LED1::enable();
    LED2::enable();
    LED1::setMode(GPIO::Mode::OUTPUT);    // Set red LED to output mode
    LED2::setMode(GPIO::Mode::OUTPUT);    // Set blue LED to output mode

    I2C1::enable();
    I2C1::configure();

    SysTickInit(16000000 / 1000);         // 16 MHz, tick every 1 ms
    Timer timer(std::chrono::seconds(1));
    bool on1 = false;
    bool on2 = false;
    for (;;) {
        if (timer.expired())
        {
            on1 = !on1;
            on2 = !on2;
            LED1::set(on1);
            LED2::set(on2);
            timer.reset();
        }
    }
    return 0;
}
