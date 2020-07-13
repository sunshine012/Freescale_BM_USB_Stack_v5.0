/**HEADER********************************************************************
* 
* Copyright (c) 2013 - 2014 Freescale Semiconductor;
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
* Comments:  
*
*END************************************************************************/
#include "adapter.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)||(OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX))
#include "MK64F12.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define USB_CLK_RECOVER_IRC_EN (*(volatile unsigned char *)0x40072144)
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#define BSPCFG_USB_USE_IRC48M            (0)
static int32_t bsp_usb_host_io_init
(
    int32_t i
)
{
    if (i == 0)
    {
#if BSPCFG_USB_USE_IRC48M

    /*
    * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
    */
    SIM_CLKDIV2 = (uint32_t)0x0UL; /* Update USB clock prescalers */

    /* Configure USB to be clocked from IRC 48MHz */
    SIM_SOPT2_REG(SIM_BASE_PTR)  |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;
    /* Enable IRC 48MHz for USB module */
    USB_CLK_RECOVER_IRC_EN = 0x03;
#else

  /* Configure USB to be clocked from PLL0 */
    HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK );
    BW_SIM_SOPT2_PLLFLLSEL(1);
    /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
    BW_SIM_CLKDIV2_USBFRAC(2 - 1);
    BW_SIM_CLKDIV2_USBDIV(5 - 1);
    /* Enable USB-OTG IP clocking */
    HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
#endif
    }
    else
    {
        return -1; //unknow controller
    }

    return 0;
    
}


int32_t bsp_usb_host_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_host_io_init(controller_id);
    if (result != 0)
    {
        return result;
    }
    /* MPU is disabled. All accesses from all bus masters are allowed */
    MPU_CESR=0;
    if (0 == controller_id)
    {
        /* Do not configure enable USB regulator for host */
        /* reset USB CTRL register */
        USB_USBCTRL_REG(USB0_BASE_PTR) = 0;

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}
#endif
/* EOF */
