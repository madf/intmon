#pragma once

#include <chrono>

#include <cstdint>

class Timer
{
    public:
        template <class Rep, class Period>
        explicit Timer(std::chrono::duration<Rep, Period> d)
            : m_start(0),
              m_duration(d)
        {
            reset();
        }

        void reset();
        bool expired();

    private:
        uint32_t m_start;
        std::chrono::milliseconds m_duration;
};
