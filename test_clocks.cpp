#include "clocks.h"

using HPRE = Clocks::HPRE;
using PPRE = Clocks::PPRE;

void testHSI()
{
    using SysClock = Clocks::SysClock<Clocks::HSI<>, HPRE::DIV1, PPRE::DIV2, PPRE::DIV1>;

    static_assert(static_cast<unsigned>(SysClock::freq) == 16);
    static_assert(static_cast<unsigned>(SysClock::AHBFreq) == 16);
    static_assert(static_cast<unsigned>(SysClock::APB1Freq) == 8);
    static_assert(static_cast<unsigned>(SysClock::APB2Freq) == 16);
}

void testHSE()
{
    using SysClock = Clocks::SysClock<Clocks::HSE<25.0>, HPRE::DIV1, PPRE::DIV2, PPRE::DIV1>;

    static_assert(static_cast<unsigned>(SysClock::freq) == 25);
    static_assert(static_cast<unsigned>(SysClock::AHBFreq) == 25);
    static_assert(static_cast<unsigned>(SysClock::APB1Freq) == 12);
    static_assert(static_cast<unsigned>(SysClock::APB2Freq) == 25);
}

void testPLLHSI()
{
    using PLL = Clocks::PLL<Clocks::HSI<>, 16, 144, 4, 5>;
    using SysClock = Clocks::SysClock<PLL, HPRE::DIV1, PPRE::DIV2, PPRE::DIV1>;

    static_assert(static_cast<unsigned>(PLL::freq) == 36);
    static_assert(static_cast<unsigned>(PLL::VCOFreq) == 1);

    static_assert(static_cast<unsigned>(SysClock::freq) == 36);
    static_assert(static_cast<unsigned>(SysClock::AHBFreq) == 36);
    static_assert(static_cast<unsigned>(SysClock::APB1Freq) == 18);
    static_assert(static_cast<unsigned>(SysClock::APB2Freq) == 36);
}

void testPLLHSE()
{
    using PLL = Clocks::PLL<Clocks::HSE<25.0>, 25, 336, 4, 7>;
    using SysClock = Clocks::SysClock<PLL, HPRE::DIV1, PPRE::DIV2, PPRE::DIV1>;

    static_assert(static_cast<unsigned>(PLL::freq) == 84);
    static_assert(static_cast<unsigned>(PLL::VCOFreq) == 1);
    static_assert(static_cast<unsigned>(PLL::USB48MHz) == 48);

    static_assert(static_cast<unsigned>(SysClock::freq) == 84);
    static_assert(static_cast<unsigned>(SysClock::AHBFreq) == 84);
    static_assert(static_cast<unsigned>(SysClock::APB1Freq) == 42);
    static_assert(static_cast<unsigned>(SysClock::APB2Freq) == 84);
}

int main()
{
    testHSI();
    testHSE();
    testPLLHSI();
    testPLLHSE();
    return 0;
}
