#pragma once

#include "field.h"

#include <cstdint>

struct SCB
{
    volatile uint32_t CPUID;  // CPU ID
    volatile uint32_t ICSR; // Interrupt control and state
    volatile uint32_t VTOR; // Vector table offset
    volatile uint32_t AIRCR; // Application interrupt control
    volatile uint32_t SCR; // System control
    volatile uint32_t CCR; // Configuration and control
    volatile uint32_t SHPR1; // System handler priority
    volatile uint32_t SHPR2; // System handler priority
    volatile uint32_t SHPR3; // System handler priority
    volatile uint32_t SHCSR; // System handler control and state
    volatile uint32_t CFSR; // Configurable fault status
    volatile uint32_t HFSR; // Hard fault status
    volatile uint32_t MMFAR; // Memory management fault address
    volatile uint32_t BFAR; // Bus fault address
    volatile uint32_t AFSR; // Auxiliary fault status

    using REVISION    = Field<&SCB::CPUID, 0, 4>;
    using PARTNO      = Field<&SCB::CPUID, REVISION::End, 12>;
    using CONSTANT    = Field<&SCB::CPUID, PARTNO::End, 4>;
    using VARIANT     = Field<&SCB::CPUID, CONSTANT::End, 4>;
    using IMPLEMENTER = Field<&SCB::CPUID, VARIANT::End, 8>;
    static_assert(IMPLEMENTER::End == 32, "SCB::CPUID layout does not cover exactly 32 bits");

    using VECTACTIVE  = Field<&SCB::ICSR, 0, 9>;
    using RES0        = Reserved<VECTACTIVE::End, 2>;
    using RETOBASE    = Field<&SCB::ICSR, RES0::End, 1>;
    using VECTPENDING = Field<&SCB::ICSR, RETOBASE::End, 7>;
    using RES1        = Reserved<VECTPENDING::End, 3>;
    using ISRPENDING  = Field<&SCB::ICSR, RES1::End, 1>;
    using RES3        = Reserved<ISRPENDING::End, 2>;
    using PENDSTCLR   = Field<&SCB::ICSR, RES3::End, 1>;
    using PENDSTSET   = Field<&SCB::ICSR, PENDSTCLR::End, 1>;
    using PENDSVCLR   = Field<&SCB::ICSR, PENDSTSET::End, 1>;
    using PENDSVSET   = Field<&SCB::ICSR, PENDSVCLR::End, 1>;
    using RES4        = Reserved<PENDSVSET::End, 2>;
    using NMIPENDSET  = Field<&SCB::ICSR, RES4::End, 1>;
    static_assert(NMIPENDSET::End == 32, "SCB::ICSR layout does not cover exactly 32 bits");

    using RES5   = Reserved<0, 9>;
    using TBLOFF = Field<&SCB::VTOR, RES5::End, 21>;
    using RES6   = Reserved<TBLOFF::End, 2>;
    static_assert(RES6::End == 32, "SCB::VTOR layout does not cover exactly 32 bits");

    using VECTRESET     = Field<&SCB::AIRCR, 0, 1>;
    using VECTCLRACTIVE = Field<&SCB::AIRCR, VECTRESET::End, 1>;
    using SYSRESETREQ   = Field<&SCB::AIRCR, VECTCLRACTIVE::End, 1>;
    using RES7          = Reserved<SYSRESETREQ::End, 5>;
    using PRIGROUP      = Field<&SCB::AIRCR, RES7::End, 3>;
    using RES8          = Reserved<PRIGROUP::End, 4>;
    using ENDIANESS     = Field<&SCB::AIRCR, RES8::End, 1>;
    using VECTKEY       = Field<&SCB::AIRCR, ENDIANESS::End, 16>;
    static_assert(VECTKEY::End == 32, "SCB::AIRCR layout does not cover exactly 32 bits");

    using RES9        = Reserved<0, 1>;
    using SLEEPONEXIT = Field<&SCB::SCR, RES9::End, 1>;
    using SLEEPDEEP   = Field<&SCB::SCR, SLEEPONEXIT::End, 1>;
    using RES10       = Reserved<SLEEPDEEP::End, 1>;
    using SEVONPEND   = Field<&SCB::SCR, RES10::End, 1>;
    using RES11       = Reserved<SEVONPEND::End, 27>;
    static_assert(RES11::End == 32, "SCB::SCR layout does not cover exactly 32 bits");

    using NONBASETHRDENA = Field<&SCB::CCR, 0, 1>;
    using USERSETMPEND   = Field<&SCB::CCR, NONBASETHRDENA::End, 1>;
    using RES12          = Reserved<USERSETMPEND::End, 1>;
    using UNALIGN_TRP    = Field<&SCB::CCR, RES12::End, 1>;
    using DIV_0_TRP      = Field<&SCB::CCR, UNALIGN_TRP::End, 1>;
    using RES13          = Reserved<DIV_0_TRP::End, 3>;
    using BFHFNMIGN      = Field<&SCB::CCR, RES13::End, 1>;
    using STKALIGN       = Field<&SCB::CCR, BFHFNMIGN::End, 1>;
    using RES14          = Reserved<STKALIGN::End, 22>;
    static_assert(RES14::End == 32, "SCB::CCR layout does not cover exactly 32 bits");

    using PRI_4 = Field<&SCB::SHPR1, 0, 8>;
    using PRI_5 = Field<&SCB::SHPR1, PRI_4::End, 8>;
    using PRI_6 = Field<&SCB::SHPR1, PRI_5::End, 8>;
    using RES15 = Reserved<PRI_6::End, 8>;
    static_assert(RES15::End == 32, "SCB::SHPR1 layout does not cover exactly 32 bits");

