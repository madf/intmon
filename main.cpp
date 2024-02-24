#include "gpio.h"
#include "mco.h"
#include "led.h"
#include "i2c.h"
#include "display.h"
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
    std::string temp;
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
    data.temp = std::to_string(data.t / 10) + "." + std::to_string(data.t % 10);
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
    LED led;

    MCO1::enable(MCO1::Source::HSE, MCO::PRE::DIV5);

    auto port = I2C1(SysClock::APB1Freq, 100000);
    Display display(port, 0x3C);
    display.init();
    BME280 sensor1(port, 0x76);
    sensor1.init();
    INA219 sensor2(port, 0x40);
    sensor2.init();

    const auto tinyFont = Font::font6x8();

    Timer timer(std::chrono::seconds(1));
    size_t i = 0;
    for (;;) {
        if (timer.expired())
        {
            timer.reset();
            BME280Data bmeData;
            if (!readBME280(sensor1, bmeData))
            {
                display.printAt(75, 2,  tinyFont, "BME280");
                display.printAt(75, 12, tinyFont, "failure");
                display.update();
                continue;
            }

            INA219Data inaData;
            if (!readINA219(sensor2, inaData))
            {
                display.printAt(75, 2,  tinyFont, "BME280");
                display.printAt(75, 12, tinyFont, "failure");
                display.update();
                continue;
            }

            display.clear();
            display.printAt(75, 2,  tinyFont, bmeData.temp);
            display.printAt(75, 12, tinyFont, std::to_string(bmeData.p));
            display.printAt(75, 22, tinyFont, std::to_string(bmeData.h));
            display.printAt(107, 2,  tinyFont, "C");
            display.printAt(100, 12, tinyFont, "mmhg");
            display.printAt(107, 22, tinyFont, "%");

            display.printAt(0, 2,  tinyFont, inaData.v);
            display.printAt(0, 12, tinyFont, inaData.c);
            display.printAt(0, 22, tinyFont, inaData.charge);
            display.printAt(45, 2,  tinyFont, "V");
            display.printAt(45, 12, tinyFont, "A");
            display.printAt(45, 22, tinyFont, "%");
            display.update();
            led.flip();
        }
    }
    return 0;
}
