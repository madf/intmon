#pragma once

#include <cstdint>

class Display;
struct Fonts;
class Keyboard;
struct Date;
struct Time;

class Menu
{
    public:
        Menu(Display& d, Fonts& f, Keyboard& k);

        void run();
    private:
        enum class Edit : uint8_t { Date = 0, Time = 1, BOR = 2, Reset = 3 };
        enum class DatePart : uint8_t { Year = 0, Month = 1, Day = 2 };
        enum class TimePart : uint8_t { Hour = 0, Minute = 1, Second = 2 };
        Edit m_edit = Edit::Date;
        Display& m_display;
        Fonts& m_fonts;
        Keyboard& m_keyboard;

        void show();
        void nextMenu();
        void prevMenu();
        void runImpl();
        void runEditDate();
        void runEditTime();
        void runSetBOR();
        void runReset();
        void showEditDate(const Date& dt, DatePart part, bool showPart);
        void showEditTime(const Time& tm, TimePart part, bool showPart);
        void showSetBOR();
        void showReset();
};
