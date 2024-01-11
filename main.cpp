#include "gpio.h"
#include "i2c.h"
#include "systick.h"
#include "timer.h"
#include "clocks.h"
#include "utils.h"

#include <chrono>

extern "C"
void SystemInit()
{
}

using LED1 = GPIO::Pin<'C', 13>;          // Blue LED
//using LED2 = GPIO::Pin<'A', 0>;           // Red LED
//using I2C1 = I2C::Port<1>;
using MCO1 = GPIO::Pin<'A', 8>;
using HSE = Clocks::HSE<25.0>;
using LSE = Clocks::LSE<32.768>;
using PLL = Clocks::PLL<HSE, 25, 336, 4, 7>;

int main()
{
    LSE::enable();
    HSE::enable();
    PLL::enable();
    LED1::enable();
    //LED2::enable();
    LED1::setMode(GPIO::Mode::OUTPUT);    // Set red LED to output mode
    //LED2::setMode(GPIO::Mode::OUTPUT);    // Set blue LED to output mode

    MCO1::enable();
    MCO1::setMode(GPIO::Mode::AF);
    MCO1::setOutputType(GPIO::OutputType::PUSH_PULL);
    MCO1::setPull(GPIO::Pull::NO);
    MCO1::setSpeed(GPIO::Speed::VERY_HIGH);
    MCO1::setAF(0);

    clearBit(&RCC::Regs->CFGR, BIT(21) | BIT(22)); // 00 -> SysClock
    setBit(&RCC::Regs->CFGR, BIT(21) | BIT(22));
    clearBit(&RCC::Regs->CFGR, BIT(24) | BIT(25) | BIT(26)); // Clear prescaler
    setBit(&RCC::Regs->CFGR, BIT(24) | BIT(25) | BIT(26)); // Set prescaler to 111 -> 5

    //I2C1::enable();
    //I2C1::configure();

    SysTick::init(16000000 / 1000);         // 16 MHz, tick every 1 ms
    Timer timer(std::chrono::seconds(1));
    bool on1 = false;
    bool on2 = false;
    for (;;) {
        if (timer.expired())
        {
            on1 = !on1;
            on2 = !on2;
            LED1::set(on1);
            //LED2::set(on2);
            timer.reset();
        }
    }
    return 0;
}
