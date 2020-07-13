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
#include "usb_misc.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    #if (defined(CPU_MK70F12))
#include "MK70FN1M012/MK70F12_sim.h"
#include "MK70FN1M012/MK70F12_usb.h"
#include "MK70FN1M012/MK70F12_usbhs.h"
#include "MK70FN1M012/MK70F12_gpio.h"
#include "MK70FN1M012/MK70F12_port.h"
#include "MK70FN1M012/MK70F12_mpu.h"
    #endif
#endif

#ifdef BSP_USB_TWR_SER2
#undef BSP_USB_TWR_SER2
#endif
#define BSP_USB_TWR_SER2              0
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#define BSP_USB_INT_LEVEL             (4)

#define USBHS_USBMODE_CM_IDLE_MASK    USBHS_USBMODE_CM(0)
#define USBHS_USBMODE_CM_DEVICE_MASK  USBHS_USBMODE_CM(2)             
#define USBHS_USBMODE_CM_HOST_MASK    USBHS_USBMODE_CM(3)

static int32_t bsp_usb_dev_io_init
(
   int32_t i
)
{
    /* Disable MPU so the module can access RAM */
    MPU_CESR &= ~MPU_CESR_VLD_MASK;
    if (i == USB_CONTROLLER_KHCI_0)
    {
        /* Configure USB to be clocked from PLL0 */
        SIM_SOPT2_REG(SIM_BASE_PTR) &= ~(SIM_SOPT2_USBFSRC_MASK);
        SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBFSRC(1);
        /* Configure USB to be clocked from clock divider */
        SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBF_CLKSEL_MASK;
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        SIM_CLKDIV2_REG(SIM_BASE_PTR) &= ~(SIM_CLKDIV2_USBFSDIV_MASK | SIM_CLKDIV2_USBFSFRAC_MASK);
        SIM_CLKDIV2_REG(SIM_BASE_PTR) |= SIM_CLKDIV2_USBFSDIV(4) | SIM_CLKDIV2_USBFSFRAC_MASK;
    
        /* Enable USB-OTG IP clocking */
        SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBFS_MASK;
    
        /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
        /* VREFIN for device is standalone not multiplexed one-purpose pin */

#if BSP_USB_TWR_SER2
        /* TWR-SER2 board has 2 connectors: on channel A, there is Micro-USB connector,
        ** which is not routed to TWRK60 board. On channel B, there is standard
        ** A-type host connector routed to the USB0 peripheral on TWRK60. To enable
        ** power to this connector, GPIO PB8 must be set as GPIO output
        */
        PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
        GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
        GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
#endif
    }
    else if (i == USB_CONTROLLER_EHCI_0)
    {   
        //Enable clock to the module
        SIM_SCGC6 |= SIM_SCGC6_USBHS_MASK;

        SIM_CLKDIV2 |= SIM_CLKDIV2_USBHSFRAC_MASK | SIM_CLKDIV2_USBHSDIV_MASK; // Divide reference clock to obtain 60MHz 
        SIM_SOPT2 &= ~SIM_SOPT2_USBHSRC_MASK;  // MCGPLLCLK for the USB 60MHz CLKC source 
        SIM_SOPT2 |= SIM_SOPT2_USBHSRC(1);     // MCGPLLCLK for the USB 60MHz CLKC source 

        //Select external clock for USBH controller
        PORTA_PCR7 = PORT_PCR_MUX(2);   //ULPI DIR
        PORTA_PCR8 = PORT_PCR_MUX(2);   //ULPI NXT
        PORTA_PCR10 = PORT_PCR_MUX(2);  //ULPI DATA0
        PORTA_PCR11 = PORT_PCR_MUX(2);  //ULPI DATA1
        PORTA_PCR24 = PORT_PCR_MUX(2);  //ULPI DATA2
        PORTA_PCR25 = PORT_PCR_MUX(2);  //ULPI DATA3
        PORTA_PCR26 = PORT_PCR_MUX(2);  //ULPI DATA4
        PORTA_PCR27 = PORT_PCR_MUX(2);  //ULPI DATA5
        PORTA_PCR28 = PORT_PCR_MUX(2);  //ULPI DATA6
        PORTA_PCR29 = PORT_PCR_MUX(2);  //ULPI DATA7
        PORTA_PCR6 = PORT_PCR_MUX(2);   //ULPI CLK
        PORTA_PCR9 = PORT_PCR_MUX(2);   //ULPI STP
        
        /*
         *If we can't control when to provide the power to ULPI PHY and when to enable the
         * ULPI PHY clock a PHY RESET should be done after ULPI PHY initialization to make the
         * PHY recover from the stall state.
         */
        PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
        GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
        GPIO_PDOR_REG(PTB_BASE_PTR) &= ~(1 << 8); // PB8 in low level
        OS_Time_delay(1);
        GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
    }
    else
    {
        return -1; //unknow controller
    }

    return 0;
    
}

