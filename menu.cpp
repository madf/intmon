#include "menu.h"

#include "timer.h"
#include "display.h"
#include "fonts.h"
#include "keyboard.h"
#include "rtc.h"
#include "datetime.h"
#include "flash.h"
#include "scb.h"

#include <chrono>
#include <utility> // std::unreachable

namespace
{

constexpr uint8_t daysInMonth(uint16_t year, uint8_t month)
{
    switch (month)
    {
        case 1:  return 31; // Jan
        case 2:             // Feb
            if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
                return 29;
            return 28;
        case 3:  return 31; // Mar
        case 4:  return 30; // Apr
        case 5:  return 31; // May
        case 6:  return 30; // Jun
        case 7:  return 31; // Jul
        case 8:  return 31; // Aug
        case 9:  return 30; // Sep
        case 10: return 31; // Oct
        case 11: return 30; // Nov
        case 12: return 31; // Dec
    };
    std::unreachable();
}

void incInRange(uint8_t& v, uint8_t l, uint8_t h)
{
    if (v == h)
        v = l;
    else
        ++v;
}

void decInRange(uint8_t& v, uint8_t l, uint8_t h)
{
    if (v == l)
        v = h;
    else --v;
}

}

Menu::Menu(Display& d, Fonts& f, Keyboard& k)
    : m_display(d),
      m_fonts(f),
      m_keyboard(k)
{
}

void Menu::run()
{
    show();
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        if (e.action)
        {
            switch (e.action.value())
            {
                case Action::Enter: runImpl(); return;
                case Action::Plus:  nextMenu(); show(); break;
                case Action::Minus: prevMenu(); show(); break;
                case Action::Exit:  return;
            };
        }
    }
}

void Menu::show()
{
    m_display.clear();
    switch (m_edit)
    {
        case Edit::Date:
            m_display.printAt(15, 2, m_fonts.medium, "Set date");
            break;
        case Edit::Time:
            m_display.printAt(15, 2, m_fonts.medium, "Set time");
            break;
        case Edit::BOR:
            m_display.printAt(15, 2, m_fonts.medium, "BOR");
            break;
        case Edit::Reset:
            m_display.printAt(15, 2, m_fonts.medium, "Reset");
            break;
    };
    m_display.update();
}

void Menu::nextMenu()
{
    m_edit = static_cast<Edit>((std::to_underlying(m_edit) + 1) % 4);
}

void Menu::prevMenu()
{
    if (m_edit == Edit::Date)
        m_edit = Edit::Reset;
    else
        m_edit = static_cast<Edit>(std::to_underlying(m_edit) - 1);
}

void Menu::runImpl()
{
    switch (m_edit)
    {
        case Edit::Date: return runEditDate();
        case Edit::Time: return runEditTime();
        case Edit::BOR: return runSetBOR();
        case Edit::Reset: return runReset();
    };
}

void Menu::runEditDate()
{
    auto date = RTC::Device::getDate();
    DatePart part = DatePart::Year;
    bool showPart = true;
    showEditDate(date, part, showPart);
    Timer blink(std::chrono::milliseconds(300));
    bool done = false;
    const auto next = [](DatePart p){ return static_cast<DatePart>((std::to_underlying(p) + 1) % 3); };
    const auto incPart = [](DatePart p, Date& d)
    {
        switch (p)
        {
            case DatePart::Year: ++d.year; break;
            case DatePart::Month: incInRange(d.month, 1, 12); break;
            case DatePart::Day: incInRange(d.day, 1, daysInMonth(d.year, d.month)); break;
        };
    };
    const auto decPart = [](DatePart p, Date& d)
    {
        switch (p)
        {
            case DatePart::Year: --d.year; break;
            case DatePart::Month: decInRange(d.month, 1, 12); break;
            case DatePart::Day: decInRange(d.day, 1, daysInMonth(d.year, d.month)); break;
        };
    };
    while (!done)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        if (e.action)
        {
            switch (e.action.value())
            {
                case Action::Enter:
                    if (part == DatePart::Day)
                        done = true;
                    else
                        part = next(part);
                    break;
                case Action::Plus:  incPart(part, date); break;
                case Action::Minus: decPart(part, date); break;
                case Action::Exit:  return;
            };
        }
        if (blink.expired())
        {
            showPart = !showPart;
            blink.reset();
        }
        showEditDate(date, part, showPart);
    }
    RTC::Device::setDate(date);
}

