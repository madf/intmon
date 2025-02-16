#include "menu.h"

#include "display.h"
#include "fonts.h"
#include "keyboard.h"
#include "rtc.h"

#include <chrono>

Menu::Menu(Display& d, Fonts& f, Keyboard& k)
    : m_display(d),
      m_fonts(f),
      m_keyboard(k),
      m_blink(std::chrono::milliseconds(300))
{
}

void Menu::run()
{
    show();
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        switch (e.action.value())
        {
            case Action::Enter: runEdit(); break;
            case Action::Plus:  nextMenu(); show(); break;
            case Action::Minus: prevMenu(); show(); break;
            case Action::Exit:  return;
        };
    }
}

void Menu::show()
{
    m_display.clear();
    switch (m_edit)
    {
        case Edit::Date:
            m_display.printAt(75, 2, m_fonts.medium, "Set date");
            break;
        case Edit::Time:
            m_display.printAt(75, 2, m_fonts.medium, "Set time");
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
        m_edit = Edit::Time;
    else
        m_edit = static_cast<Edit>(std::to_underlying(m_edit) - 1);
}

void Menu::runEdit()
{
    switch (m_edit)
    {
        case Edit::Date: return runEditDate();
        case Edit::Time: return runEditTime();
    };
}

void Menu::runEditDate()
{
    auto date = RTC::Device::getDate();
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        switch (e.action.value())
        {
            case Action::Enter: runEdit(); break;
            case Action::Plus:  nextMenu(); show(); break;
            case Action::Minus: prevMenu(); show(); break;
            case Action::Exit:  return;
        };
    }
    RTC::Device::setDate(date);
}

void Menu::runEditTime()
{
    auto time = RTC::Device::getTime();
    while (true)
    {
        const auto e = m_keyboard.get();
        using Action = Keyboard::Action;
        switch (e.action.value())
        {
            case Action::Enter: runEdit(); break;
            case Action::Plus:  nextMenu(); show(); break;
            case Action::Minus: prevMenu(); show(); break;
            case Action::Exit:  return;
        };
    }
    RTC::Device::setTime(time);
}
