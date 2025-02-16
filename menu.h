#pragma once

#include "timer.h"

#include <cstdint>

class Display;
struct Fonts;
class Keyboard;

class Menu
{
    public:
        Menu(Display& d, Fonts& f, Keyboard& k);

        void run();
    private:
        enum class Edit : uint8_t { Date = 0, Time = 1 };
        enum class DatePart : uint8_t { Year = 0, Month = 1, Day = 2 };
        enum class TimePart : uint8_t { Hour = 0, Minute = 1 };
        Edit m_edit = Edit::Date;
        Display& m_display;
        Fonts& m_fonts;
        Keyboard& m_keyboard;
        Timer m_blink;

        void show();
        void nextMenu();
        void prevMenu();
        void runEdit();
        void runEditDate();
        void runEditTime();
};