void Menu::runEditTime()
{
    auto time = RTC::Device::getTime();
    TimePart part = TimePart::Hour;
    bool showPart = true;
    showEditTime(time, part, showPart);
    Timer blink(std::chrono::milliseconds(300));
    bool done = false;
    const auto next = [](TimePart p){ return static_cast<TimePart>((std::to_underlying(p) + 1) % 3); };
    const auto incPart = [](TimePart p, Time& t)
    {
        switch (p)
        {
            case TimePart::Hour: incInRange(t.hour, 0, 23); break;
            case TimePart::Minute: incInRange(t.minute, 0, 59); break;
            case TimePart::Second: incInRange(t.second, 0, 59); break;
        };
    };
    const auto decPart = [](TimePart p, Time& t)
    {
        switch (p)
        {
            case TimePart::Hour: decInRange(t.hour, 0, 23); break;
            case TimePart::Minute: decInRange(t.minute, 0, 59); break;
            case TimePart::Second: decInRange(t.second, 0, 59); break;
        };
    };
    while (!done)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        if (e.action)
        {
            switch (e.action.value())
            {
                case Action::Enter:
                    if (part == TimePart::Second)
                        done = true;
                    else
                        part = next(part);
                    break;
                case Action::Plus:  incPart(part, time); break;
                case Action::Minus: decPart(part, time); break;
                case Action::Exit:  return;
            };
        }
        if (blink.expired())
        {
            showPart = !showPart;
            blink.reset();
        }
        showEditTime(time, part, showPart);
    }
    RTC::Device::setTime(time);
}

void Menu::runSetBOR()
{
    showSetBOR();
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        if (e.action)
        {
            switch (e.action.value())
            {
                case Action::Enter:
                    Flash::Interface::setBORLevel(Flash::Interface::BORLevel::L2);
                    SCB::Interface::systemReset();
                    break;
                case Action::Exit:  return;
                default: break;
            };
        }
    }
}

void Menu::runReset()
{
    showReset();
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        if (e.action)
        {
            switch (e.action.value())
            {
                case Action::Enter:
                    SCB::Interface::systemReset();
                    break;
                case Action::Exit:  return;
                default: break;
            };
        }
    }
}

void Menu::showEditDate(const Date& dt, DatePart part, bool showPart)
{
    m_display.clear();
    auto dts = toString(dt);
    if (!showPart)
    {
        if (part == DatePart::Year)
            dts.fillAt<0, 4>(' ');
        else if (part == DatePart::Month)
            dts.fillAt<5, 2>(' ');
        else if (part == DatePart::Day)
            dts.fillAt<8, 2>(' ');
    }

    m_display.printAt(0, 0, m_fonts.big, dts);
    m_display.update();
}

void Menu::showEditTime(const Time& tm, TimePart part, bool showPart)
{
    m_display.clear();
    auto ts = toStringFull(tm);
    if (!showPart)
    {
        if (part == TimePart::Hour)
            ts.fillAt<0, 2>(' ');
        else if (part == TimePart::Minute)
            ts.fillAt<3, 2>(' ');
        else if (part == TimePart::Second)
            ts.fillAt<6, 2>(' ');
    }

    m_display.printAt(0, 0, m_fonts.big, ts);
    m_display.update();
}

void Menu::showSetBOR()
{
    m_display.clear();
    m_display.printAt(0, 0, m_fonts.medium, "Set BOR");
    m_display.printAt(0, 20, m_fonts.tiny, "Yes?");
    m_display.update();
}

void Menu::showReset()
{
    m_display.clear();
    m_display.printAt(0, 0, m_fonts.medium, "Reset");
    m_display.printAt(0, 20, m_fonts.tiny, "Yes?");
    m_display.update();
}
