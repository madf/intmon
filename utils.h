#pragma once

#include <cstdint>

inline
constexpr uint32_t BIT(uint32_t x) { return 1UL << x; }

// #define BIT(x) (1UL << (x))

void spin(uint32_t count);

bool waitBIT(const volatile uint32_t* reg, uint32_t bit, uint32_t timeout);
