#include "utils.h"

#include <string>
#include <iostream>

int fail(const std::string& message)
{
    std::cout << message << "\n";
    return -1;
}

int main()
{
    uint32_t reg = 0;
    const auto testBit1 = BIT(0);
    const auto testBit2 = BIT(3);
    const auto testPattern = BIT(1) | BIT(5);

    if (isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly set.");

    if (isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly set.");

    if (isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly set.");

    setBit(&reg, testBit1);

    if (!isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly unset.");

    if (isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly set.");

    if (isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly set.");

    setBit(&reg, testBit2);

    if (!isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly unset.");

    if (!isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly unset.");

    if (isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly set.");

    setBit(&reg, BIT(1));

    if (!isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly unset.");

    if (!isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly unset.");

    if (isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly set.");

    setBit(&reg, BIT(5));

    if (!isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly unset.");

    if (!isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly unset.");

    if (!isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly unset.");

    clearBit(&reg, testPattern);

    if (!isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly unset.");

    if (!isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly unset.");

    if (isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly set.");
    if (isBitSet(&reg, BIT(1)))
        return fail("Bit 1 from testPattern is unexpectedly set.");
    if (isBitSet(&reg, BIT(5)))
        return fail("Bit 1 from testPattern is unexpectedly set.");

    setBit(&reg, testPattern);

    if (!isBitSet(&reg, testBit1))
        return fail("testBit1 is unexpectedly unset.");

    if (!isBitSet(&reg, testBit2))
        return fail("testBit2 is unexpectedly unset.");

    if (!isBitSet(&reg, testPattern))
        return fail("testPattern is unexpectedly unset.");

    return 0;
}
