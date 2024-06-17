#include "datetime.h"
#include "utils.h"

std::string toString(const Date& d)
{
    return std::to_string(d.year) + "-" +
           lz(d.month) + "-" +
           lz(d.day);
}

std::string toString(const Time& t, Time::Format f)
{
    std::string res = lz(t.hour) + ":" + lz(t.minute);
    if (f == Time::Format::Short)
        return res;
    return res + ":" + lz(t.second);
}
