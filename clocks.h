#pragma once

#include "rcc.h"
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

template <volatile uint32_t (RCCType::* Reg), uint32_t OnBit, uint32_t ReadyBit, uint32_t Timeout>
struct Base
{
    static constexpr auto ON = OnBit;
    static constexpr auto READY = ReadyBit;
    static constexpr auto TIMEOUT = Timeout;
    using BaseType = Base<Reg, OnBit, ReadyBit, Timeout>;

    static bool isReady()
    {
        return isBitSet(&(RCC->*Reg), ReadyBit);
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&(RCC->*Reg), OnBit);
        return waitBitOn(&(RCC->*Reg), ReadyBit, timeout);
    }

    static bool enable()
    {
        return enable(Timeout);
    }

    static bool disable(uint32_t timeout)
    {
        clearBit(&(RCC->*Reg), OnBit);
        return waitBitOff(&(RCC->*Reg), ReadyBit, timeout);
    }

    static bool disable()
    {
        return disable(Timeout);
    }

    static bool isEnabled()
    {
        return isBitSet(&(RCC->*Reg), OnBit);
    }
};

using HSIBase = Base<&RCCType::CR, BIT(0) /*on/off*/, BIT(1) /*ready*/, 2 /*ms, timeout*/>;

template <double F = 16.0>
struct HSI : HSIBase
{
    static constexpr auto freq = F;
};

using HSEBase = Base<&RCCType::CR, BIT(16) /*on/off*/, BIT(17) /*ready*/, 100 /*ms, timeout*/>;

template <double F>
struct HSE : HSEBase
{
    static constexpr auto freq = F;
};

using LSIBase = Base<&RCCType::CSR, BIT(0) /*on/off*/, BIT(1) /*ready*/, 2 /*ms, timeout*/>;

template <double F = 32.0>
struct LSI : LSIBase
{
    static constexpr auto freq = F;
};

struct LSEBase : Base<&RCCType::BDCR, BIT(0) /*on/off*/, BIT(1) /*ready*/, 5000 /*ms, timeout*/>
{
    static bool enable(uint32_t timeout)
    {
        setBit(&RCC->APB1ENR, POWER_INTERFACE_CLOCK_ON); // Enable power interface clock
        return BaseType::enable(timeout);
    }

    static bool isEnabled()
    {
        return BaseType::isEnabled() &&
               isBitSet(&RCC->APB1ENR, POWER_INTERFACE_CLOCK_ON);
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

using PLLBase = Base<&RCCType::CR, BIT(24) /*on/off*/, BIT(25) /*ready*/, 2 /*ms, timeout*/>;

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct PLL : PLLBase
{
    static_assert(isPLLInput_v<I>, "Not a PLL input clock");
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
