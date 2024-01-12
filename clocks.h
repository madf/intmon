#pragma once

#include "rcc.h"
#include "pwr.h"
#include "utils.h"

#include <cstdint>
#include <utility> // std::to_underlying
#include <type_traits>

/*
 * using PLL = Clocks::PLL<Clocks::HSE<25>, 25, 336, 4, 7>
 * using SysClock = Clocks::SysClock<PLL, 1, 2, 1>
 *
 */

namespace Clocks
{

inline constexpr auto POWER_INTERFACE_CLOCK_ON = BIT(28);

template <volatile uint32_t (RCC::Type::* Reg), uint32_t OnBit, uint32_t ReadyBit, uint32_t Timeout>
struct Base
{
    static constexpr auto ON = OnBit;
    static constexpr auto READY = ReadyBit;
    static constexpr auto TIMEOUT = Timeout;
    using BaseType = Base<Reg, OnBit, ReadyBit, Timeout>;

    static bool isReady()
    {
        return isBitSet(&(RCC::Regs->*Reg), ReadyBit);
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&(RCC::Regs->*Reg), OnBit);
        return waitBitOn(&(RCC::Regs->*Reg), ReadyBit, timeout);
    }

    static bool enable()
    {
        return enable(Timeout);
    }

    static bool disable(uint32_t timeout)
    {
        clearBit(&(RCC::Regs->*Reg), OnBit);
        return waitBitOff(&(RCC::Regs->*Reg), ReadyBit, timeout);
    }

    static bool disable()
    {
        return disable(Timeout);
    }

    static bool isEnabled()
    {
        return isBitSet(&(RCC::Regs->*Reg), OnBit);
    }
};

using HSIBase = Base<&RCC::Type::CR, BIT(0) /*on/off*/, BIT(1) /*ready*/, 2 /*ms, timeout*/>;

template <double F = 16.0>
struct HSI : HSIBase
{
    static constexpr auto freq = F;
};

template <typename T>
struct isHSI : std::false_type {};

template <double F>
struct isHSI<HSI<F>> : std::true_type {};

template <typename T>
inline constexpr bool isHSI_v = isHSI<T>::value;

using HSEBase = Base<&RCC::Type::CR, BIT(16) /*on/off*/, BIT(17) /*ready*/, 100 /*ms, timeout*/>;

template <double F>
struct HSE : HSEBase
{
    static constexpr auto freq = F;
};

template <typename T>
struct isHSE : std::false_type {};

template <double F>
struct isHSE<HSE<F>> : std::true_type {};

template <typename T>
inline constexpr bool isHSE_v = isHSE<T>::value;

using LSIBase = Base<&RCC::Type::CSR, BIT(0) /*on/off*/, BIT(1) /*ready*/, 2 /*ms, timeout*/>;

template <double F = 32.0>
struct LSI : LSIBase
{
    static constexpr auto freq = F;
};

