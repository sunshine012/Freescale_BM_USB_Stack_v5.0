/*
** ###################################################################
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    K22P144M100SF5RM, Rev.1, Nov 2012
**     Version:             rev. 1.2, 2012-12-04
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2012, 2013 Freescale, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-06-06)
**         Initial version.
**     - rev. 1.1 (2012-11-12)
**         Update according to reference manual rev.1, draft B
**     - rev. 1.2 (2012-12-04)
**         Update according to reference manual rev.1
**
** ###################################################################
*/

/**
 * @file MKL26Z
 * @version 1.2
 * @date 2012-12-04
 * @brief Device specific configuration file for MKL26Z (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "derivative.h"

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) 
{
    HW_SIM_COPC_WR(0x00);
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {}
