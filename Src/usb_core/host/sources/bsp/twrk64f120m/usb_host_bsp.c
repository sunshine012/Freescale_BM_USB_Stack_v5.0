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
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK64F12))
#include "MK64F12/MK64F12_sim.h"
#include "MK64F12/MK64F12_usb.h"
//#include "MK64F12/MK64F12_gpio.h"
//#include "MK64F12/MK64F12_port.h"
#include "MK64F12/MK64F12_mpu.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK64F12.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define BSPCFG_USB_USE_IRC48M                    (0)
static int32_t bsp_usb_host_io_init
(
    int32_t i
)
{
    if (i == 0)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#if BSPCFG_USB_USE_IRC48M

    /*
    * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
    */
    BW_SIM_CLKDIV2_USBFRAC(0);
    BW_SIM_CLKDIV2_USBDIV(0);
    /* Configure USB to be clocked from IRC 48MHz */
    HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK);
    BW_SIM_SOPT2_PLLFLLSEL(3);
    /* Enable USB-OTG IP clocking */
    HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
    /* Enable IRC 48MHz for USB module */
    HW_USB_CLK_RECOVER_IRC_EN_SET(USB_CLK_RECOVER_IRC_EN_IRC_EN_MASK);
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

	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
	HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
	HW_GPIO_PSOR_WR(2, HW_GPIO_PSOR(2).U | 1<<9);       /* Output high */
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
    if (0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        /* MPU is disabled. All accesses from all bus masters are allowed */
        HW_MPU_CESR_WR(0);
        /* Do not configure enable USB regulator for host */
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#endif
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
