#include "scb.h"

using Interface = SCB::Interface;

namespace
{

void __NOP()
{
    __asm volatile ("nop");
}

void __DSB()
{
    __asm volatile ("dsb 0xF":::"memory");
}

}

void Interface::systemReset()
{
    __DSB();
    SCB::Regs->AIRCR = (0x5FAUL << 16) | (SCB::Regs->AIRCR & (0x7UL << 8)) | 0x01UL << 2;
    __DSB();
    while (true)
        __NOP();
}

void Interface::sleep()
{
    __asm volatile ("wfi");
}
