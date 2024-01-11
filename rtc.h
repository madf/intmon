#pragma once

#include <array>
#include <cstdint>

namespace RTC
{

struct Type
{
    volatile uint32_t TR;                   // Time
    volatile uint32_t DR;                   // Date
    volatile uint32_t CR;                   // Control
    volatile uint32_t ISR;                  // Initialization & status
    volatile uint32_t PRER;                 // Prescaler
    volatile uint32_t WUTR;                 // Wakeup timer
    volatile uint32_t CALIBR;               // Calibration
    volatile uint32_t ALRMAR;               // Alarm A
    volatile uint32_t ALRMBR;               // Alarm B
    volatile uint32_t WPR;                  // Write protection
    volatile uint32_t SSR;                  // Subsecond
    volatile uint32_t SHIFTR;               // Shift control
    volatile uint32_t TSTR;                 // Time stamp time
    volatile uint32_t TSDR;                 // Time stamp date
    volatile uint32_t TSSSR;                // Time stamp subsecond
    volatile uint32_t CALR;                 // Calibration
    volatile uint32_t TAFCR;                // Tamper and alternate function configuration
    volatile uint32_t ALRMASSR;             // Alarm A subsecond
    volatile uint32_t ALRMBSSR;             // Alarm B subsecond
    volatile std::array<uint32_t, 20> BKPR; // Backup 0-19
};

inline Type* const Regs = reinterpret_cast<Type*>(0x40002800);

}
