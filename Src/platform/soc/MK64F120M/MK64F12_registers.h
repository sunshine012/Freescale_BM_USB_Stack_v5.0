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
#ifndef __MK64F12_REGISTERS_H__
#define __MK64F12_REGISTERS_H__

/*
 * Include the cpu specific register header files.
 *
 * The CPU macro should be declared in the project or makefile.
 */
#if (defined(CPU_MK64F12))
    /* Extension register headers. (These will eventually be merged into the CMSIS-style header.)*/
    #include "MK64F12/MK64F12_gpio.h"
    #include "MK64F12/MK64F12_mcg.h"
    #include "MK64F12/MK64F12_pit.h"
    #include "MK64F12/MK64F12_port.h"
    #include "MK64F12/MK64F12_sim.h"
    #include "MK64F12/MK64F12_uart.h"
    #include "MK64F12/MK64F12_usb.h"
    #include "MK64F12/MK64F12_wdog.h"
    #include "MK64F12/MK64F12_i2c.h"
    #include "MK64F12/MK64F12_i2s.h"

    /* CMSIS-style register definitions*/
    #include "MK64F12/MK64F12.h"
#else
    #error "No valid CPU defined!"
#endif

#endif /* __MK64F12_REGISTERS_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