    using RES16  = Reserved<0, 24>;
    using PRI_11 = Field<&SCB::SHPR2, RES16::End, 8>;
    static_assert(PRI_11::End == 32, "SCB::SHPR2 layout does not cover exactly 32 bits");

    using RES17  = Reserved<0, 16>;
    using PRI_14 = Field<&SCB::SHPR3, RES17::End, 8>;
    using PRI_15 = Field<&SCB::SHPR3, PRI_14::End, 8>;
    static_assert(PRI_15::End == 32, "SCB::SHPR3 layout does not cover exactly 32 bits");

    using MEMFAULTACT    = Field<&SCB::SHCSR, 0, 1>;
    using BUSFAULTACT    = Field<&SCB::SHCSR, MEMFAULTACT::End, 1>;
    using RES18          = Reserved<BUSFAULTACT::End, 1>;
    using USGFAULTACT    = Field<&SCB::SHCSR, RES18::End, 1>;
    using RES19          = Reserved<USGFAULTACT::End, 3>;
    using SVCALLACT      = Field<&SCB::SHCSR, RES19::End, 1>;
    using MONITORACT     = Field<&SCB::SHCSR, SVCALLACT::End, 1>;
    using RES20          = Reserved<MONITORACT::End, 1>;
    using PENDSVACT      = Field<&SCB::SHCSR, RES20::End, 1>;
    using SYSTICKACT     = Field<&SCB::SHCSR, PENDSVACT::End, 1>;
    using USGFAULTPENDED = Field<&SCB::SHCSR, SYSTICKACT::End, 1>;
    using MEMFAULTPENDED = Field<&SCB::SHCSR, USGFAULTPENDED::End, 1>;
    using BUSFAULTPENDED = Field<&SCB::SHCSR, MEMFAULTPENDED::End, 1>;
    using SVCALLPENDED   = Field<&SCB::SHCSR, BUSFAULTPENDED::End, 1>;
    using MEMFAULTENA    = Field<&SCB::SHCSR, SVCALLPENDED::End, 1>;
    using BUSFAULTENA    = Field<&SCB::SHCSR, MEMFAULTENA::End, 1>;
    using USGFAULTENA    = Field<&SCB::SHCSR, BUSFAULTENA::End, 1>;
    using RES21          = Reserved<USGFAULTENA::End, 13>;
    static_assert(RES21::End == 32, "SCB::SHCSR layout does not cover exactly 32 bits");

    using IACCVIOL    = Field<&SCB::CFSR, 0, 1>;
    using DACCVIOL    = Field<&SCB::CFSR, IACCVIOL::End, 1>;
    using RES22       = Reserved<DACCVIOL::End, 1>;
    using MUNSTKERR   = Field<&SCB::CFSR, RES22::End, 1>;
    using MSTKERR     = Field<&SCB::CFSR, MUNSTKERR::End, 1>;
    using MLSPERR     = Field<&SCB::CFSR, MSTKERR::End, 1>;
    using RES23       = Reserved<MLSPERR::End, 1>;
    using MMARVALID   = Field<&SCB::CFSR, RES23::End, 1>;
    using IBUSERR     = Field<&SCB::CFSR, MMARVALID::End, 1>;
    using PRECISERR   = Field<&SCB::CFSR, IBUSERR::End, 1>;
    using IMPRECISERR = Field<&SCB::CFSR, PRECISERR::End, 1>;
    using UNSTKERR    = Field<&SCB::CFSR, IMPRECISERR::End, 1>;
    using STKERR      = Field<&SCB::CFSR, UNSTKERR::End, 1>;
    using LSPERR      = Field<&SCB::CFSR, STKERR::End, 1>;
    using RES24       = Reserved<LSPERR::End, 1>;
    using BFARVALID   = Field<&SCB::CFSR, RES24::End, 1>;
    using UNDEFINSTR  = Field<&SCB::CFSR, BFARVALID::End, 1>;
    using INVSTATE    = Field<&SCB::CFSR, UNDEFINSTR::End, 1>;
    using INVPC       = Field<&SCB::CFSR, INVSTATE::End, 1>;
    using NOCP        = Field<&SCB::CFSR, INVPC::End, 1>;
    using RES25       = Reserved<NOCP::End, 4>;
    using UNALIGNED   = Field<&SCB::CFSR, RES25::End, 1>;
    using DIVBYZERO   = Field<&SCB::CFSR, UNALIGNED::End, 1>;
    using RES26       = Reserved<DIVBYZERO::End, 6>;
    static_assert(RES26::End == 32, "SCB::CFSR layout does not cover exactly 32 bits");

    using RES27   = Reserved<0, 1>;
    using VECTTBL = Field<&SCB::HFSR, RES27::End, 1>;
    using RES28   = Reserved<VECTTBL::End, 28>;
    using FORCED  = Field<&SCB::HFSR, RES28::End, 1>;
    using DEBUGVT = Field<&SCB::HFSR, FORCED::End, 1>;
    static_assert(DEBUGVT::End == 32, "SCB::HFSR layout does not cover exactly 32 bits");

    using IMPDEF = Field<&SCB::AFSR, 0, 32>;
    static_assert(IMPDEF::End == 32, "SCB::AFSR layout does not cover exactly 32 bits");

    static SCB* get() { return reinterpret_cast<SCB*>(0xE000ED00); }

    class Interface
    {
        public:
            static void systemReset();
            static void sleep();
    };
};
