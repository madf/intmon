#include "screen.h"

#include "menu.h"
#include "clocks.h"

namespace
{

std::string formatTemp(int32_t v)
{
    return std::to_string(v / 10) + "." + std::to_string(v % 10);
}

}

Screen::Screen(uint8_t pFreq)
    : m_port(pFreq, 100000),
      m_display(m_port, 0x3C),
      m_sensor(m_port, 0x76),
      m_adc(ADC::Device::create<ADC::ADC1>()),
      m_cal(ADC::readCalibration()),
      m_timer(std::chrono::seconds(1))
{
    m_display.init();
    m_sensor.init();
    m_adc.init({});
}

void Screen::run()
{
    m_dt = RTC::Device::get();
    if (m_dt.year() < 2000)
    {
        RTC::Device::setDate({2000, m_dt.month(), m_dt.day()});
        m_dt = RTC::Device::get();
    }
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        if (e.action)
        {
            switch (e.action.value())
            {
                case Action::Enter: runMenu(); m_timer.expire(); break;
                case Action::Plus:  nextView(); show(); break;
                case Action::Minus: prevView(); show(); break;
                case Action::Exit: break;
            };
        }

        if (m_timer.expired())
        {
            m_timer.reset();
            if (!readBME280())
            {
                showBME280Failure();
                continue;
            }

            if (!readADC())
            {
                showADCFailure();
                continue;
            }

            m_dt = RTC::Device::get();
            show();
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
    m_view = static_cast<View>((std::to_underlying(m_view) + 1) % 5);
}

void Screen::prevView()
{
    if (m_view == View::DateTime)
        m_view = View::ADC;
    else
        m_view = static_cast<View>(std::to_underlying(m_view) - 1);
}

void Screen::show()
{
    m_display.clear();
    showCommon();
    switch (m_view)
    {
        case View::DateTime: showDT(); break;
        case View::Temp:     showTemp(); break;
        case View::Press:    showPress(); break;
        case View::Hum:      showHum(); break;
        case View::ADC:      showADC(); break;
    };
    m_display.update();
}

void Screen::showDT()
{
    m_display.printAt(0, 0, m_fonts.big, toString(m_dt.time));
    m_display.printAt(0, 22, m_fonts.tiny, toString(m_dt.date));
}

void Screen::showTemp()
{
    m_display.printAt(0, 0, m_fonts.big, formatTemp(m_hpt.t));
    m_display.printAt(50, 0, m_fonts.big, "C");
}

void Screen::showPress()
{
    m_display.printAt(0, 0, m_fonts.big, std::to_string(m_hpt.p));
    m_display.printAt(40, 0, m_fonts.big, "mm");
}

void Screen::showHum()
{
    m_display.printAt(0, 0, m_fonts.big, std::to_string(m_hpt.h));
    m_display.printAt(40, 0, m_fonts.big, "%");
}

void Screen::showADC()
{
    const auto vref = std::to_string(m_adcData.vref / 100) + "." + lz(m_adcData.vref % 100);
    m_display.printAt(0, 2, m_fonts.tiny, vref);
    m_display.printAt(40, 2, m_fonts.tiny, "V");
    const auto temp = std::to_string(m_adcData.t / 10) + "." + std::to_string(m_adcData.t % 10);
    m_display.printAt(0, 12, m_fonts.tiny, temp);
    m_display.printAt(40, 12, m_fonts.tiny, "C");
    const auto volt = std::to_string(m_adcData.v / 100) + "." + lz(m_adcData.v % 100);
    m_display.printAt(0, 22, m_fonts.tiny, volt);
    m_display.printAt(40, 22, m_fonts.tiny, "V");
}

void Screen::showCommon()
{
    m_display.printAt(75, 2,  m_fonts.tiny, formatTemp(m_hpt.t));
    m_display.printAt(75, 12, m_fonts.tiny, std::to_string(m_hpt.p));
    m_display.printAt(75, 22, m_fonts.tiny, std::to_string(m_hpt.h));
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

void Screen::showADCFailure()
{
    m_display.clear();
    m_display.printAt(75, 2,  m_fonts.tiny, "ADC");
    m_display.printAt(75, 12, m_fonts.tiny, "failure");
    m_display.update();
}

bool Screen::readBME280()
{
    HPT data;
    if (!m_sensor.readData(data.h, data.p, data.t))
        return false;
    data.h /= 1024;
    data.p /= 25600;
    data.t /= 10;
    // Convert hPa to mmhg
    data.p *= 75;
    data.p /= 100;
    m_hpt = data;
    return true;
}

bool Screen::readADC()
{
    m_adc.configureChannel(ADC::Channel::CHVREFINT, ADC::SamplingTime::CYC480);
    if (!m_adc.start())
        return false;
    const uint32_t vRefInt = m_adc.read();
    const uint32_t vTemp = m_adc.readTemperature(ADC::SamplingTime::CYC480, m_cal, vRefInt, 10);
    const uint32_t vVolt = m_adc.readVoltage(ADC::Channel::CH0, ADC::SamplingTime::CYC480, m_cal, vRefInt, 100);
    m_adc.stop();
    m_adcData.vref = 330 * m_cal.cal / vRefInt; // x100
    m_adcData.t = vTemp;
    m_adcData.v = vVolt;
    return true;
}
