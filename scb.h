#pragma once

#include <array>
#include <cstdint>

namespace SCB
{

struct Type
{
    volatile uint32_t CPUID;  // CPU ID
    volatile uint32_t ICSR; // Interrupt control and state
    volatile uint32_t VTOR; // Vector table offset
    volatile uint32_t AIRCR; // Application interrupt control
    volatile uint32_t SCR; // System control
    volatile uint32_t CCR; // Configuration and control
    volatile std::array<uint32_t, 3> SHRP; // System handler priority
    volatile uint32_t SHCSR; // System handler control and state
    volatile uint32_t CFSR; // Configurable fault status
    volatile uint32_t HFSR; // Hard fault status
    volatile uint32_t MMFAR; // Memory management fault address
    volatile uint32_t BFAR; // Bus fault address
    volatile uint32_t AFSR; // Auxiliary fault status
};

inline Type* const Regs = reinterpret_cast<Type*>(0xE000ED00);

class Interface
{
    public:
        static void systemReset();
        static void sleep();
};

}