int32_t bsp_usb_dev_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_dev_io_init(controller_id);
    if (result != 0)
        return result;

    /* MPU is disabled. All accesses from all bus masters are allowed */
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Configure enable USB regulator for device */
        SIM_SOPT1CFG_REG(SIM_BASE_PTR) |= SIM_SOPT1CFG_URWE_MASK;
        SIM_SOPT1_REG(SIM_BASE_PTR) |= SIM_SOPT1_USBREGEN_MASK;

        /* reset USB CTRL register */
        USB_USBCTRL_REG(USB0_BASE_PTR) = 0;

        /* Enable internal pull-up resistor */
        USB_CONTROL_REG(USB0_BASE_PTR) = USB_CONTROL_DPPULLUPNONOTG_MASK;
        USB_USBTRC0_REG(USB0_BASE_PTR) |= 0x40; /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init(INT_USB0, BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else if (USB_CONTROLLER_EHCI_0 == controller_id)
    {
        USBHS_USBCMD |= USBHS_USBCMD_RST_MASK;    
        while (USBHS_USBCMD & USBHS_USBCMD_RST_MASK)
        { /* delay while resetting USB controller */ } 

        USBHS_USBMODE = USBHS_USBMODE_CM_DEVICE_MASK;
        /* Set interrupt threshold control = 0 */
        USBHS_USBCMD &= ~( USBHS_USBCMD_ITC(0xFF));
        /* Setup Lockouts Off */
        USBHS_USBMODE |= USBHS_USBMODE_SLOM_MASK;

        /* setup interrupt */
        OS_intr_init(INT_USBHS, BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#define BSP_USB_INT_LEVEL             (4)

#define USBHS_USBMODE_CM_IDLE_MASK    USBHS_USBMODE_CM(0)
#define USBHS_USBMODE_CM_DEVICE_MASK  USBHS_USBMODE_CM(2)             
#define USBHS_USBMODE_CM_HOST_MASK    USBHS_USBMODE_CM(3)

static int32_t bsp_usb_dev_io_init
(
   int32_t i
)
{
    /* Disable MPU so the module can access RAM */
    HW_MPU_CESR_CLR(MPU_CESR_VLD_MASK);
    if (i == USB_CONTROLLER_KHCI_0)
    {
        /* Configure USB to be clocked from PLL0 */
        HW_SIM_SOPT2_CLR(SIM_SOPT2_USBFSRC_MASK);
        BW_SIM_SOPT2_USBFSRC(1);
        /* Configure USB to be clocked from clock divider */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBF_CLKSEL_MASK);
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        HW_SIM_SOPT2_CLR(SIM_CLKDIV2_USBFSDIV_MASK | SIM_CLKDIV2_USBFSFRAC_MASK);
        BW_SIM_CLKDIV2_USBFSDIV(5-1);
        BW_SIM_CLKDIV2_USBFSFRAC(2-1);
    
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBFS_MASK);
    
        /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
        /* VREFIN for device is standalone not multiplexed one-purpose pin */

#if BSP_USB_TWR_SER2
        /* TWR-SER2 board has 2 connectors: on channel A, there is Micro-USB connector,
        ** which is not routed to TWRK60 board. On channel B, there is standard
        ** A-type host connector routed to the USB0 peripheral on TWRK60. To enable
        ** power to this connector, GPIO PB8 must be set as GPIO output
        */
        BW_PORT_PCRn_MUX(HW_GPIOB, 8, 1);
        HW_GPIO_PDDR_SET(HW_PORTB,1 << 8);
        HW_GPIO_PDOR_SET(HW_PORTB,1 << 8);
#endif
    }
    else if (i == USB_CONTROLLER_EHCI_0)
    {   
        //Enable clock to the module
        HW_SIM_SCGC6_SET(SIM_SCGC6_USBHS_MASK);
        // Divide reference clock to obtain 60MHz 
        BW_SIM_CLKDIV2_USBHSFRAC(2-1);
        BW_SIM_CLKDIV2_USBHSDIV(2-1);
        HW_SIM_SOPT2_CLR(SIM_SOPT2_USBHSRC_MASK);
        BW_SIM_SOPT2_USBHSRC(1);

        //Select external clock for USBH controller
        HW_SIM_SCGC5_SET(SIM_SCGC5_PORTA_MASK);
        BW_PORT_PCRn_MUX(HW_PORTA, 7, 2);   //ULPI DIR
        BW_PORT_PCRn_MUX(HW_PORTA, 8, 2);   //ULPI NXT
        BW_PORT_PCRn_MUX(HW_PORTA, 10, 2);  //ULPI DATA0
        BW_PORT_PCRn_MUX(HW_PORTA, 11, 2);  //ULPI DATA1
        BW_PORT_PCRn_MUX(HW_PORTA, 24, 2);  //ULPI DATA2
        BW_PORT_PCRn_MUX(HW_PORTA, 25, 2);  //ULPI DATA3
        BW_PORT_PCRn_MUX(HW_PORTA, 26, 2);  //ULPI DATA4
        BW_PORT_PCRn_MUX(HW_PORTA, 27, 2);  //ULPI DATA5
        BW_PORT_PCRn_MUX(HW_PORTA, 28, 2);  //ULPI DATA6
        BW_PORT_PCRn_MUX(HW_PORTA, 29, 2);  //ULPI DATA7
        BW_PORT_PCRn_MUX(HW_PORTA, 6, 2);   //ULPI CLK
        BW_PORT_PCRn_MUX(HW_PORTA, 9, 2);   //ULPI STP
        
        /*
         *If we can't control when to provide the power to ULPI PHY and when to enable the
         * ULPI PHY clock a PHY RESET should be done after ULPI PHY initialization to make the
         * PHY recover from the stall state.
         */
        HW_SIM_SCGC5_SET(SIM_SCGC5_PORTB_MASK);
        BW_PORT_PCRn_MUX(HW_PORTB, 8, 1);
         // PB8 as output
        HW_GPIO_PDDR_SET(HW_PORTB,1 << 8);
        // PB8 in low level
        HW_GPIO_PDOR_CLR(HW_PORTB,1 << 8);
        OS_Time_delay(1);
        // PB8 in high level
        HW_GPIO_PDOR_SET(HW_PORTB,1 << 8);
    }
    else
    {
        return -1; //unknow controller
    }

    return 0;
    
}

int32_t bsp_usb_dev_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_dev_io_init(controller_id);
    if (result != 0)
        return result;

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB_CONTROL_DPPULLUPNONOTG_MASK);
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else if (USB_CONTROLLER_EHCI_0 == controller_id)
    {
        USBHS_USBCMD |= USBHS_USBCMD_RST_MASK;    
        while (BR_USBHS_USBCMD_RST)
        { /* delay while resetting USB controller */ } 

        HW_USBHS_USBMODE_WR(USBHS_USBMODE_CM_DEVICE_MASK);
        /* Set interrupt threshold control = 0 */
        BW_USBHS_USBCMD_ITC(0);
        /* Setup Lockouts Off */
        HW_USBHS_USBMODE_CLR(USBHS_USBMODE_SLOM_MASK);

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)

