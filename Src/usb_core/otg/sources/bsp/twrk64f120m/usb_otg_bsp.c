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
* $FileName: usb_otg_bsp.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*         
*****************************************************************************/
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
#include "MK64F12/MK64F12_gpio.h"
#include "MK64F12/MK64F12_port.h"
#include "MK64F12/MK64F12_mpu.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK64F12.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "usb_otg_khci_max3353_prv.h"
#define BSP_USB_INT_LEVEL                (4)
#define BSPCFG_USB_USE_IRC48M            (0)
#define BSP_USB_OTG_MAX3353_INT_LEVEL    (4)
/* TODO: Move this structure to other place */
/* struct contains max3353 init params */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#define KHCI_BASE_PTR                USB0_BASE_PTR      /* KHCI_BASE_PTR */
#define KHCI_VECTOR                  INT_USB0           /* KHCI_VECTOR */
#define MAX_3353_INT_PORT            PORTD_BASE_PTR     /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               INT_PORTD          /* MAX3353_VECTOR */
#define I2C_CHANNEL                  "i2c0:"            /* I2C Channel */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#define KHCI_BASE_PTR                USB0_BASE          /* KHCI_BASE_PTR */
#define KHCI_VECTOR                  (USB0_IRQn + 16)   /* KHCI_VECTOR */
#define MAX_3353_INT_PORT            HW_PORTD           /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               (PORTD_IRQn + 16)  /* MAX3353_VECTOR */
#define I2C_CHANNEL                  0                  /* I2C Channel */
#endif
#define MAX3353_INT_PIN              14 /*MAX3353_INT_PIN*/
static const struct usb_khci_max3353_otg_init_struct g_khci0_max3353_otg_init_param = {
    {
        (void*)KHCI_BASE_PTR,
        KHCI_VECTOR,
        BSP_USB_INT_LEVEL ,
    },
    {
        (void*)MAX_3353_INT_PORT,
        MAX3353_INT_PIN,
        MAX3353_VECTOR,
        BSP_USB_OTG_MAX3353_INT_LEVEL,
        I2C_CHANNEL,
        0x2C
    }
};
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_otg_max3353_pin_int_clear
* Returned Value   : none
* Comments         :
*    This function clears the pin interrupt flag associated with the max3353 interrupt pin 
*
*END*----------------------------------------------------------------------*/
void _bsp_usb_otg_max3353_clear_pin_int_flag()
{
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (PORT_ISFR_REG(MAX_3353_INT_PORT) & (1<<MAX3353_INT_PIN))
    {
        PORT_ISFR_REG(MAX_3353_INT_PORT) |= 1<<MAX3353_INT_PIN;
    }
#else
     if (HW_PORT_ISFR_RD(MAX_3353_INT_PORT) & (1<<MAX3353_INT_PIN))
    {
        HW_PORT_ISFR_SET(MAX_3353_INT_PORT,1<<MAX3353_INT_PIN);
    }
#endif
}    

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_otg_max3353_set_pin_int
* Returned Value   : none
* Comments         :
*    This function enables/disables the pin interrupt associated with the max3353 interrupt pin 
*
*END*----------------------------------------------------------------------*/
void _bsp_usb_otg_max3353_set_pin_int
(
    bool level,
    bool enable
)
{
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    if (enable)
    {
        if (level)/* interrupt is triggered  by low level */  
        {
            HW_PORT_PCRn_WR(MAX_3353_INT_PORT,MAX3353_INT_PIN,0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x08));    
        }
        else/* interrupt is triggered by falling edge */
        {
            HW_PORT_PCRn_WR(MAX_3353_INT_PORT,MAX3353_INT_PIN,0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0A));
        }
    }
    else
    {
        HW_PORT_PCRn_WR(MAX_3353_INT_PORT,MAX3353_INT_PIN,0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x00));    
    }
#else
    if (enable)
    {
        if (level)/* interrupt is triggered  by low level */  
        {
            PORT_PCR_REG(MAX_3353_INT_PORT,MAX3353_INT_PIN) |=  0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x08);
        }
        else/* interrupt is triggered by falling edge */
        {
            PORT_PCR_REG(MAX_3353_INT_PORT,MAX3353_INT_PIN) |=  0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0A);
        }
    }
    else
    {
       PORT_PCR_REG(MAX_3353_INT_PORT,MAX3353_INT_PIN) |=  0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x00);
    }
#endif
}

void* bsp_usb_otg_get_max3353_init_param
(
    uint8_t controller_id
)
{
    if (controller_id ==0)
    {
        return (void*)(&g_khci0_max3353_otg_init_param);
    }
    else
    {
        return NULL;
    }
}

static int32_t bsp_usb_otg_io_init
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
        /* Configure USBFRAC = 0, USBDIV = 0 => frq(USBout) = 1 / 1 * frq(PLLin) */
        /* Configure USB to be clocked from PLL */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK );
        BW_SIM_SOPT2_PLLFLLSEL(1);
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        BW_SIM_CLKDIV2_USBFRAC(2 - 1);
        BW_SIM_CLKDIV2_USBDIV(5 - 1);
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
#endif
        HW_SIM_SCGC5_SET(SIM_SCGC5_PORTD_MASK);
#endif
    }
    else
    {
        return -1; //unknow controller
    }

    return 0;
}

int32_t bsp_usb_otg_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_otg_io_init(controller_id);

    if (result != 0)
    {
        return result;
    }
    if (0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        /* MPU is disabled. All accesses from all bus masters are allowed */
        HW_MPU_CESR_WR(0);
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);

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
