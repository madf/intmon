#pragma once

#include "field.h"

#include <cstdint>

struct PWR
{
    volatile uint32_t CR;  // Control
    volatile uint32_t CSR; // Control & status

    static PWR* get() { return reinterpret_cast<PWR*>(0x40007000); }

    using LPDS   = Field<&PWR::CR, 0, 1>;
    using PDDS   = Field<&PWR::CR, LPDS::End, 1>;
    using CWUF   = Field<&PWR::CR, PDDS::End, 1>;
    using CSBF   = Field<&PWR::CR, CWUF::End, 1>;
    using PVDE   = Field<&PWR::CR, CSBF::End, 1>;
    using PLS    = Field<&PWR::CR, PVDE::End, 3>;
    using DBP    = Field<&PWR::CR, PLS::End, 1>;
    using FPDS   = Field<&PWR::CR, DBP::End, 1>;
    using LPLVDS = Field<&PWR::CR, FPDS::End, 1>;
    using MRLVDS = Field<&PWR::CR, LPLVDS::End, 1>;
    using RES0   = Reserved<MRLVDS::End, 1>;
    using ADCDC1 = Field<&PWR::CR, RES0::End, 1>;
    using VOS    = Field<&PWR::CR, ADCDC1::End, 2>;
    using RES1   = Reserved<VOS::End, 16>;
    static_assert(RES1::End == 32, "PWR::CR layout does not cover exactly 32 bits");

    using WUF    = Field<&PWR::CSR, 0, 1>;
    using SBF    = Field<&PWR::CSR, WUF::End, 1>;
    using PVDO   = Field<&PWR::CSR, SBF::End, 1>;
    using BRR    = Field<&PWR::CSR, PVDO::End, 1>;
    using RES2   = Reserved<BRR::End, 4>;
    using EWUP   = Field<&PWR::CSR, RES2::End, 1>;
    using BRE    = Field<&PWR::CSR, EWUP::End, 1>;
    using RES3   = Reserved<BRE::End, 4>;
    using VOSRDY = Field<&PWR::CSR, RES3::End, 1>;
    using RES4   = Reserved<VOSRDY::End, 17>;
    static_assert(RES4::End == 32, "PWR::CSR layout does not cover exactly 32 bits");

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
};
