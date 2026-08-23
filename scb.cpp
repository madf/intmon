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
    groupWrite<VECTKEY, PRIGROUP, SYSRESETREQ>(0x05FAUL, 0x07UL, true);
    __DSB();
    while (true)
        __NOP();
}

void Interface::sleep()
{
    __asm volatile ("wfi");
}