#define USBHS_USBMODE_CM_IDLE_MASK    USBHS_USBMODE_CM(0)
#define USBHS_USBMODE_CM_DEVICE_MASK  USBHS_USBMODE_CM(2)             
#define USBHS_USBMODE_CM_HOST_MASK    USBHS_USBMODE_CM(3)

static int32_t bsp_usb_dev_io_init
(
   int32_t i
)
{
    /* Disable MPU so the module can access RAM */
    HW_MPU_CESR_CLR(MPU_CESR_VLD_MASK);

    if ( i == USB_CONTROLLER_KHCI_0)
    {
        /* Configure USB to be clocked from PLL0 */
        HW_SIM_SOPT2_CLR(SIM_SOPT2_USBFSRC_MASK);
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBFSRC(1));

        /* Configure USB to be clocked from clock divider */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBF_CLKSEL_MASK);

        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFSDIV_MASK | SIM_CLKDIV2_USBFSFRAC_MASK);
        HW_SIM_CLKDIV2_SET(SIM_CLKDIV2_USBFSDIV(4) | SIM_CLKDIV2_USBFSFRAC_MASK);
        
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBFS_MASK);
#if 0   
        /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
        /* VREFIN for device is standalone not multiplexed one-purpose pin */

#if BSP_USB_TWR_SER2
        /* TWR-SER2 board has 2 connectors: on channel A, there is Micro-USB connector,
        ** which is not routed to TWRK60 board. On channel B, there is standard
        ** A-type host connector routed to the USB0 peripheral on TWRK60. To enable
        ** power to this connector, GPIO PB8 must be set as GPIO output
        */
        PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
        GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
        GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
