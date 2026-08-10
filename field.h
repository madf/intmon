#pragma once

#include "timer.h"
#include "minfo.h"

#include <type_traits>
#include <cstdint>

template <auto RegPtr, uint8_t Offset, uint8_t W>
struct FieldBase
{
    static constexpr auto Width = W;
    using Info = MemberInfo<decltype(RegPtr)>;
    using Subsystem = typename Info::Class;
    static_assert(std::is_same_v<typename Info::Type, uint32_t>, "the register must be of the uint32_t type");
    static_assert(Offset + Width <= 32, "field doesn't fit in a 32-bit register");
    static constexpr uint8_t End = Offset + Width;
    static constexpr uint32_t Mask = ((1u << Width) - 1u) << Offset;

    static auto& reg() { return Subsystem::get()->*RegPtr; }

    static uint32_t read()
    {
        return (reg() & Mask) >> Offset;
    }

    static void write(uint32_t value)
    {
        reg() = (reg() & ~Mask) | ((value << Offset) & Mask);
    }
};

template <auto RegPtr, uint8_t Offset, uint8_t Width>
struct Field : FieldBase<RegPtr, Offset, Width> {};

template <auto RegPtr>
struct Field<RegPtr, 0, 32> : FieldBase<RegPtr, 0, 32>
{
    using Base = FieldBase<RegPtr, 0, 32>;
    static uint32_t read()
    {
        return Base::reg();
    }

    static void write(uint32_t value)
    {
        Base::reg() = value;
    }
};

template <auto RegPtr, uint8_t Offset>
struct Field<RegPtr, Offset, 1> : private FieldBase<RegPtr, Offset, 1>
{
    using Base = FieldBase<RegPtr, Offset, 1>;
    using Base::Width;
    using Base::End;
    static bool isSet() { return Base::read() > 0; }
    static void set() { Base::write(1); }
    static void clear() { Base::write(0); }
    static bool waitOn()
    {
        while (!isSet())
            asm("nop");
        return isSet();
    }
    static bool waitOn(const Timer& timer)
    {
        while (!timer.expired() && !isSet())
            asm("nop");
        return isSet();
    }
    template <class Rep, class Period>
    static bool waitOn(const std::chrono::duration<Rep, Period>& d)
    {
        return waitOn(Timer(d));
    }
    static bool waitOff()
    {
        while (isSet())
            asm("nop");
        return !isSet();
    }
    static bool waitOff(const Timer& timer)
    {
        while (!timer.expired() && isSet())
            asm("nop");
        return !isSet();
    }
    template <class Rep, class Period>
    static bool waitOff(const std::chrono::duration<Rep, Period>& d)
    {
        return waitOff(Timer(d));
    }
};

template <typename T>
struct isField : std::false_type {};

template <auto RegPtr, uint8_t Offset, uint8_t Width>
struct isField<Field<RegPtr, Offset, Width>> : std::true_type {};

template <typename T>
inline constexpr bool isFieldV = isField<T>::value;

template <typename T>
struct isFlagField : std::integral_constant<bool, isFieldV<T> && T::Width == 1> {};

template <typename T>
inline constexpr bool isFlagFieldV = isFlagField<T>::value;

template <uint8_t Offset, uint8_t Width>
struct Reserved
{
    static_assert(Offset + Width <= 32, "field doesn't fit in a 32-bit register");
    static constexpr uint8_t End = Offset + Width;
};
