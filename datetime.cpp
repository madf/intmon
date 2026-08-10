#include "datetime.h"
#include "utils.h"

FString<10> toString(const Date& d)
{
    FString<10> res;
    res[0] = '0' + (d.year / 1000) % 10;
    res[1] = '0' + (d.year / 100) % 10;
    res[2] = '0' + (d.year / 10) % 10;
    res[3] = '0' + d.year % 10;
    res[4] = '-';
    lzAt(d.month, &res[5]);
    res[7] = '-';
    lzAt(d.day, &res[8]);
    return res;
}

void toStringAt(const Date& d, char* pos)
{
    pos[0] = '0' + (d.year / 1000) % 10;
    pos[1] = '0' + (d.year / 100) % 10;
    pos[2] = '0' + (d.year / 10) % 10;
    pos[3] = '0' + d.year % 10;
    pos[4] = '-';
    lzAt(d.month, &pos[5]);
    pos[7] = '-';
    lzAt(d.day, &pos[8]);
}

FString<5> toString(const Time& t)
{
    FString<5> res;
    lzAt(t.hour, &res[0]);
    res[2] = ':';
    lzAt(t.minute, &res[3]);
    return res;
}

void toStringAt(const Time& t, char* pos)
{
    lzAt(t.hour, &pos[0]);
    pos[2] = ':';
    lzAt(t.minute, &pos[3]);
}

FString<8> toStringFull(const Time& t)
{
    FString<8> res;
    lzAt(t.hour, &res[0]);
    res[2] = ':';
    lzAt(t.minute, &res[3]);
    res[5] = ':';
    lzAt(t.second, &res[6]);
    return res;
}

void toStringFull(const Time& t, char* pos)
{
    lzAt(t.hour, &pos[0]);
    pos[2] = ':';
    lzAt(t.minute, &pos[3]);
    pos[5] = ':';
    lzAt(t.second, &pos[6]);
}
