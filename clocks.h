#pragma once

#include "rcc.h"
#include "pwr.h"
#include "field.h"
#include "utils.h"

#include <chrono>
#include <utility> // std::to_underlying
#include <type_traits>
#include <cstdint>

/*
 * using PLL = Clocks::PLL<Clocks::HSE<25>, 25, 336, 4, 7>
 * using SysClock = Clocks::SysClock<PLL, 1, 2, 1>
 *
 */

namespace Clocks
{

template <typename OnField, typename ReadyField>
struct Base
{
    static_assert(isFlagFieldV<OnField>, "OnField must be a single-bit field");
    static_assert(isFlagFieldV<ReadyField>, "ReadyField must be a single-bit field");
    using BaseType = Base<OnField, ReadyField>;

    static bool isReady()
    {
        return ReadyField::isSet();
    }

    template <class Rep, class Period>
    static bool enable(std::chrono::duration<Rep, Period> timeout)
    {
        OnField::set();
        return ReadyField::waitOn(timeout);
    }

    template <class Rep, class Period>
    static bool disable(std::chrono::duration<Rep, Period> timeout)
    {
        OnField::clear();
        return ReadyField::waitOff(timeout);
    }

    static bool isEnabled()
    {
        return OnField::isSet();
    }
};

template <double F = 16.0>
struct HSI : Base<RCC::HSION, RCC::HSIRDY>
{
    static constexpr auto freq = F;
    static constexpr auto timeout = std::chrono::milliseconds(2);
};


template <typename T>
struct isHSI : std::false_type {};

template <double F>
struct isHSI<HSI<F>> : std::true_type {};

template <typename T>
inline constexpr bool isHSIV = isHSI<T>::value;

template <double F>
struct HSE : Base<RCC::HSEON, RCC::HSERDY>
{
    static constexpr auto freq = F;
    static constexpr auto timeout = std::chrono::milliseconds(100);

    static bool enable()
    {
        return BaseType::enable(timeout);
    }
};

template <typename T>
struct isHSE : std::false_type {};

template <double F>
struct isHSE<HSE<F>> : std::true_type {};

template <typename T>
inline constexpr bool isHSEV = isHSE<T>::value;

template <double F = 32.0>
struct LSI : Base<RCC::LSION, RCC::LSIRDY>
{
    static constexpr auto freq = F;
    static constexpr auto timeout = std::chrono::milliseconds(2);
};

template <double F>
struct LSE : Base<RCC::LSEON, RCC::LSERDY>
{
    static constexpr auto freq = F;
    static constexpr auto timeout = std::chrono::milliseconds(5000);

    template <class Rep, class Period>
    static bool enable(std::chrono::duration<Rep, Period> timeout)
    {
        PWR::Interface::enable();

        if (!PWR::Interface::disableBackupDomainWriteProtection())
            return false;
        return BaseType::enable(timeout);
    }

    static bool enable()
    {
        return enable(timeout);
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
inline constexpr bool isRTCInputV = isRTCInput<T>::value;

template <typename T>
struct isPLLInput : std::false_type {};

template <double F>
struct isPLLInput<HSI<F>> : std::true_type {};

template <double F>
struct isPLLInput<HSE<F>> : std::true_type {};

template <typename T>
inline constexpr bool isPLLInputV = isPLLInput<T>::value;

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct PLL : Base<RCC::PLLON, RCC::PLLRDY>
{
    static_assert(isPLLInputV<I>, "Not a PLL input clock");
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
    static constexpr auto timeout = std::chrono::milliseconds(2);

    //static_assert(static_cast<unsigned>(USB48MHz) == 48, "USB clock must be 48 MHz");

    template <class Rep, class Period>
    static bool enable(std::chrono::duration<Rep, Period> timeout)
    {
        if (!Input::enable())
            return false;

        BaseType::disable(timeout);

        RCC::PLLQ::write(q);
        if constexpr (isHSEV<Input>)
            RCC::PLLSRC::set();
        else
            RCC::PLLSRC::clear();

        RCC::PLLP::write(p / 2 - 1);
        RCC::PLLN::write(n);
        RCC::PLLM::write(m);

        return BaseType::enable(timeout);
    }

    static bool enable()
    {
        return enable(timeout);
    }
};

template <typename T>
struct isPLL : std::false_type {};

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct isPLL<PLL<I, M, N, P, Q>> : std::true_type {};

template <typename T>
inline constexpr bool isPLLV = isPLL<T>::value;

template <typename T>
struct isSysClockInput : std::false_type {};

template <double F>
struct isSysClockInput<HSI<F>> : std::true_type {};

template <double F>
struct isSysClockInput<HSE<F>> : std::true_type {};

template <typename I, uint8_t M, uint16_t N, uint8_t P, uint8_t Q>
struct isSysClockInput<PLL<I, M, N, P, Q>> : std::true_type {};

template <typename T>
inline constexpr bool isSysClockInputV = isSysClockInput<T>::value;

template <typename I>
struct SysClockBase
{
    using Input = I;

    static_assert(isSysClockInputV<Input>, "SysClock input must be either HSI, HSE or PLL");

    static constexpr double freq = Input::freq;

    static bool isReady()
    {
        return Input::isReady();
    }

    template <class Rep, class Period>
    static bool enable(std::chrono::duration<Rep, Period> timeout)
    {
        if (!Input::enable(timeout))
            return false;

        PWR::Interface::setVoltageScalingMode(1);
        return true;
    }

    static bool enable()
    {
        return Input::enable();
    }

    template <class Rep, class Period>
    static bool disable(std::chrono::duration<Rep, Period> timeout)
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

    template <class Rep, class Period>
    static bool enable(std::chrono::duration<Rep, Period> timeout)
    {
        if (!Input::enable())
            return false;

        // Set APBx prescalers to max value to not exceed frequencies accidentally during configuration
        RCC::PPRE1::write(0x07); // Set APB1 prescaler to 16
        RCC::PPRE2::write(0x07); // Set APB2 prescaler to 16

        // Set AHB prescaler
        RCC::HPRE::write(HPREBits(AHBDiv));

        // Set clock source
        uint8_t inputClockCode = 0x00; // HSI is default, 00
        if constexpr (isHSEV<Input>)
            inputClockCode = 0x01; // HSE is 01
        else if constexpr (isPLLV<Input>)
            inputClockCode = 0x02; // PLL is 10
        // 11 is not applicable
        RCC::SW::write(inputClockCode);

        // Verify clock source
        Timer timer(timeout);
        while (!timer.expired() && RCC::SWS::read() != inputClockCode)
            asm("nop");
        if (RCC::SWS::read() != inputClockCode)
            return false;

        RCC::PPRE1::write(PPREBits(APB1Div)); // Set APB1 prescaler
        RCC::PPRE2::write(PPREBits(APB2Div)); // Set APB2 prescaler

        return true;
    }

    static bool enable()
    {
        return enable(std::chrono::milliseconds(5000)); // 5 s timeout by default
    }
};

}
