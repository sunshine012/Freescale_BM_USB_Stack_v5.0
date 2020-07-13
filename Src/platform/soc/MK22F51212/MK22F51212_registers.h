/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __MK22F51212_REGISTERS_H__
#define __MK22F51212_REGISTERS_H__

/*
 * Include the cpu specific register header files.
 *
 * The CPU macro should be declared in the project or makefile.
 */
#if (defined(CPU_MK22F51212)) || defined(CPU_MK22F12812)
    #define K22F51212_SERIES
    /* Extension register headers. (These will eventually be merged into the CMSIS-style header.)*/
    #include "MK22F51212/MK22F51212_gpio.h"
    #include "MK22F51212/MK22F51212_mcg.h"
    #include "MK22F51212/MK22F51212_pit.h"
    #include "MK22F51212/MK22F51212_port.h"
    #include "MK22F51212/MK22F51212_sim.h"
    #include "MK22F51212/MK22F51212_uart.h"
    #include "MK22F51212/MK22F51212_usb.h"
    #include "MK22F51212/MK22F51212_wdog.h"
    #include "MK22F51212/MK22F51212_smc.h"
    #include "MK22F51212/MK22F51212_osc.h"
    #include "MK22F51212/MK22F51212_i2c.h"
    #include "MK22F51212/MK22F51212_i2s.h"

    /* CMSIS-style register definitions*/
    #include "MK22F51212/MK22F51212.h"
#else
    #error "No valid CPU defined!"
#endif

#endif /* __MK22F51212_REGISTERS_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

