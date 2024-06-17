#pragma once

#include <string>
#include <cstdint>

struct Date
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
};

struct Time
{
    enum class Format : uint8_t { Short, Full };
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

std::string toString(const Date& d);
std::string toString(const Time& t, Time::Format f);

inline
std::string toString(const Time& t)
{
    return toString(t, Time::Format::Short);
}

inline
std::string toString(const DateTime& dt)
{
    return toString(dt.date) + " " + toString(dt.time, Time::Format::Full);
}

inline
std::string toString(const DateTime& dt, Time::Format f)
{
    return toString(dt.date) + " " + toString(dt.time, f);
}
