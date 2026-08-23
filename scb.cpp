#include "scb.h"

#include "asm.h"

using Interface = SCB::Interface;

void Interface::systemReset()
{
    ASM_DSB();
    groupWrite<VECTKEY, PRIGROUP, SYSRESETREQ>(0x05FAUL, 0x07UL, true);
    ASM_DSB();
    while (true)
        ASM_NOP();
}

void Interface::sleep()
{
    ASM_WFI();
}
