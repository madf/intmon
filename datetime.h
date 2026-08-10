#pragma once

#include "fstring.h"

#include <cstdint>

struct Date
{
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day   = 0;
};

struct Time
{
    enum class Format : uint8_t { Short, Full };
    uint8_t hour   = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
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

FString<10> toString(const Date& d);
void toStringAt(const Date& d, char* pos);
FString<5> toString(const Time& t);
void toStringAt(const Time& t, char* pos);
FString<8> toStringFull(const Time& t);

inline
FString<16> toString(const DateTime& dt)
{
    FString<16> res;
    toStringAt(dt.date, res.data());
    toStringAt(dt.time, &res[12]);
    res[11] = ' ';
    return res;
}
