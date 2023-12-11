#pragma once

#include "rcc.h"
#include "utils.h"

#include <type_traits>
#include <cstdint>

/*
 * using PLL = Clocks::PLL<HSE<25>, 25, 336, 4, 7>
 * using SysClock = Clocks::SysClock<PLL, 1, 2, 1>
 *
 */

namespace Clocks
{

inline constexpr auto HSI_ON = BIT(0);
inline constexpr auto HSI_READY = BIT(1);
inline constexpr uint32_t HSI_TIMEOUT = 2; // 2 ms
inline constexpr auto HSE_ON = BIT(16);
inline constexpr auto HSE_READY = BIT(17);
inline constexpr uint32_t HSE_TIMEOUT = 100; // 100 ms
inline constexpr auto LSI_ON = BIT(0);
inline constexpr auto LSI_READY = BIT(1);
inline constexpr uint32_t LSI_TIMEOUT = 2; // 2 ms
inline constexpr auto LSE_ON = BIT(0);
inline constexpr auto LSE_READY = BIT(1);
inline constexpr uint32_t LSE_TIMEOUT = 5000; // 2 ms
inline constexpr auto POWER_INTERFACE_CLOCK_ON = BIT(28);
inline constexpr auto PLL_ON = BIT(24);
inline constexpr auto PLL_READY = BIT(25);
inline constexpr uint32_t PLL_TIMEOUT = 2; // 2 ms

template <double F = 16.0>
struct HSI
{
    static constexpr auto freq = F;

    static bool enable(uint32_t timeout)
    {
        RCC->CR |= HSI_ON;
        return waitBIT(&RCC->CR, HSI_READY, timeout);
    }

    static bool enable()
    {
        return enable(HSI_TIMEOUT);
    }
};

template <double F>
struct HSE
{
    static constexpr auto freq = F;

    static bool enable(uint32_t timeout)
    {
        RCC->CR |= HSE_ON;
        return waitBIT(&RCC->CR, HSE_READY, timeout);
    }

    static bool enable()
    {
        return enable(HSE_TIMEOUT);
    }
};

template <double F = 32.0>
struct LSI
{
    static constexpr auto freq = F;

    static bool enable(uint32_t timeout)
    {
        RCC->CSR |= LSI_ON;
        return waitBIT(&RCC->CSR, LSI_READY, timeout);
    }

    static bool enable()
    {
        return enable(LSI_TIMEOUT);
    }
};

template <double F>
struct LSE
{
    static constexpr auto freq = F;

    static bool enable(uint32_t timeout)
    {
        RCC->APB1ENR |= POWER_INTERFACE_CLOCK_ON; // Enable power interfaec clock
        RCC->BDCR |= LSE_ON;
        return waitBIT(&RCC->BDCR, LSE_READY, timeout);
    }

    static bool enable()
    {
        return enable(LSE_TIMEOUT);
    }
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

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct PLL
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

    static bool enable(uint32_t timeout)
    {
        RCC->CR |= PLL_ON;
        return waitBIT(&RCC->CR, PLL_READY, timeout);
    }

    static bool enable()
    {
        return enable(PLL_TIMEOUT);
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

template <typename I, uint8_t HPRE, uint8_t PPRE1, uint8_t PPRE2>
struct SysClock
{
    static_assert(isSysClockInput_v<I>, "Not a SysClock input clock");
    using Input = I;

    static constexpr double freq = Input::freq;

    static constexpr auto AHBFreq = freq / HPRE;
    static constexpr auto APB1Freq = AHBFreq / PPRE1;
    static constexpr auto APB2Freq = AHBFreq / PPRE2;

    static void enable()
    {
        Input::enable();
    }
};

}
