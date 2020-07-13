/**HEADER*********************************************************************
*
* Copyright (c) 2014 Freescale Semiconductor;
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
* $FileName: soc_sci.h
* $Version :
* $Date    :
*
* Comments:
*
*  
*
*END*************************************************************************/
 #ifndef _SOC_SCI_H_
 #define _SOC_SCI_H_

/*INCLUDES*-----------------------------------------------------------------*/

/*MACROS*-------------------------------------------------------------------*/

#if (defined(CPU_MK22F51212))
#elif (defined(CPU_MK22F12812))
#define SCI_CLK      96000000
#endif
/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
/*
** kuart_init_struct_t
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct kuart_init_struct
{

   /* The size of the queues to buffer incoming/outgoing data */
   uint32_t queue_size;

   /* The device to initialize */
   uint32_t device;

   /* The clock speed of cpu */
   uint32_t clock_speed;

   /* The baud rate for the channel */
   uint32_t baud_rate;

   /* RX / TX interrupt vector */
   uint32_t rx_tx_vector;

   /* ERR interrupt vector */
   uint32_t err_vector;

   /* RX / TX interrupt vector priority */
   uint32_t rx_tx_priority;

   /* ERR interrupt vector priority */
   uint32_t err_priority;

#if (PSP_MQX_CPU_IS_KINETIS || PSP_MQX_CPU_IS_VYBRID)  
	uint8_t tx_dma_channel;
	uint8_t rx_dma_channel;
#endif        
} kuart_init_struct_t;

/*GLOBAL FUNCTION PROTOTYPES*-----------------------------------------------*/
void *_bsp_get_serial_base_address(uint8_t dev_num);
uint32_t bsp_get_serial_vector(uint8_t dev_num);

#endif
/* EOF */
