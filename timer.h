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

        template <class Rep, class Period>
        static void wait(std::chrono::duration<Rep, Period> d) { Timer(d).wait(); }

        void reset();
        bool expired() const;
        void wait() const;

    private:
        uint32_t m_start;
        std::chrono::milliseconds m_duration;
};
