//*****************************************************************************
//
//! @file system_apollo1.h
//!
//! @brief Ambiq Micro Apollo1 MCU specific functions.
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
//*****************************************************************************

#ifndef SYSTEM_APOLLO1_H
#define SYSTEM_APOLLO1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint32_t SystemCoreClock;     // System Clock Frequency (Core Clock)

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void SystemInit (void);
extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif  // SYSTEM_APOLLO1_H

