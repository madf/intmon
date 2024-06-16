#pragma once

#include <array>
#include <cstdint>

namespace RTC
{

struct Type
{
    volatile uint32_t TR;                   // Time
    volatile uint32_t DR;                   // Date
    volatile uint32_t CR;                   // Control
    volatile uint32_t ISR;                  // Initialization & status
    volatile uint32_t PRER;                 // Prescaler
    volatile uint32_t WUTR;                 // Wakeup timer
    volatile uint32_t CALIBR;               // Calibration
    volatile uint32_t ALRMAR;               // Alarm A
    volatile uint32_t ALRMBR;               // Alarm B
    volatile uint32_t WPR;                  // Write protection
    volatile uint32_t SSR;                  // Subsecond
    volatile uint32_t SHIFTR;               // Shift control
    volatile uint32_t TSTR;                 // Time stamp time
    volatile uint32_t TSDR;                 // Time stamp date
    volatile uint32_t TSSSR;                // Time stamp subsecond
    volatile uint32_t CALR;                 // Calibration
    volatile uint32_t TAFCR;                // Tamper and alternate function configuration
    volatile uint32_t ALRMASSR;             // Alarm A subsecond
    volatile uint32_t ALRMBSSR;             // Alarm B subsecond
    volatile std::array<uint32_t, 20> BKPR; // Backup 0-19
};

inline Type* const Regs = reinterpret_cast<Type*>(0x40002800);

class Device
{
    public:
        enum class ClockSource : uint8_t {
            NONE = 0x00,
            LSE = 0x01,
            LSI = 0x02,
            HSE = 0x03
        };

        enum class Format {
            Normal,
            AmPm
        };

        enum class Output : uint8_t {
            Disable = 0x00,
            AlarmA = 0x01,
            AlarmB = 0x02,
            WakeUp = 0x03
        };

        enum class OutputType {
            OpenDrain,
            PushPull
        };

        enum class Polarity {
            High,
            Low
        };

        struct Config
        {
            ClockSource clockSource;
            uint8_t asyncPreDiv;
            uint16_t syncPreDiv;
            Format format;
            Output output;
            OutputType outputType;
            Polarity polarity;

            Config()
                : clockSource(ClockSource::LSE),
                  asyncPreDiv(0x7F),
                  syncPreDiv(0xFF),
                  format(Format::Normal),
                  output(Output::Disable),
                  outputType(OutputType::OpenDrain),
                  polarity(Polarity::High)
            {}
        };

        struct Date
        {
            uint8_t year;
            uint8_t month;
            uint8_t day;
        };

        struct Time
        {
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        };

        struct DateTime
        {
            Date date;
            Time time;

            auto year() const { return date.year; }
            auto month() const { return date.month; }
            auto day() const { return date.day; }

            auto hour() const { return time.hour; }
            auto minute() const { return time.minute; }
            auto second() const { return time.second; }
        };

        static bool init(const Config& config);
        static bool init() { return init({}); }

        static bool resync();

        static Date getDate();
        static Time getTime();
        static DateTime get() { return {getDate(), getTime()}; }
        static bool set(uint8_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);

    private:
        static void enable();
        static bool enterInit();
        static bool exitInit();
        static void wpEnable();
        static void wpDisable();

        static void setClockSource(ClockSource cs);
        static void setFormat(Format f);
        static void setOutput(Output o);
        static void setPolarity(Polarity p);
        static void setOutputType(OutputType t);
};

}
