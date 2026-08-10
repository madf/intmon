#include "screen.h"

#include "menu.h"
#include "clocks.h"
#include "pwr.h"
#include "flash.h"

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
      m_timer(std::chrono::seconds(1)),
      m_deadTimer(std::chrono::seconds(10))
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

            if (m_adcData.batPerc() < 0)
            {
                if (m_deadTimer.expired())
                {
                    m_display.clear();
                    showDeadBat();
                    m_display.update();
                    return;
                }
            }
            else
            {
                m_deadTimer.reset();
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
    m_view = static_cast<View>((std::to_underlying(m_view) + 1) % 7);
}

void Screen::prevView()
{
    if (m_view == View::DateTime)
        m_view = View::PM;
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
        case View::State:    showState(); break;
        case View::ADC:      showADC(); break;
        case View::PM:       showPM(); break;
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

void Screen::showState()
{
    const auto temp = FSNum<uint8_t>(m_adcData.t / 10) + FSLit(".") + FSNum<uint8_t>(m_adcData.t % 10);
    m_display.printAt(4, 2, m_fonts.tiny, temp, 1, Display::NoBG);
    m_display.printAt(40, 2, m_fonts.tiny, "C");
    const auto perc = m_adcData.batPerc();
    m_display.rect(2, 14, 60, 18, Display::Color::White);
    m_display.bar(62, 20, 3, 6, Display::Color::White);
    m_display.bar(3, 15, 58 * perc / 100, 16, Display::Color::White);
    m_display.printAt(14, 19, m_fonts.tiny, std::to_string(perc), 1, Display::NoBG);
    m_display.printAt(14 + m_fonts.tiny.width() * 4, 19, m_fonts.tiny, "%", 1, Display::NoBG);
}

void Screen::showADC()
{
    FString<4> temp;
    if (m_adcData.t > 999)
        temp = " err";
    else
    {
        lzAt(m_adcData.t / 10, &temp[0]);
        temp[2] = '.';
        temp[3] = '0' + m_adcData.t % 10;
    }
    m_display.printAt(0, 2, m_fonts.tiny, temp);
    m_display.printAt(40, 2, m_fonts.tiny, "C");
    const auto v = m_adcData.vbat();
    FString<4> volt;
    if (v > 999)
        volt = " err";
    else
    {
        volt[0] = '0' + (v / 100) % 10;
        volt[1] = '.';
        lzAt(v % 100, &volt[2]);
    }
    m_display.printAt(0, 12, m_fonts.tiny, volt);
    m_display.printAt(40, 12, m_fonts.tiny, "V");
    FString<3> perc(3, ' ');
    auto p = m_adcData.batPerc();
    if (p > 100 || p < -99)
        perc = "err";
    else
    {
        if (p == 100)
            perc = "100";
        else
        {
            if (p < 0)
            {
                perc[0] = '-';
                p = -p;
            }
            lzAt(p, &perc[1]);
        }
    }
    m_display.printAt(0, 22, m_fonts.tiny, perc);
    m_display.printAt(40, 22, m_fonts.tiny, "%");
}

void Screen::showPM()
{
    m_display.printAt(0, 2, m_fonts.tiny, "BOR:");
    const auto bor = std::to_underlying(Flash::Interface::getBORLevel());
    if (bor > 2)
        m_display.printAt(40, 2, m_fonts.tiny, "Dis");
    else
        m_display.printAt(40, 2, m_fonts.tiny, FSLit("L") + FSNum<uint8_t>(bor + 1), 1, Display::NoBG);
    m_display.printAt(0, 12, m_fonts.tiny, "PVD:");
    switch (PWR::Interface::getPVDStatus())
    {
        case PWR::Interface::PVDStatus::Disabled:
            m_display.printAt(40, 12, m_fonts.tiny, "Dis");
            break;
        case PWR::Interface::PVDStatus::Normal:
            m_display.printAt(40, 12, m_fonts.tiny, "Nor");
            break;
        case PWR::Interface::PVDStatus::Undervoltage:
            m_display.printAt(40, 12, m_fonts.tiny, "Und");
            break;
        default:
            m_display.printAt(40, 12, m_fonts.tiny, "?");
            break;
    };
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

void Screen::showDeadBat()
{
    const auto v = m_adcData.vbat();
    FString<4> volt;
    if (v > 999)
        volt = " err";
    else
    {
        volt[0] = '0' + (v / 100) % 10;
        volt[1] = '.';
        lzAt(v % 100, &volt[2]);
    }
    m_display.printAt(40, 12, m_fonts.tiny, volt);
    m_display.printAt(80, 12, m_fonts.tiny, "V");
    m_display.rect(34, 7, 60, 18, Display::Color::White);
    m_display.bar(94, 13, 3, 6, Display::Color::White);
    m_display.line(49, 31, 79, 0, Display::Color::White);
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
