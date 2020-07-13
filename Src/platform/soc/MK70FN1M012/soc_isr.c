/**HEADER*********************************************************************
*
* Copyright (c) 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
*
******************************************************************************
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
******************************************************************************
*
* $FileName: soc_isr.c
* $Version :
* $Date    :
*
* Comments:
*
*  
*
*END*************************************************************************/
#include "types.h"
#include "soc.h"
#include "derivative.h"
#include "soc_pit.h"
#include "soc_i2s.h"
#include "user_config.h"

typedef void (*int_isr_fptr_t)(void*);
static int_isr_fptr_t lowlevel_usb_isr_89 = NULL;
static void* isr_param_89 = NULL;
static void* isr_param_uart2_isr = NULL;

static int_isr_fptr_t lowlevel_usb_isr_112 = NULL;
static void* isr_param_112 = NULL;

#if (DELAY_ENABLE) || (MAX_TIMER_OBJECTS)
static int_isr_fptr_t lowlevel_pit0_isr = NULL;
static int_isr_fptr_t lowlevel_pit1_isr = NULL;
#endif

#if (I2S_ENABLE)
static int_isr_fptr_t lowlevel_i2s0_tx_isr = NULL;
#endif /* I2S_ENABLE */

#if (PORTB_ENABLE)
static int_isr_fptr_t lowlevel_portb_isr_104 = NULL;
static void* isr_param_104 = NULL;
#endif /* PORTB_ENABLE */
static int_isr_fptr_t lowlevel_uart2_isr = NULL;

int32_t soc_install_isr( uint32_t vector, int_isr_fptr_t isr_ptr, void* isr_data)
{
    if(vector == 89)
    {
        lowlevel_usb_isr_89 = isr_ptr;
        isr_param_89 = isr_data;
    }

#if (PORTB_ENABLE)
    if(vector == 104)
    {
        lowlevel_portb_isr_104 = isr_ptr;
        isr_param_104 = isr_data;
    }
#endif /* PORTB_ENABLE */

    if(vector == 112)
    {
        lowlevel_usb_isr_112 = isr_ptr;
        isr_param_112 = isr_data;
    }
#if (DELAY_ENABLE) || (MAX_TIMER_OBJECTS)  
    if(vector == pit_get_vector(0))
    {
        lowlevel_pit0_isr = isr_ptr;
    }
    
    if(vector == pit_get_vector(1))
    {
        lowlevel_pit1_isr = isr_ptr;
    }
#endif

#if I2S_ENABLE
    if(vector == i2s_get_vector(0))
    {
        lowlevel_i2s0_tx_isr = isr_ptr;
    }
#endif
    if(vector == (UART2_RX_TX_IRQn + 16))
    {
        lowlevel_uart2_isr = isr_ptr;
        isr_param_uart2_isr = isr_data;
    }
    return 1;
}

void USB_ISR(void)
{
    if(lowlevel_usb_isr_89 != NULL)
    {
        lowlevel_usb_isr_89(isr_param_89);
    }
}

void USBHS_ISR(void)
{
    if(lowlevel_usb_isr_112 != NULL)
    {
        lowlevel_usb_isr_112(isr_param_112);
    }
}

void PIT0_ISR(void)
{
#if (DELAY_ENABLE) || (MAX_TIMER_OBJECTS)
    if(lowlevel_pit0_isr != NULL)
    {
        lowlevel_pit0_isr(NULL);
    }
#endif
}

void PIT1_ISR(void)
{
#if (DELAY_ENABLE) || (MAX_TIMER_OBJECTS)
    if(lowlevel_pit1_isr != NULL)
    {
        lowlevel_pit1_isr(NULL);
    }
#endif
}

void PORTB_ISR(void)
{
#if (PORTB_ENABLE)
    if(lowlevel_portb_isr_104 != NULL)
    {
        lowlevel_portb_isr_104(isr_param_104);
    }
#endif /* PORTB_ENABLE */
}

void I2S0_TX_ISR(void)
{
#if I2S_ENABLE
    if(lowlevel_i2s0_tx_isr != NULL)
    {
        lowlevel_i2s0_tx_isr(NULL);
    }
#endif /* I2S_ENABLE */
}


void UART2_ISR(void)
{
  if(lowlevel_uart2_isr != NULL)
      lowlevel_uart2_isr(isr_param_uart2_isr);
}
/* EOF */

