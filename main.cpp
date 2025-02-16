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

using LED = LEDs::LED<GPIO::Pin<'C', 13>>; // Blue LED
//using LED2 = GPIO::Pin<'A', 0>;           // Red LED
using MCO1 = MCO::Port<1>;
using HSE = Clocks::HSE<25.0>;
using LSE = Clocks::LSE<32.768>;
using PLL = Clocks::PLL<HSE, 25, 336, 4, 7>;
using SysClock = Clocks::SysClock<PLL, Clocks::HPRE::DIV2, Clocks::PPRE::DIV2, Clocks::PPRE::DIV1>;
using I2C1 = I2C::Port<1>;

namespace
{

struct BME280Data
{
    uint32_t h;
    uint32_t p;
    int32_t t;
};

bool readBME280(BME280& sensor, BME280Data& data)
{
    if (!sensor.readData(data.h, data.p, data.t))
        return false;
    data.h /= 1024;
    data.p /= 25600;
    data.t /= 10;
    // Convert hPa to mmhg
    data.p *= 75;
    data.p /= 100;
    return true;
}

struct INA219Data
{
    std::string v;
    std::string c;
    std::string p;
    std::string charge;
};

std::string fromMilli(int16_t v)
{
    const int16_t mills = std::abs(v % 1000);
    std::string r = std::to_string(v / 1000);

    if (mills < 10) return r + ".00" + std::to_string(mills);
    if (mills < 100) return r + ".0" + std::to_string(mills);
    return r + "." + std::to_string(mills);
}

bool readINA219(INA219& sensor, INA219Data& data)
{
    uint16_t v = 0;
    uint16_t vs = 0;
    uint16_t c = 0;
    uint16_t p = 0;
    if (!sensor.readData(v, vs, c, p))
        return false;
    data.v = fromMilli(v / 2); // (v / 8) * 4 = v / 2 -> mV
    const double i = vs / 10; // vs / 100 -> mV, 0.1 Ohm shunt, vs / 100 / 0.1 = vs / 10 -> mA
    data.c = fromMilli(static_cast<int16_t>(i));
    data.p = fromMilli(static_cast<int16_t>(i * v / 2000));
    double ch = (v / 20 - 250) / 1.7;
    data.charge = std::to_string(static_cast<unsigned>(ch));
    return true;
}

}

int main()
{
    LSE::enable();
    SysClock::enable();
    SysTick::init(SysClock::AHBFreq * 1000); // MHz to ms
    //LED led;
    //Keyboard keyboard;

    MCO1::enable(MCO1::Source::HSE, MCO::PRE::DIV5);

    //auto port = I2C1(SysClock::APB1Freq, 100000);
    ADC::init(ADC::PRE::DIV1);
    ADC::setIntChannel(ADC::IntChannel::TSVREF);
    auto adc = ADC::Device::create<ADC::ADC1>();
    ADC::Config adcConfig;
    adc.init(adcConfig);
    adc.configureChannel(ADC::Channel::CHVREFINT, ADC::SamplingTime::CYC3);
    //Display display(port, 0x3C);
    //display.init();
    RTC::Device::init();
    //BME280 sensor1(port, 0x76);
    //sensor1.init();
    //INA219 sensor2(port, 0x40);
    //sensor2.init();

    //Fonts fonts;

    Screen screen(SysClock::APB1Freq);

    screen.run();

    /*Timer timer(std::chrono::seconds(1));
    for (;;) {
        const auto e = keyboard.get();
        if (e.ledAction)
            led.set(e.ledAction.value() == Keyboard::LEDAction::Off);
        if (e.action)
        {
            const auto a = e.action.value();
            using Action = Keyboard::Action;
            switch (e.action.value())
            {
                case Action::Enter: runMenu(display, fonts, keyboard); break;
                case Action::Plus:  view = nextView(view); break;
                case Action::Minus: view = prevView(view); break;
            };
        }

        if (timer.expired())
        {
            timer.reset();
            BME280Data bmeData;
            if (!readBME280(sensor1, bmeData))
            {
                display.printAt(75, 2,  fonts.tiny, "BME280");
                display.printAt(75, 12, fonts.tiny, "failure");
                display.update();
                continue;
            }

            showMain(view, {bmeData.h, bmeData.p, bmeData.t}, RTC::Device::get());
        }
    }*/
    return 0;
}
