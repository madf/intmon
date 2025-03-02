#pragma once

#include "display.h"
#include "keyboard.h"
#include "bme280.h"
#include "i2c.h"
#include "fonts.h"
#include "rtc.h"
#include "adc.h"
#include "datetime.h"
#include "timer.h"

#include <cstdint>

class Screen
{
    public:
        Screen(uint8_t pFreq);
        void run();

    private:
        enum class View : uint8_t { DateTime = 0, Temp = 1, Press = 2, Hum = 3, ADC = 4 };

        struct HPT
        {
            uint32_t h = 0;
            uint32_t p = 0;
            int32_t t  = 0;
        };

        struct ADCData
        {
            uint32_t vref = 0;
            uint32_t t = 0;
            uint32_t v = 0;
        };

        using I2C1 = I2C::Port<1>;

        View m_view = View::DateTime;
        I2C1 m_port;
        Display m_display;
        Fonts m_fonts;
        Keyboard m_keyboard;
        BME280 m_sensor;
        ADC::Device m_adc;
        ADC::Calibration m_cal;
        HPT m_hpt;
        ADCData m_adcData;
        DateTime m_dt;
        Timer m_timer;

        void runMenu();
        void show();
        void showDT();
        void showTemp();
        void showPress();
        void showHum();
        void showADC();
        void showCommon();
        void showBME280Failure();
        void showADCFailure();

        void prevView();
        void nextView();

        bool readBME280();
        bool readADC();
};
