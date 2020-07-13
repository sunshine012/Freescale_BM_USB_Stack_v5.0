/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
 **************************************************************************//*!
 *
 * @file wdt_kinetis.c
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief   This file contains the implementation of the Watchdog service routines on CFV2
 *****************************************************************************/
 
#include "types.h"          /* User Defined Data Types */
#include "soc.h"
#include "derivative.h"     /* include peripheral declarations */
#include "wdt_kinetis.h"       /* own header with public declarations */

/*****************************************************************************/
void Watchdog_Reset(void)
{
#ifdef MCU_MKL25Z4
    //(void)(RCM_SRS0 |= RCM_SRS0_WDOG_MASK);
#elif (defined MCU_MKL26Z4)
    //(void)(RCM_SRS0 |= RCM_SRS0_WDOG_MASK);
#elif (defined CPU_MK22F51212)
    HW_WDOG_REFRESH_WR(0xA602);
    HW_WDOG_REFRESH_WR(0xB480);
#else
    (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480);
#endif
}