#include "screen.h"

#include "display.h"
#include "fonts.h"

using Test = Screen::Test;
using Main = Screen::Main;

namespace
{

std::string toString(Keyboard::Action v)
{
    switch (v)
    {
        case Keyboard::Action::Menu: return "M";
        case Keyboard::Action::Plus: return "+";
        case Keyboard::Action::Minus: return "-";
        case Keyboard::Action::Esc: return "E";
    };
    return "?";
}

std::string toString(Keyboard::LEDAction v)
{
    switch (v)
    {
        case Keyboard::LEDAction::On: return "On";
        case Keyboard::LEDAction::Off: return "Off";
    };
    return "?";
}

std::string formatTemp(int32_t v)
{
    return std::to_string(v / 10) + "." + std::to_string(v % 10);
}

}

void Test::update(const Keyboard::Event& e,
                  bool menuState,
                  bool plusState,
                  bool minusState,
                  bool escState)
{
    m_display.clear();
    if (e.action)
        m_display.printAt(0, 0, m_fonts.medium, "A: " + toString(e.action.value()));
    else
        m_display.printAt(0, 0, m_fonts.medium, "A: -");
    if (e.ledAction)
        m_display.printAt(0, 16, m_fonts.medium, "L: " + toString(e.ledAction.value()));
    else
        m_display.printAt(0, 16, m_fonts.medium, "L: -");
    m_display.rect(64, 0, 64, 16, Display::Color::White);
    m_display.vline(80, 0, 16, Display::Color::White);
    m_display.vline(96, 0, 16, Display::Color::White);
    m_display.vline(112, 0, 16, Display::Color::White);
    if (menuState)
        m_display.bar(66, 2, 12, 12, Display::Color::White);
    if (plusState)
        m_display.bar(82, 2, 12, 12, Display::Color::White);
    if (minusState)
        m_display.bar(98, 2, 12, 12, Display::Color::White);
    if (escState)
        m_display.bar(114, 2, 12, 12, Display::Color::White);
    m_display.update();
}

void Main::update(const HPT& hpt, const DateTime& dt)
{
    m_hpt = hpt;
    m_dt = dt;
    show();
}

void Main::next()
{
    m_state = static_cast<State>((std::to_underlying(m_state) + 1) % 4);
    show();
}

void Main::prev()
{
    if (m_state == State::DateTime)
        m_state = State::Hum;
    else
        m_state = static_cast<State>(std::to_underlying(m_state) - 1);

    show();
}

void Main::show()
{
    switch (m_state)
    {
        case State::DateTime: return showDT();
        case State::Temp:     return showTemp();
        case State::Press:    return showPress();
        case State::Hum:      return showHum();
    }
}

void Main::showDT()
{
    m_display.clear();

    showCommon();

    m_display.printAt(0, 0, m_fonts.big, toString(m_dt.time));
    m_display.printAt(0, 22, m_fonts.tiny, toString(m_dt.date));

    m_display.update();
}

void Main::showTemp()
{
    m_display.clear();

    showCommon();

    m_display.printAt(0, 0, m_fonts.big, formatTemp(m_hpt.t));
    m_display.printAt(50, 0, m_fonts.big, "C");

    m_display.update();
}

void Main::showPress()
{
    m_display.clear();

    showCommon();

    m_display.printAt(0, 0, m_fonts.big, std::to_string(m_hpt.p));
    m_display.printAt(40, 0, m_fonts.big, "mm");

    m_display.update();
}

void Main::showHum()
{
    m_display.clear();

    showCommon();

    m_display.printAt(0, 0, m_fonts.big, std::to_string(m_hpt.h));
    m_display.printAt(40, 0, m_fonts.big, "%");

    m_display.update();
}

void Main::showCommon()
{
    m_display.printAt(75, 2,  m_fonts.tiny, formatTemp(m_hpt.t));
    m_display.printAt(75, 12, m_fonts.tiny, std::to_string(m_hpt.p));
    m_display.printAt(75, 22, m_fonts.tiny, std::to_string(m_hpt.h));
    m_display.printAt(107, 2,  m_fonts.tiny, "C");
    m_display.printAt(100, 12, m_fonts.tiny, "mmhg");
    m_display.printAt(107, 22, m_fonts.tiny, "%");

    m_display.vline(71, 0, 32, Display::Color::White);
}
