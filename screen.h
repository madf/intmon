#pragma once

#include "keyboard.h"
#include "datetime.h"

#include <cstdint>

class Display;
struct Fonts;

namespace Screen
{

enum class EditRes : uint8_t { NotDone, Done };

struct Base
{
    public:
        Base(Display& d, Fonts& f) : m_display(d), m_fonts(f) {}

    protected:
        Display& m_display;
        Fonts& m_fonts;
};

class Test : public Base
{
    public:
        Test(Display& d, Fonts& f) : Base(d, f) {}

        void update(const Keyboard::Event& e,
                    bool menuState,
                    bool plusState,
                    bool minusState,
                    bool escState);
};

class Main : public Base
{
    public:
        struct HPT
        {
            uint32_t h;
            uint32_t p;
            int32_t t;
        };

        Main(Display& d, Fonts& f) : Base(d, f) {}
        void update(const HPT& hpt, const DateTime& dt);
        void next();
        void prev();

    private:
        enum class State : uint8_t { DateTime = 0, Temp = 1, Press = 2, Hum = 3 };

        State m_state = State::DateTime;
        HPT m_hpt;
        DateTime m_dt;

        void showDT();
        void showTemp();
        void showPress();
        void showHum();
};

class Menu : public Base
{
    public:
        Menu(Display& d, Fonts& f) : Base(d, f) {}
        void enter();
        void next();
        void prev();

    private:
        enum class State : uint8_t { EditDate = 0, EditTime = 1 };

        State m_state = State::EditDate;
};

class EditDate : public Base
{
    public:
        EditDate(Display& d, Fonts& f, const Date& dt) : Base(d, f), m_date(dt) {}
        EditRes enter();
        void plus();
        void minus();

    private:
        enum class State : uint8_t { EditYear = 0, EditMonth = 1, EditDay = 2 };

        State m_state = State::EditYear;
        Date m_date;
};

class EditTime : public Base
{
    public:
        EditTime(Display& d, Fonts& f, const Time& tm) : Base(d, f), m_time(tm) {}
        EditRes enter();
        void plus();
        void minus();

    private:
        enum class State : uint8_t { EditHour = 0, EditMinute = 1 };

        State m_state = State::EditHour;
        Time m_time;
};

}
