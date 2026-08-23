#pragma once

#include "timer.h"
#include "minfo.h"

#include <tuple>
#include <type_traits>
#include <cstdint>

template <auto RegPtr>
struct Reg
{
    using Info = MemberInfo<decltype(RegPtr)>;
    using Subsystem = typename Info::Class;
    static_assert(std::is_same_v<typename Info::Type, uint32_t>, "The register must be of the uint32_t type");
    static auto& reg() { return Subsystem::get()->*RegPtr; }
    static void regWrite(uint32_t v, uint32_t mask) { reg() = (reg() & ~mask) | (v & mask); }
    static uint32_t regRead(uint32_t mask) { return reg() & mask; }
};

template <auto RP, uint8_t Off, uint8_t W>
struct FieldBase : Reg<RP>
{
    using Base = Reg<RP>;
    static constexpr auto RegPtr = RP;
    static constexpr auto Offset = Off;
    static constexpr auto Width = W;
    static_assert(Offset + Width <= 32, "field doesn't fit in a 32-bit register");
    static constexpr uint8_t End = Offset + Width;
    static constexpr uint32_t Mask = ((1u << Width) - 1u) << Offset;

    static uint32_t read()
    {
        return Base::regRead(Mask) >> Offset;
    }

    static void write(uint32_t value)
    {
        Base::regWrite(value << Offset, Mask);
    }
};

template <auto RegPtr, uint8_t Offset, uint8_t Width>
struct Field : FieldBase<RegPtr, Offset, Width> {};

template <auto RegPtr>
struct Field<RegPtr, 0, 32> : FieldBase<RegPtr, 0, 32>
{
    using Base = FieldBase<RegPtr, 0, 32>;
    using Base::Mask;
    using Base::Offset;
    static uint32_t read()
    {
        return Base::reg();
    }

    static void write(uint32_t value)
    {
        Base::reg() = value;
    }
};

template <auto RP, uint8_t Off>
struct Field<RP, Off, 1> : private FieldBase<RP, Off, 1>
{
    using Base = FieldBase<RP, Off, 1>;
    using Base::RegPtr;
    using Base::Mask;
    using Base::Offset;
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

template <typename T, typename... Ts>
inline constexpr bool sameRegisterFieldsV = ((T::RegPtr == Ts::RegPtr) && ...);

template <typename T, typename... Ts>
struct GetCommonReg
{
    static_assert(sameRegisterFieldsV<T, Ts...>, "The fields map to different registers.");
    using Type = Reg<T::RegPtr>;
};

template <typename... Ts>
using GetCommonRegT = GetCommonReg<Ts...>::Type;

template <typename T, typename VT>
inline
constexpr uint32_t fieldBits(VT v)
{
    if constexpr (isFlagFieldV<T>)
    {
        static_assert(std::is_same_v<VT, bool>, "Flag fields must have boolean value.");
        return (v ? 1UL : 0UL) << T::Offset;
    }
    else if constexpr (isFieldV<T>)
    {
        static_assert(std::is_unsigned_v<VT>, "Numeric fields must have unsigned numeric value.");
        return (static_cast<uint32_t>(v) << T::Offset) & T::Mask;
    }
    else
    {
        static_assert(false, "This function must be used with field type parameter.");
    }
}

template <typename... Ts, typename... VTs>
inline
constexpr std::tuple<uint32_t, uint32_t> composeFieldBits(VTs... vs)
{
    static_assert((isFieldV<Ts> && ...), "Field composition must happen on register fields.");
    static_assert(sameRegisterFieldsV<Ts...>, "Field composition must happen on fields of the same register.");
    constexpr uint32_t mask = (Ts::Mask | ...);
    return {(fieldBits<Ts>(vs) | ...) & mask, mask};
}

template <typename... Ts, typename... VTs>
inline
void groupWrite(VTs... vs)
{
    static_assert(sameRegisterFieldsV<Ts...>, "Field composition must happen on fields of the same register.");
    using R = GetCommonRegT<Ts...>;
    std::apply(R::regWrite, composeFieldBits<Ts...>(vs...));
}
