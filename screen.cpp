#include "screen.h"

#include "display.h"
#include "fonts.h"

using Test = Screen::Test;

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
