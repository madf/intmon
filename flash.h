#pragma once

#include "field.h"

#include <cstdint>

struct Flash
{
    volatile uint32_t ACR;      // Access control
    volatile uint32_t KEYR;     // Key
    volatile uint32_t OPTKEYR_; // Option key
    volatile uint32_t SR;       // Status
    volatile uint32_t CR;       // Control
    volatile uint32_t OPTCR;    // Option control

    static Flash* get() { return reinterpret_cast<Flash*>(0x40023C00); }

    using LATENCY = Field<&Flash::ACR, 0, 4>;
    using RES0    = Reserved<LATENCY::End, 4>;
    using PRFTEN  = Field<&Flash::ACR, RES0::End, 1>;
    using ICEN    = Field<&Flash::ACR, PRFTEN::End, 1>;
    using DCEN    = Field<&Flash::ACR, ICEN::End, 1>;
    using ICRST   = Field<&Flash::ACR, DCEN::End, 1>;
    using DCRST   = Field<&Flash::ACR, ICRST::End, 1>;
    using RES1    = Reserved<DCRST::End, 19>;
    static_assert(RES1::End == 32, "Flash::ACR layout does not cover exactly 32 bits");

    using FKEYR = Field<&Flash::KEYR, 0, 32>;
    static_assert(FKEYR::End == 32, "Flash::KEYR layout does not cover exactly 32 bits");

    using OPTKEYR = Field<&Flash::OPTKEYR_, 0, 32>;
    static_assert(OPTKEYR::End == 32, "Flash::OPTKEYR layout does not cover exactly 32 bits");

    using EOP    = Field<&Flash::SR, 0, 1>;
    using OPERR  = Field<&Flash::SR, EOP::End, 1>;
    using RES2   = Reserved<OPERR::End, 2>;
    using WRPERR = Field<&Flash::SR, RES2::End, 1>;
    using PGAERR = Field<&Flash::SR, WRPERR::End, 1>;
    using PGPERR = Field<&Flash::SR, PGAERR::End, 1>;
    using PGSERR = Field<&Flash::SR, PGPERR::End, 1>;
    using RDERR  = Field<&Flash::SR, PGSERR::End, 1>;
    using RES3   = Reserved<RDERR::End, 7>;
    using BSY    = Field<&Flash::SR, RES3::End, 1>;
    using RES4   = Reserved<BSY::End, 15>;
    static_assert(RES4::End == 32, "Flash::SR layout does not cover exactly 32 bits");

    using PG    = Field<&Flash::CR, 0, 1>;
    using SER   = Field<&Flash::CR, PG::End, 1>;
    using MER   = Field<&Flash::CR, SER::End, 1>;
    using SNB   = Field<&Flash::CR, MER::End, 4>;
    using RES5  = Reserved<SNB::End, 1>;
    using PSIZE = Field<&Flash::CR, RES5::End, 2>;
    using RES6  = Reserved<PSIZE::End, 6>;
    using STRT  = Field<&Flash::CR, RES6::End, 1>;
    using RES7  = Reserved<STRT::End, 7>;
    using EOPIE = Field<&Flash::CR, RES7::End, 1>;
    using ERRIE = Field<&Flash::CR, EOPIE::End, 1>;
    using RES8  = Reserved<ERRIE::End, 5>;
    using LOCK  = Field<&Flash::CR, RES8::End, 1>;
    static_assert(LOCK::End == 32, "Flash::CR layout does not cover exactly 32 bits");

    using OPTLOCK    = Field<&Flash::OPTCR, 0, 1>;
    using OPTSTRT    = Field<&Flash::OPTCR, OPTLOCK::End, 1>;
    using BORLEV     = Field<&Flash::OPTCR, OPTSTRT::End, 2>;
    using RES9       = Reserved<BORLEV::End, 1>;
    using WDG_SW     = Field<&Flash::OPTCR, RES9::End, 1>;
    using nRST_STOP  = Field<&Flash::OPTCR, WDG_SW::End, 1>;
    using nRST_STDBY = Field<&Flash::OPTCR, nRST_STOP::End, 1>;
    using RDP        = Field<&Flash::OPTCR, nRST_STDBY::End, 8>;
    using nWRP       = Field<&Flash::OPTCR, RDP::End, 8>;
    using RES10      = Reserved<nWRP::End, 7>;
    using SPRMOD     = Field<&Flash::OPTCR, RES10::End, 1>;
    static_assert(SPRMOD::End == 32, "Flash::OPTCR layout does not cover exactly 32 bits");

    class Interface
    {
        public:
            enum class BORLevel : uint8_t
            {
                L3       = 0x00, // VBOR3, 2.9 V
                L2       = 0x01, // VBOR2, 2.6 V
                L1       = 0x02, // VBOR1, 2.3 V
                Disabled = 0x03  // BOR is disabled
            };

            static BORLevel getBORLevel();
            static void setBORLevel(BORLevel level);
    };
};
