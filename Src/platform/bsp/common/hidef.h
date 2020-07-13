/**HEADER********************************************************************
*
* Copyright (c) 2012, 2013 Freescale Semiconductor;
* All Rights Reserved
*
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName:hidef.h
* $Version :
* $Date    :
*
* Comments:
* 
*
*END************************************************************************/
#ifndef _H_HIDEF_
#define _H_HIDEF_
#include "derivative.h"
#ifdef __cplusplus
    extern "C" {
#endif

#if defined(__CWCC__)
    #pragma gcc_extensions on
#endif
    
#if (defined(__CWCC__)||defined(__GNUC__))    
    #include <stddef.h>
    #if (defined __MK_xxx_H__)
    #if (defined MCU_MKL25Z4) || (defined MCU_MKL46Z4)
        #define __RESET_WATCHDOG()  (void)(RCM_SRS0 |= RCM_SRS0_WDOG_MASK)
    #else
        #define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
        #define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
    #endif
        
        /*!< Macro to enable all interrupts. */
        #define EnableInterrupts  asm ("CPSIE i");
        /*!< Macro to disable all interrupts. */
        #define DisableInterrupts asm ("CPSID i");
    #else
        /**** Version for ColFire V1 */    
        /*!< Macro to enable all interrupts. */
        #define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }
        /*!< Macro to disable all interrupts. */
        #define DisableInterrupts asm { move.w SR,D0; ori.l #0x0700,D0; move.w D0,SR;  }
        
    #endif
#elif defined(__IAR_SYSTEMS_ICC__)
    #include <intrinsics.h>
    #if (defined MCU_MKL25Z4) || (defined MCU_MKL46Z4)
        #define __RESET_WATCHDOG()  (void)(RCM_SRS0 |= RCM_SRS0_WDOG_MASK)
    #else
    #define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
    #define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
    #endif
    /*!< Macro to enable all interrupts. */
    #define EnableInterrupts  __enable_interrupt();
    /*!< Macro to disable all interrupts. */
    #define DisableInterrupts __disable_interrupt();
#elif defined(__CC_ARM)
    #include <stddef.h>
    
    #define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
    #define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
    
    /*!< Macro to enable all interrupts. */
    #define EnableInterrupts  __enable_irq();
    /*!< Macro to disable all interrupts. */
    #define DisableInterrupts __disable_irq();
#endif
    
#ifdef __cplusplus
    }
#endif

#endif

/*****************************************************/
/* end hidef.h */