#endif
#endif
    }
#if 0
    else if (i == USB_CONTROLLER_EHCI_0)
    {   
        //Enable clock to the module
        SIM_SCGC6 |= SIM_SCGC6_USBHS_MASK;

        SIM_CLKDIV2 |= SIM_CLKDIV2_USBHSFRAC_MASK | SIM_CLKDIV2_USBHSDIV_MASK; // Divide reference clock to obtain 60MHz 
        SIM_SOPT2 &= ~SIM_SOPT2_USBHSRC_MASK;  // MCGPLLCLK for the USB 60MHz CLKC source 
        SIM_SOPT2 |= SIM_SOPT2_USBHSRC(1);     // MCGPLLCLK for the USB 60MHz CLKC source 

        //Select external clock for USBH controller
        PORTA_PCR7 = PORT_PCR_MUX(2);   //ULPI DIR
        PORTA_PCR8 = PORT_PCR_MUX(2);   //ULPI NXT
        PORTA_PCR10 = PORT_PCR_MUX(2);  //ULPI DATA0
        PORTA_PCR11 = PORT_PCR_MUX(2);  //ULPI DATA1
        PORTA_PCR24 = PORT_PCR_MUX(2);  //ULPI DATA2
        PORTA_PCR25 = PORT_PCR_MUX(2);  //ULPI DATA3
        PORTA_PCR26 = PORT_PCR_MUX(2);  //ULPI DATA4
        PORTA_PCR27 = PORT_PCR_MUX(2);  //ULPI DATA5
        PORTA_PCR28 = PORT_PCR_MUX(2);  //ULPI DATA6
        PORTA_PCR29 = PORT_PCR_MUX(2);  //ULPI DATA7
        PORTA_PCR6 = PORT_PCR_MUX(2);   //ULPI CLK
        PORTA_PCR9 = PORT_PCR_MUX(2);   //ULPI STP
        
        /*
         *If we can't control when to provide the power to ULPI PHY and when to enable the
         * ULPI PHY clock a PHY RESET should be done after ULPI PHY initialization to make the
         * PHY recover from the stall state.
         */
        PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
        GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
        GPIO_PDOR_REG(PTB_BASE_PTR) &= ~(1 << 8); // PB8 in low level
        OS_Time_delay(1);
        GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
    }
#endif
    else
    {
        /* unknow controller */
        return -1; 
    }

    return 0;
    
}

int32_t bsp_usb_dev_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_dev_io_init(controller_id);
    if (result != 0)
        return result;

    /* MPU is disabled. All accesses from all bus masters are allowed */
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        USB0->USBCTRL = 0;

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(0x40);
        /* setup interrupt */
        OS_intr_init(USB0_IRQn, BSP_USB_INT_LEVEL, 0, TRUE);
    }
#if 0
    else if (USB_CONTROLLER_EHCI_0 == controller_id)
    {
        USBHS_USBCMD |= USBHS_USBCMD_RST_MASK;    
        while (USBHS_USBCMD & USBHS_USBCMD_RST_MASK)
        { /* delay while resetting USB controller */ } 

        USBHS_USBMODE = USBHS_USBMODE_CM_DEVICE_MASK;
        /* Set interrupt threshold control = 0 */
        USBHS_USBCMD &= ~( USBHS_USBCMD_ITC(0xFF));
        /* Setup Lockouts Off */
        USBHS_USBMODE |= USBHS_USBMODE_SLOM_MASK;

        /* setup interrupt */
    }
#endif
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}

#endif
/* EOF */
