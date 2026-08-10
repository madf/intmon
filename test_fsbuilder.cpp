#include "fsbuilder.h"

#include <stdexcept>

namespace
{

void test()
{
    unsigned x = 42;
    auto fsb = FSNum(x) + FSLit(" + ") + FSNum<uint32_t, 3>(11) + FSLit(" = ") + FSNum<uint32_t>(53);
    std::string r1;
    for (auto it = fsb.iter(); it; it.next())
        r1 += *it;
    if (r1 != "42 + 11 = 53")
        throw std::runtime_error("Expected: '42 + 11 = 53'. Got: '" + r1 + "'");
    std::string r2;
    for (auto it = fsb.iter(ForcePadding); it; it.next())
        r2 += *it;
    if (r2 != "042 + 011 = 053")
        throw std::runtime_error("Expected: '42 + 11 = 53'. Got: '" + r2 + "'");
    auto fs = fsb.exec();
    if (fs != "042 + 011 = 053")
        throw std::runtime_error("Expected: '42 + 11 = 53'. Got: '" + std::string(fs.data(), fs.size()) + "'");
}

void test1()
{
    uint32_t t = 356;
    const auto fsb = FSNum<uint8_t>(t / 10) + FSLit(".") + FSNum<uint8_t>(t % 10);
    std::string r;
    for (auto it = fsb.iter(); it; it.next())
        r += *it;
    if (r != "35.6")
        throw std::runtime_error("Expected: '35.6'. Gor: '" + r + "'");
}

}

int main()
{
    //test();
    test1();
    return 0;
}
