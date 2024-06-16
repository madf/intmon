#include "gpio.h"
#include "mco.h"
#include "led.h"
#include "button.h"
#include "i2c.h"
#include "display.h"
#include "rtc.h"
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
using ButtonMenu = Buttons::Button<GPIO::Pin<'B', 2>>;
using ButtonPlus = Buttons::Button<GPIO::Pin<'B', 3>>;
using ButtonMinus = Buttons::Button<GPIO::Pin<'B', 4>>;
//using LED2 = GPIO::Pin<'A', 0>;           // Red LED
using MCO1 = MCO::Port<1>;
using HSE = Clocks::HSE<25.0>;
using LSE = Clocks::LSE<32.768>;
using PLL = Clocks::PLL<HSE, 25, 336, 4, 7>;
using SysClock = Clocks::SysClock<PLL, Clocks::HPRE::DIV2, Clocks::PPRE::DIV2, Clocks::PPRE::DIV1>;
using I2C1 = I2C::Port<1>;

namespace
{

enum class OutMode { TIME, VOLTAGE };

struct Fonts
{
    Fonts()
        : big(Font::font11x18()),
          tiny(Font::font6x8())
    {}
    Font big;
    Font tiny;
};

OutMode nextMode(OutMode v)
{
    if (v == OutMode::TIME)
        return OutMode::VOLTAGE;
    return OutMode::TIME;
}

std::string lz(uint8_t v)
{
    if (v < 10)
        return "0" + std::to_string(v);
    return std::to_string(v);
}

std::string formatTime(const RTC::Device::Time& time)
{
    std::string res;
    res += lz(time.hour);
    res += ":";
    res += lz(time.minute);
    return res;
}

std::string formatDate(const RTC::Device::Date& date)
{
    std::string res;
    res += std::to_string(date.year + 2000);
    res += "-";
    res += lz(date.month);
    res += "-";
    res += lz(date.day);
    return res;
}

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

void showTime(Display& display, const Fonts& fonts, const RTC::Device::DateTime& dt)
{
    display.printAt(0, 0, fonts.big, formatTime(dt.time));
    display.printAt(0, 22, fonts.tiny, formatDate(dt.date));
}

void showVoltage(Display& display, const Fonts& fonts, const INA219Data& data)
{
    display.printAt(0, 2,  fonts.tiny, data.v);
    display.printAt(0, 12, fonts.tiny, data.c);
    display.printAt(0, 22, fonts.tiny, data.charge);
    display.printAt(45, 2,  fonts.tiny, "V");
    display.printAt(45, 12, fonts.tiny, "A");
    display.printAt(45, 22, fonts.tiny, "%");
}

}

int main()
{
    LSE::enable();
    SysClock::enable();
    SysTick::init(SysClock::AHBFreq * 1000); // MHz to ms
    LED led;
    ButtonMenu buttonMenu;
    ButtonPlus buttonPlus;
    ButtonMinus buttonMinus;

    MCO1::enable(MCO1::Source::HSE, MCO::PRE::DIV5);

    auto port = I2C1(SysClock::APB1Freq, 100000);
    Display display(port, 0x3C);
    display.init();
    RTC::Device::init();
    BME280 sensor1(port, 0x76);
    sensor1.init();
    //INA219 sensor2(port, 0x40);
    //sensor2.init();

    Fonts fonts;

    Timer timer(std::chrono::seconds(1));
    OutMode mode = OutMode::TIME;
    for (;;) {
        const auto bs = buttonMenu.get();
        if (bs == ButtonMenu::State::PRESSED)
        {
            mode = nextMode(mode);
            led.set(false);
        }
        else if (bs == ButtonMenu::State::RELEASED)
        {
            led.set(true);
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

            /*INA219Data inaData;
            if (!readINA219(sensor2, inaData))
            {
                display.printAt(75, 2,  fonts.tiny, "INA219");
                display.printAt(75, 12, fonts.tiny, "failure");
                display.update();
                continue;
            }*/

            display.clear();
            //display.rect(0, 0, 128, 32, Display::Color::White);
            display.printAt(75, 2,  fonts.tiny, bmeData.temp);
            display.printAt(75, 12, fonts.tiny, std::to_string(bmeData.p));
            display.printAt(75, 22, fonts.tiny, std::to_string(bmeData.h));
            display.printAt(107, 2,  fonts.tiny, "C");
            display.printAt(100, 12, fonts.tiny, "mmhg");
            display.printAt(107, 22, fonts.tiny, "%");

            //if (mode == OutMode::TIME)
                showTime(display, fonts, RTC::Device::get());
            //else
                //showVoltage(display, fonts, inaData);

            display.vline(71, 0, 32, Display::Color::White);

            display.update();
        }
    }
    return 0;
}
