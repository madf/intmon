#pragma once

#include "rcc.h"
#include "pwr.h"
#include "utils.h"

#include <type_traits>
#include <cstdint>

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

using HSEBase = Base<&RCC::Type::CR, BIT(16) /*on/off*/, BIT(17) /*ready*/, 100 /*ms, timeout*/>;

template <double F>
struct HSE : HSEBase
{
    static constexpr auto freq = F;
};

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

template <typename T>
struct isHSE : std::false_type {};

template <double F>
struct isHSE<HSE<F>> : std::true_type {};

template <typename T>
inline constexpr bool isHSE_v = isHSE<T>::value;

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
        BaseType::enable(timeout);
        return false;
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }
};

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
    static_assert(isSysClockInput_v<I>, "Not a SysClock input clock");
    using Input = I;

    static constexpr double freq = Input::freq;

    static bool isReady()
    {
        return Input::isReady();
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&RCC::Regs->APB1ENR, POWER_INTERFACE_CLOCK_ON); // Enable power interface clock
        setBit(&PWR::Regs->CR, BIT(15)); // Voltage scaling mode 2
        return Input::enable(timeout);
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

template <typename I, uint8_t HPRE, uint8_t PPRE1, uint8_t PPRE2>
struct SysClock : SysClockBase<I>
{
    using Base = SysClockBase<I>;
    using Base::Input;
    using Base::freq;

    static constexpr auto AHBFreq = freq / HPRE;
    static constexpr auto APB1Freq = AHBFreq / PPRE1;
    static constexpr auto APB2Freq = AHBFreq / PPRE2;
};

}
