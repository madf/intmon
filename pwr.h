#pragma once

#include <cstdint>

namespace PWR
{

struct Type
{
    volatile uint32_t CR;  // Control
    volatile uint32_t CSR; // Control & status
};

inline Type* const Regs = reinterpret_cast<Type*>(0x40007000);

}
