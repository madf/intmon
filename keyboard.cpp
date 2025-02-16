#include "keyboard.h"

auto Keyboard::get() -> Event
{
    using State = Buttons::Base::State;

    const auto enterState  = m_enterBtn.get();
    const auto plusState   = m_plusBtn.get();
    const auto minusState  = m_minusBtn.get();
    const auto exitState   = m_exitBtn.get();

    if (enterState == State::PRESSED)
        return makeEvent(Action::Enter);
    if (plusState  == State::PRESSED)
        return makeEvent(Action::Plus);
    if (minusState == State::PRESSED)
        return makeEvent(Action::Minus);
    if (exitState  == State::PRESSED)
        return makeEvent(Action::Exit);

    if (!m_ledState)
        return {{}, {}};

    const auto anyRelease = enterState == State::RELEASED ||
                            plusState  == State::RELEASED ||
                            minusState == State::RELEASED ||
                            exitState  == State::RELEASED;

    const auto anyPressed = m_enterBtn.isPressed() ||
                            m_plusBtn.isPressed()  ||
                            m_minusBtn.isPressed() ||
                            m_exitBtn.isPressed();

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