struct LSEBase : Base<&RCC::Type::BDCR, BIT(0) /*on/off*/, BIT(1) /*ready*/, 5000 /*ms, timeout*/>
{
    static bool enable(uint32_t timeout)
    {
        struct PICDisabler
        {
            PICDisabler() { disable = !isBitSet(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON); }
            ~PICDisabler() { if (disable) clearBit(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON); }
            bool disable;
        } atExit;

        setBit(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON); // Enable power interface clock
        setBit(&PWR::Regs->CR, BIT(8)); // Disable write protection for backup domain
        if (!waitBitOn(&PWR::Regs->CR, BIT(8), 2))
            return false;
        return BaseType::enable(timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }
};

template <double F>
struct LSE : LSEBase
{
    static constexpr auto freq = F;
};

template <typename T>
struct isRTCInput : std::false_type {};

template <double F>
struct isRTCInput<LSI<F>> : std::true_type {};

template <double F>
struct isRTCInput<LSE<F>> : std::true_type {};

template <double F>
struct isRTCInput<HSE<F>> : std::true_type {};

template <typename T>
inline constexpr bool isRTCInput_v = isRTCInput<T>::value;

template <typename T>
struct isPLLInput : std::false_type {};

template <double F>
struct isPLLInput<HSI<F>> : std::true_type {};

template <double F>
struct isPLLInput<HSE<F>> : std::true_type {};

template <typename T>
inline constexpr bool isPLLInput_v = isPLLInput<T>::value;

using PLLBase = Base<&RCC::Type::CR, BIT(24) /*on/off*/, BIT(25) /*ready*/, 2 /*ms, timeout*/>;

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct PLL : PLLBase
{
    static_assert(isPLLInput_v<I>, "Not a PLL input clock");
    static_assert(Q > 1 && Q < 16, "Q must be in [2, 15] range");
    static_assert(P / 2 > 0 && P / 2 < 5, "P must be exactly one of [2, 4, 6, 8]");
    static_assert(N > 1 && N < 433, "N must be in [2, 433] range");
    static_assert(M > 1 && M < 64, "M must be in [2, 63] range");
    using Input = I;
    static constexpr auto m = M;
    static constexpr auto n = N;
    static constexpr auto p = P;
    static constexpr auto q = Q;

    static constexpr auto VCOFreq = Input::freq / M;
    static constexpr auto USB48MHz = VCOFreq * N / Q;
    static constexpr auto Output = VCOFreq * N / P;

    static constexpr double freq = Output;

    //static_assert(static_cast<unsigned>(USB48MHz) == 48, "USB clock must be 48 MHz");

    static bool enable(uint32_t timeout)
    {
        if (!Input::enable())
            return false;

        BaseType::disable(timeout);
        clearBit(&RCC::Regs->PLLCFGR, 0x0F << 24);
        setBit(&RCC::Regs->PLLCFGR, q << 24);

        clearBit(&RCC::Regs->PLLCFGR, BIT(22));
        if constexpr (isHSE_v<Input>)
            setBit(&RCC::Regs->PLLCFGR, BIT(22));

        clearBit(&RCC::Regs->PLLCFGR, 0x03 << 16);
        setBit(&RCC::Regs->PLLCFGR, (p / 2 - 1) << 16);

        clearBit(&RCC::Regs->PLLCFGR, 0x01FF << 6);
        setBit(&RCC::Regs->PLLCFGR, n << 6);

        clearBit(&RCC::Regs->PLLCFGR, 0x3F);
        setBit(&RCC::Regs->PLLCFGR, m);

        return BaseType::enable(timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }
};

template <typename T>
struct isPLL : std::false_type {};

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct isPLL<PLL<I, M, N, P, Q>> : std::true_type {};

template <typename T>
inline constexpr bool isPLL_v = isPLL<T>::value;

template <typename T>
struct isSysClockInput : std::false_type {};

template <double F>
struct isSysClockInput<HSI<F>> : std::true_type {};

template <double F>
struct isSysClockInput<HSE<F>> : std::true_type {};

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct isSysClockInput<PLL<I, M, N, P, Q>> : std::true_type {};

template <typename T>
inline constexpr bool isSysClockInput_v = isSysClockInput<T>::value;

template <typename I>
struct SysClockBase
{
    using Input = I;

    static_assert(isSysClockInput_v<Input>, "SysClock input must be either HSI, HSE or PLL");

    static constexpr double freq = Input::freq;

    static bool isReady()
    {
        return Input::isReady();
    }

    static bool enable(uint32_t timeout)
    {
        if (!Input::enable(timeout))
            return false;

        setBit(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON); // Enable power interface clock
        setBit(&PWR::Regs->CR, BIT(15)); // Voltage scaling mode 2
    }

    static bool enable()
    {
        return Input::enable();
    }

    static bool disable(uint32_t timeout)
    {
        return Input::disable(timeout);
    }

    static bool disable()
    {
        return Input::disable();
    }

    static bool isEnabled()
    {
        return Input::isEnabled();
    }
};

enum class HPRE : uint16_t {
    DIV1   = 1,
    DIV2   = 2,
    DIV4   = 4,
    DIV8   = 8,
    DIV16  = 16,
    DIV64  = 64,
    DIV128 = 128,
    DIV256 = 256,
    DIV512 = 512
};

inline consteval uint8_t HPREBits(HPRE d)
{
    switch (d)
    {
        case HPRE::DIV1:   return 0x00; // Correct, high bit is zero
        case HPRE::DIV2:   return 0x08; // Correct, high bit is always 1 for non-1 divizors
        case HPRE::DIV4:   return 0x09;
        case HPRE::DIV8:   return 0x0A;
        case HPRE::DIV16:  return 0x0B;
        case HPRE::DIV64:  return 0x0C;
        case HPRE::DIV128: return 0x0D;
        case HPRE::DIV256: return 0x0E;
        case HPRE::DIV512: return 0x0F;
    };
    return 0x00; // Just in case
}

enum class PPRE : uint8_t {
    DIV1  = 1,
    DIV2  = 2,
    DIV4  = 4,
    DIV8  = 8,
    DIV16 = 16
};

inline consteval uint8_t PPREBits(PPRE d)
{
    switch (d)
    {
        case PPRE::DIV1:  return 0x00; // Correct, high bit is zero
        case PPRE::DIV2:  return 0x04; // Correct, high bit is always 1 for non-1 divizors
        case PPRE::DIV4:  return 0x05;
        case PPRE::DIV8:  return 0x06;
        case PPRE::DIV16: return 0x07;
    };
    return 0x00; // Just in case
}

template <typename I, HPRE AHBDiv, PPRE APB1Div, PPRE APB2Div>
struct SysClock : SysClockBase<I>
{
    using Base = SysClockBase<I>;
    using Input = Base::Input;
    using Base::freq;

    static constexpr auto AHBFreq = freq / std::to_underlying(AHBDiv);
    static constexpr auto APB1Freq = AHBFreq / std::to_underlying(APB1Div);
    static constexpr auto APB2Freq = AHBFreq / std::to_underlying(APB2Div);

    static bool enable(uint32_t timeout)
    {
        if (!Input::enable())
            return false;

        // Set APBx prescalers to max value to not exceed frequencies accidentally during configuration
        setBit(&RCC::Regs->CFGR, 0x07 << 10); // Set APB1 prescaler to 16
        setBit(&RCC::Regs->CFGR, 0x07 << 13); // Set APB2 prescaler to 16

        // Set AHB prescaler
        clearBit(&RCC::Regs->CFGR, 0x0F << 4);
        setBit(&RCC::Regs->CFGR, HPREBits(AHBDiv) << 4);

        // Set clock source
        uint8_t inputClockCode = 0x00; // HSI is default, 00
        if constexpr (isHSE_v<Input>)
            inputClockCode = 0x01; // HSE is 01
        else if constexpr (isPLL_v<Input>)
            inputClockCode = 0x02; // PLL is 10
        // 11 is not applicable
        clearBit(&RCC::Regs->CFGR, 0x03);
        setBit(&RCC::Regs->CFGR, inputClockCode);

        // Verify clock source
        if (!waitBitOn(&RCC::Regs->CFGR, inputClockCode << 2, timeout))
            return false;

        // Set APB1 prescaler
        clearBit(&RCC::Regs->CFGR, 0x03 << 10);
        setBit(&RCC::Regs->CFGR, PPREBits(APB1Div) << 10);

        // Set APB2 prescaler
        clearBit(&RCC::Regs->CFGR, 0x03 << 13);
        setBit(&RCC::Regs->CFGR, PPREBits(APB2Div) << 10);

        return true;
    }

    static bool enable()
    {
        return enable(5000); // 5 ms timeout by default
    }
};

}
