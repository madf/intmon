#pragma once

#include <cstdint>

namespace RCC
{

struct Type
{
    volatile uint32_t CR;           // Clock control
    volatile uint32_t PLLCFGR;      // PLL configuration
    volatile uint32_t CFGR;         // Clock configuration
    volatile uint32_t CIR;          // Clock interrupt
    volatile uint32_t AHB1RSTR;     // AHB1 peripheral reset
    volatile uint32_t AHB2RSTR;     // AHB2 peripheral reset
    volatile uint32_t AHB3RSTR;     // AHB3 peripheral reset
    volatile uint32_t RESERVED0;
    volatile uint32_t APB1RSTR;     // APB1 peripheral reset
    volatile uint32_t APB2RSTR;     // APB2 peripheral reset
    volatile uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;      // AHB1 peripheral clock enable
    volatile uint32_t AHB2ENR;      // AHB2 peripheral clock enable
    volatile uint32_t AHB3ENR;      // AHB3 peripheral clock enable
    volatile uint32_t RESERVED2;
    volatile uint32_t APB1ENR;      // APB1 peripheral clock enable
    volatile uint32_t APB2ENR;      // APB2 peripheral clock enable
    volatile uint32_t RESERVED3[2];
    volatile uint32_t AHB1LPENR;    // AHB1 peripheral clock enable in low power mode
    volatile uint32_t AHB2LPENR;    // AHB2 peripheral clock enable in low power mode
    volatile uint32_t AHB3LPENR;    // AHB3 peripheral clock enable in low power mode
    volatile uint32_t RESERVED4;
    volatile uint32_t APB1LPENR;    // APB1 peripheral clock enable in low power mode
    volatile uint32_t APB2LPENR;    // APB2 peripheral clock enable in low power mode
    volatile uint32_t RESERVED5[2];
    volatile uint32_t BDCR;         // Backup domain control
    volatile uint32_t CSR;          // Clock control & status
    volatile uint32_t RESERVED6[2];
    volatile uint32_t SSCGR;        // Spread spectrum clock generation
    volatile uint32_t PLLI2SCFGR;   // PLLI2S configuration
    volatile uint32_t PLLSAICFGR;   // PLLSAI configuration
    volatile uint32_t DCKCFGR;      // Dedicated clocks configuration
};

inline Type* const Regs = reinterpret_cast<Type*>(0x40023800);

}
