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

struct HSIBase
{
    static constexpr auto ON = BIT(0);
    static constexpr auto READY = BIT(1);
    static constexpr uint32_t TIMEOUT = 2; // 2 ms

    static bool isReady()
    {
        return isBitSet(&RCC->CR, READY);
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&RCC->CR, ON);
        return waitBIT(&RCC->CR, READY, timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }

    static bool isEnabled()
    {
        return isBitSet(&RCC->CR, ON);
    }
};

template <double F = 16.0>
struct HSI : HSIBase
{
    static constexpr auto freq = F;
};

struct HSEBase
{
    static constexpr auto ON = BIT(16);
    static constexpr auto READY = BIT(17);
    static constexpr uint32_t TIMEOUT = 100; // 100 ms

    static bool isReady()
    {
        return isBitSet(&RCC->CR, READY);
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&RCC->CR, ON);
        return waitBIT(&RCC->CR, READY, timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }

    static bool isEnabled()
    {
        return isBitSet(&RCC->CR, ON);
    }
};

template <double F>
struct HSE : HSEBase
{
    static constexpr auto freq = F;
};

struct LSIBase
{
    static constexpr auto ON = BIT(0);
    static constexpr auto READY = BIT(1);
    static constexpr uint32_t TIMEOUT = 2; // 2 ms

    static bool isReady()
    {
        return isBitSet(&RCC->CSR, READY);
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&RCC->CSR, ON);
        return waitBIT(&RCC->CSR, READY, timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }

    static bool isEnabled()
    {
        return isBitSet(&RCC->CSR, ON);
    }
};

template <double F = 32.0>
struct LSI : LSIBase
{
    static constexpr auto freq = F;
};

struct LSEBase
{
    static constexpr auto ON = BIT(0);
    static constexpr auto READY = BIT(1);
    static constexpr uint32_t TIMEOUT = 5000; // 2 ms

    static bool isReady()
    {
        return isBitSet(&RCC->BDCR, READY);
    }

    static bool enable(uint32_t timeout)
    {
        setBit(&RCC->APB1ENR, POWER_INTERFACE_CLOCK_ON); // Enable power interface clock
        setBit(&RCC->BDCR, ON);
        return waitBIT(&RCC->BDCR, READY, timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }

    static bool isEnabled()
    {
        return isBitSet(&RCC->BDCR, ON) &&
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

struct PLLBase
{
    static constexpr auto ON = BIT(24);
    static constexpr auto READY = BIT(25);
    static constexpr uint32_t TIMEOUT = 2; // 2 ms

    static bool enable(uint32_t timeout)
    {
        setBit(&RCC->CR, ON);
        return waitBIT(&RCC->CR, READY, timeout);
    }

    static bool enable()
    {
        return enable(TIMEOUT);
    }

    static bool isEnabled()
    {
        return isBitSet(&RCC->CR, ON);
    }
};

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

    static bool enable(uint32_t timeout)
    {
        return Input::enable(timeout);
    }

    static bool enable()
    {
        return Input::enable();
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
