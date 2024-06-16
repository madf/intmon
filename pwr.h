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

class Interface
{
    public:
        static void enable();
        static void disable();
        static bool disableBackupDomainWriteProtection();
        static bool isEnabled();

        static void setVoltageScalingMode(uint8_t m);
};

}
