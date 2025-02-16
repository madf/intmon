#include "screen.h"

#include "menu.h"
#include "clocks.h"

namespace
{

std::string formatTemp(int32_t v)
{
    return std::to_string(v / 10) + "." + std::to_string(v % 10);
}

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

}

Screen::Screen(double pFreq)
    : m_port(pFreq, 100000),
      m_display(m_port, 0x3C),
      m_sensor(m_port, 0x76),
      m_timer(std::chrono::seconds(1))
{
    m_display.init();
    m_sensor.init();
}

void Screen::run()
{
    HPT hpt;
    DateTime dt;
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        switch (e.action.value())
        {
            case Action::Enter: runMenu(); break;
            case Action::Plus:  nextView(); show(hpt, dt); break;
            case Action::Minus: prevView(); show(hpt, dt); break;
            case Action::Exit: break;
        };

        if (m_timer.expired())
        {
            m_timer.reset();
            BME280Data bmeData;
            if (!readBME280(m_sensor, bmeData))
                showBME280Failure();
            else
            {
                hpt = {bmeData.h, bmeData.p, bmeData.t};
                dt = RTC::Device::get();
                show(hpt, dt);
            }
        }
    }
}

void Screen::runMenu()
{
    Menu menu(m_display, m_fonts, m_keyboard);
    menu.run();
}

void Screen::nextView()
{
    m_view = static_cast<View>((std::to_underlying(m_view) + 1) % 4);
}

void Screen::prevView()
{
    if (m_view == View::DateTime)
        m_view = View::Hum;
    else
        m_view = static_cast<View>(std::to_underlying(m_view) - 1);
}

void Screen::show(const HPT& hpt, const DateTime& dt)
{
    m_display.clear();
    showCommon(hpt);
    switch (m_view)
    {
        case View::DateTime: showDT(dt); break;
        case View::Temp:     showTemp(hpt.t); break;
        case View::Press:    showPress(hpt.p); break;
        case View::Hum:      showHum(hpt.h); break;
    };
    m_display.update();
}

void Screen::showDT(const DateTime& dt)
{
    m_display.printAt(0, 0, m_fonts.big, toString(dt.time));
    m_display.printAt(0, 22, m_fonts.tiny, toString(dt.date));
}

void Screen::showTemp(int32_t t)
{
    m_display.printAt(0, 0, m_fonts.big, formatTemp(t));
    m_display.printAt(50, 0, m_fonts.big, "C");
}

void Screen::showPress(uint32_t p)
{
    m_display.printAt(0, 0, m_fonts.big, std::to_string(p));
    m_display.printAt(40, 0, m_fonts.big, "mm");
}

void Screen::showHum(uint32_t h)
{
    m_display.printAt(0, 0, m_fonts.big, std::to_string(h));
    m_display.printAt(40, 0, m_fonts.big, "%");
}

void Screen::showCommon(const HPT& hpt)
{
    m_display.printAt(75, 2,  m_fonts.tiny, formatTemp(hpt.t));
    m_display.printAt(75, 12, m_fonts.tiny, std::to_string(hpt.p));
    m_display.printAt(75, 22, m_fonts.tiny, std::to_string(hpt.h));
    m_display.printAt(107, 2,  m_fonts.tiny, "C");
    m_display.printAt(100, 12, m_fonts.tiny, "mmhg");
    m_display.printAt(107, 22, m_fonts.tiny, "%");

    m_display.vline(71, 0, 32, Display::Color::White);
}

void Screen::showBME280Failure()
{
    m_display.clear();
    m_display.printAt(75, 2,  m_fonts.tiny, "BME280");
    m_display.printAt(75, 12, m_fonts.tiny, "failure");
    m_display.update();
}
