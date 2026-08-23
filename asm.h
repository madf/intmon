#pragma once

inline
void ASM_NOP()
{
    __asm volatile ("nop");
}

inline
void ASM_DSB()
{
    __asm volatile ("dsb 0xF":::"memory");
}

inline
void ASM_WFI()
{
    __asm volatile ("wfi");
}

inline
void ASM_WFE()
{
    __asm volatile ("wfe");
}
