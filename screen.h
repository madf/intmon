#pragma once

#include "display.h"
#include "keyboard.h"
#include "bme280.h"
#include "i2c.h"
#include "fonts.h"
#include "rtc.h"
#include "datetime.h"
#include "timer.h"

#include <cstdint>

class Screen
{
    public:
        Screen(double pFreq);
        void run();

    private:
        enum class View : uint8_t { DateTime = 0, Temp = 1, Press = 2, Hum = 3 };

        struct HPT
        {
            uint32_t h = 0;
            uint32_t p = 0;
            int32_t t  = 0;
        };

        using I2C1 = I2C::Port<1>;

        View m_view = View::DateTime;
        I2C1 m_port;
        Display m_display;
        Fonts m_fonts;
        Keyboard m_keyboard;
        BME280 m_sensor;
        Timer m_timer;

        void runMenu();
        void show(const HPT& hpt, const DateTime& dt);
        void showDT(const DateTime& dt);
        void showTemp(int32_t t);
        void showPress(uint32_t p);
        void showHum(uint32_t h);
        void showCommon(const HPT& hpt);
        void showBME280Failure();

        void prevView();
        void nextView();
};
