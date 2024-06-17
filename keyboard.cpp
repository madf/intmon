#include "keyboard.h"

auto Keyboard::get() -> Event
{
    using State = Buttons::Base::State;

    const auto menuState  = m_menuBtn.get();
    const auto plusState  = m_plusBtn.get();
    const auto minusState = m_minusBtn.get();
    const auto escState   = m_escBtn.get();

    if (menuState  == State::PRESSED)
        return makeEvent(Action::Menu);
    if (plusState  == State::PRESSED)
        return makeEvent(Action::Plus);
    if (minusState == State::PRESSED)
        return makeEvent(Action::Minus);
    if (escState   == State::PRESSED)
        return makeEvent(Action::Esc);

    if (!m_ledState)
        return {{}, {}};

    const auto anyRelease = menuState  == State::RELEASED ||
                            plusState  == State::RELEASED ||
                            minusState == State::RELEASED ||
                            escState   == State::RELEASED;

    const auto anyPressed = m_menuBtn.isPressed()  ||
                            m_plusBtn.isPressed()  ||
                            m_minusBtn.isPressed() ||
                            m_escBtn.isPressed();

    if (anyRelease && !anyPressed)
    {
        m_ledState = false;
        return {{}, {LEDAction::Off}};
    }

    return {{}, {}};
}

auto Keyboard::makeEvent(Action a) -> Event
{
    std::optional<LEDAction> la;
    if (!m_ledState)
    {
        m_ledState = true;
        la = LEDAction::On;
    }
    return {{a}, la};
}
