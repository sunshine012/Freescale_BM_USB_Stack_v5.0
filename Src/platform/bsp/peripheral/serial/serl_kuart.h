/**HEADER********************************************************************
*
* Copyright (c) 2008, 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
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
* $FileName: serl_kuart.h$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the sci drivers for the MCF51XX
*
*END************************************************************************/
#ifndef _serial_kuart_h_
#define _serial_kuart_h_

#include "soc_sci.h"
#if (PSP_MQX_CPU_IS_KINETIS || PSP_MQX_CPU_IS_VYBRID)
#include <lwtimer.h>
#include <dma.h>
#endif
/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
#define SERL_INT_ERROR (-1)
#define SERL_INT_OK    (0)

typedef const kuart_init_struct_t * kuart_init_struct_cptr;

/*
** KUART_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct kuart_info_struct
{
   /* The current init values for this port */
   kuart_init_struct_t          init;

   /* The sci device register */
   UART_MemMapPtr             sci_ptr;

   /* The previous interrupt handler and data */
   void       (_CODE_PTR_ old_isr)(void *);
   void       (_CODE_PTR_ old_isr_exception_handler)(uint32_t, uint32_t, void *,
               void *);
   void                             *old_isr_data;

   /* Various flags */
   uint32_t                           flags;
   
   /* Statistical information */
   uint32_t                           interrupts;
   uint32_t                           rx_chars;
   uint32_t                           tx_chars;
   uint32_t                           rx_breaks;
   uint32_t                           rx_parity_errors;
   uint32_t                           rx_framing_errors;
   uint32_t                           rx_overruns;
   uint32_t                           rx_dropped_input;
   uint32_t                           rx_noise_errors;
#if (PSP_MQX_CPU_IS_KINETIS || PSP_MQX_CPU_IS_VYBRID)   
   uint8_t tx_dma_chan;
   uint8_t rx_dma_chan;
   uint32_t tx_dma_hardware_request;
   uint32_t rx_dma_hardware_request;
   uint32_t err_int;
   LWTIMER_PERIOD_STRUCT_PTR lw_timer_ptr;
   LWTIMER_STRUCT_PTR lw_timer;
   uint8_t tx_keep;
   uint8_t rx_keep;
   DMA_CHANNEL_HANDLE tx_dch;
   DMA_CHANNEL_HANDLE rx_dch;
   DMA_TCD tx_tcd;
   DMA_TCD rx_tcd;
   uint32_t tx_dma_seq;
   uint32_t rx_dma_seq;	
#endif
} kuart_info_struct_t;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _kuart_polled_init(kuart_init_struct_t *, void **,char *);
extern uint32_t _kuart_polled_install(char *, kuart_init_struct_cptr, uint32_t);
extern uint32_t _kuart_int_install(char *, kuart_init_struct_cptr, uint32_t);
extern void   *_bsp_get_serial_base_address(uint8_t);
extern uint32_t _kuart_dma_install(char *, kuart_init_struct_cptr, uint32_t);
#if PSP_MQX_CPU_IS_KINETIS 
extern uint32_t _bsp_get_serial_error_int_num(uint8_t dev_num);
#endif
#if (PSP_MQX_CPU_IS_KINETIS || PSP_MQX_CPU_IS_VYBRID)
extern uint32_t _bsp_get_serial_tx_dma_request(uint8_t dev_num);
extern uint32_t _bsp_get_serial_rx_dma_request(uint8_t dev_num);
#endif

#ifdef __cplusplus
}
#endif

#endif //_serial_kuart_h_

/* EOF */
