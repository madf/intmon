#include "fstring.h"

#include <stdexcept>
#include <string>

namespace
{

void test()
{
    FString<16> ts1;
    if (ts1.size() != 16)
        throw std::runtime_error("Expected size is 16. Got " + std::to_string(ts1.size()) + ".");
    ts1.assign("123456");
    if (ts1 != "123456")
        throw std::runtime_error("Fist 6 symbols must be '123456'. Got '" + std::string(ts1.data(), 6) + "'.");
    if (ts1[6] != '\0')
        throw std::runtime_error("Zero must be at position 6. Got " + std::to_string(ts1[6]));
    FString<8> ts2("12345678");
    if (ts2.size() != 8)
        throw std::runtime_error("Expected size is 16. Got " + std::to_string(ts2.size()) + ".");
    if (ts2 != "12345678")
        throw std::runtime_error("The string must be '12345678'. Got '" + std::string(ts2.data(), 8) + "'.");
    auto ts3 = ts2 + ts1;
    if (ts3.size() != 24)
        throw std::runtime_error("Expected size is 24. Got " + std::to_string(ts3.size()) + ".");
    if (ts3 != "12345678123456")
        throw std::runtime_error("The string must be '12345678132456'. Got '" + std::string(ts3.data(), 14) + "'.");
    auto ts4 = ts2 + "abc";
    if (ts4.size() != 12)
        throw std::runtime_error("Expected size is 11. Got " + std::to_string(ts4.size()) + ".");
    if (ts4 != "12345678abc")
        throw std::runtime_error("The string must be '12345678abc'. Got '" + std::string(ts4.data(), 11) + "'.");
    FString<4> ts5;
    ts5.setDigitAt<0>(1);
    ts5.setDigitAt<1>(11);
    ts5.setDigitAt<2>(9);
    ts5.setDigitAt<3>(5);
    if (ts5 != "1195")
        throw std::runtime_error("The string must bu '1195'. Got '" + std::string(ts5.data(), 4) + "'.");
    ts5.fillAt<1, 2>('*');
    if (ts5 != "1**5")
        throw std::runtime_error("The string must bu '1**5'. Got '" + std::string(ts5.data(), 4) + "'.");
}

}

int main()
{
    test();
    return 0;
}
