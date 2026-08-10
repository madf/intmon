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

        enum class PVDLevel : uint8_t
        {
            L22 = 0x00, // 2.2 V
            L23 = 0x01, // 2.3 V
            L24 = 0x02, // 2.4 V
            L25 = 0x03, // 2.5 V
            L26 = 0x04, // 2.6 V
            L27 = 0x05, // 2.7 V
            L28 = 0x06, // 2.8 V
            L29 = 0x07  // 2.9 V
        };

        enum class PVDStatus : uint8_t
        {
            Disabled     = 0,
            Normal       = 1,
            Undervoltage = 2
        };

        static void enablePVD(PVDLevel level);
        static PVDStatus getPVDStatus();
};

}
