#pragma once

#include "field.h"

#include <chrono>
#include <cstdint>

struct SysTick
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;

    using ENABLE    = Field<&SysTick::CTRL, 0, 1>;
    using TICKINT   = Field<&SysTick::CTRL, ENABLE::End, 1>;
    using CLKSOURCE = Field<&SysTick::CTRL, TICKINT::End, 1>;
    using RES0      = Reserved<CLKSOURCE::End, 13>;
    using COUNTFLAG = Field<&SysTick::CTRL, RES0::End, 1>;
    using RES1      = Reserved<COUNTFLAG::End, 15>;
    static_assert(RES1::End == 32, "SysTick::CTRL layout does not cover exactly 32 bits");

    using RELOAD = Field<&SysTick::LOAD, 0, 24>;
    using RES2   = Reserved<RELOAD::End, 8>;
    static_assert(RES2::End == 32, "SysTick::LOAD layout does not cover exactly 32 bits");

    using CURRENT = Field<&SysTick::VAL, 0, 24>;
    using RES3    = Reserved<CURRENT::End, 8>;
    static_assert(RES3::End == 32, "SysTick::VAL layout does not cover exactly 32 bits");

    using TENMS = Field<&SysTick::CALIB, 0, 24>;
    using RES4  = Reserved<TENMS::End, 6>;
    using SKEW  = Field<&SysTick::CALIB, RES4::End, 1>;
    using NOREF = Field<&SysTick::CALIB, SKEW::End, 1>;
    static_assert(NOREF::End == 32, "SysTick::CALIB layout does not cover exactly 32 bits");

    static SysTick* get() { return reinterpret_cast<SysTick*>(0xE000E010); }

    class Interface
    {
        public:
            static void init(uint32_t ticks);
            static uint32_t getTick();

            static void delayMS(unsigned ms);
            static void delayUS(uint8_t ahbFreq, unsigned us);
            static void delayUS(unsigned us);

            template <class Rep, class Period>
            inline
            static void delay(std::chrono::duration<Rep, Period> d)
            {
                delayMS(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
            }
    };
};
