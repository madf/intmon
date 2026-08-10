#pragma once

#include <type_traits>

template <typename T>
struct MemberInfo;

template <typename T, typename C>
struct MemberInfo<T C::*>
{
    using Class = C;
    using Type = std::remove_cv_t<T>;
};
