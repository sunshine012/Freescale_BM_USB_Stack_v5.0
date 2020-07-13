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
* $FileName: serl_int_kuart.c$
* $Version : 3.8.10.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains the low level functions for the interrupt driven
*   serial I/O for the SCI device.
*
*END************************************************************************/
#include "types.h"
#include "user_config.h"
#include "bsp.h"
	
#include "derivative.h"

#include "serial.h"
#include "io_serl_int.h"
#include "io_prv.h"
#include "charq.h"
#include "serinprv.h"
#include "serl_kuart.h"

/* Polled functions used */
extern uint32_t _kuart_change_baudrate(UART_MemMapPtr, uint32_t, uint32_t);
extern uint32_t _kuart_polled_init(kuart_init_struct_t *, void **, char *);
extern uint32_t _kuart_polled_deinit(kuart_init_struct_t *, kuart_info_struct_t *);
extern uint32_t _kuart_polled_ioctl(kuart_info_struct_t *, uint32_t, uint32_t *);

/* Interrupt driver functions */

extern void    _kuart_int_putc(io_serial_int_device_struct_t *, char);
extern uint32_t _kuart_int_init(io_serial_int_device_struct_t *, char *);
extern uint32_t _kuart_int_deinit(kuart_init_struct_t *, kuart_info_struct_t *);
extern uint32_t _kuart_int_enable(kuart_info_struct_t *);
extern void    _kuart_int_err_isr(void *);
extern void    _kuart_int_rx_tx_isr(void *);

static kuart_info_struct_t kuart_info = {0};

extern const kuart_init_struct_t _bsp_sci1_init;


io_serial_int_device_struct_t sci_int_dev = 
{
	   /* The I/O init function */
	   (uint32_t (*)(void*, char*))_kuart_int_init,
	
	   /* The enable interrupts function */
	   (uint32_t (*)(void*))_kuart_int_enable,
	
	   /* The I/O deinit function */
	  (uint32_t (*)(void*, void*))_kuart_int_deinit,
	
	   /* The output function, used to write out the first character */
	   (void (*)(void*, char))_kuart_int_putc,
	
	   /* The ioctl function, (change bauds etc) */
	   NULL,
	
	   /* The I/O channel initialization data */
	   (void*)&_bsp_sci1_init,
	   
	   /* Device specific information */
	   NULL,
	
	   /* The queue size to use */
	   64,
	   
	   /* Open count for number of accessing file descriptors */
	   0,
	
	   /* Open flags for this channel */
	   0,
	
	   /* The input queue */
	   NULL,
	
	   /* The input waiting tasks */
	   NULL,
	
	   /* The output queue */
	   NULL,
	
	   /* The output waiting tasks */
	   NULL,
	
	   /* Has output been started */
	   0,
	   
#if (PSP_MQX_CPU_IS_KINETIS || PSP_MQX_CPU_IS_VYBRID) 
	   bool 			tx_dma_ongoing,
#endif   
	   /* Protocol flag information */
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_change_baudrate
* Returned Value   : MQX error code
* Comments         :
*    Calculates and sets new baudrate dividers for given SCI channel.
*
*END*----------------------------------------------------------------------*/

uint32_t _kuart_change_baudrate
    (
        /* [IN] SCI channel registers */
        UART_MemMapPtr sci_ptr,

        /* [IN] SCI input clock frequency */
        uint32_t        clock_frequency,

        /* [IN] Requested baud rate */
        uint32_t        baud_rate
    )
{
    uint32_t            baud_divisor = 0, brfa = 0;

    if ((clock_frequency > 0) && (baud_rate > 0))
    {
        baud_divisor = clock_frequency / (baud_rate << 4);
        if (baud_divisor > 0)
        {
            brfa = ((((clock_frequency - baud_rate*(baud_divisor << 4)) << 1) + (baud_rate >> 1)) / baud_rate);
            if (brfa >= 32) {
                baud_divisor += 1;
                brfa = 0;
            }
        }
        if (baud_divisor > ((UART_BDH_SBR_MASK << 8) | UART_BDL_SBR_MASK))
        {
            return (uint32_t)SERL_INT_ERROR;
        }
    }
    sci_ptr->BDH = (unsigned char)((baud_divisor >> 8) & UART_BDH_SBR_MASK);
    sci_ptr->BDL = (unsigned char)(baud_divisor & UART_BDL_SBR_MASK);
    sci_ptr->C4 &= (~ UART_C4_BRFA_MASK);
    sci_ptr->C4 |= UART_C4_BRFA(brfa);
    return SERL_INT_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_peripheral_enable
* Returned Value   : None
* Comments         :
*    Enables the SCI peripheral.
*
*END*----------------------------------------------------------------------*/

static void _kuart_int_peripheral_enable
    (
        /* [IN] SCI channel */
        UART_MemMapPtr sci_ptr
    )
{
    /* Enable only receive interrupt, transmit will be enabled during sending first character */
    sci_ptr->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_RIE_MASK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_peripheral_disable
* Returned Value   : None
* Comments         :
*    Disables the SCI peripheral.
*
*END*----------------------------------------------------------------------*/

static void _kuart_int_peripheral_disable
    (
        /* [IN] SCI channel */
        UART_MemMapPtr sci_ptr
    )
{
    /* Transmitter and receiver disable */
    sci_ptr->C2 &= (~ (UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_RIE_MASK | UART_C2_TIE_MASK));
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_install
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    Install an interrupt driven uart serial device.
*
*END*----------------------------------------------------------------------*/

uint32_t _kuart_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *identifier,

      /* [IN] The I/O init data pointer */
      kuart_init_struct_cptr  init_data_ptr,

      /* [IN] The I/O queue size to use */
      uint32_t  queue_size
   )
{ /* Body */

#if PE_LDD_VERSION
    if (PE_PeripheralUsed((uint32_t)_bsp_get_serial_base_address(init_data_ptr->device)))
    {
        return IO_ERROR;
    }
#endif

   return _io_serial_int_install(identifier,
      (uint32_t (_CODE_PTR_)(void *, char *))_kuart_int_init,
      (uint32_t (_CODE_PTR_)(void *))_kuart_int_enable,
      (uint32_t (_CODE_PTR_)(void *,void *))_kuart_int_deinit,
      (void    (_CODE_PTR_)(void *, char))_kuart_int_putc,
      (uint32_t (_CODE_PTR_)(void *, uint32_t, void *))_kuart_polled_ioctl,
      (void *)init_data_ptr, queue_size);

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_polled_init
* Returned Value   : SERL_INT_OK or a MQX error code.
* Comments         :
*    This function initializes the SCI
*
*END*********************************************************************/

uint32_t _kuart_polled_init
   (
      /* [IN] the initialization information for the device being opened */
      kuart_init_struct_t *               io_init_ptr,

      /* [OUT] the address to store device specific information */
      void                          **io_info_ptr_ptr,

      /* [IN] the rest of the name of the device opened */
      char                           *open_name_ptr
   )
{ /* Body */
   UART_MemMapPtr                       sci_ptr;
   kuart_info_struct_t *                sci_info_ptr;
   uint32_t                              channel, clock;
   uint8_t                               flags;

   /* Get peripheral address */
   channel = io_init_ptr->device;

   sci_ptr = _bsp_get_serial_base_address(channel);
   if (sci_ptr == NULL)
   {
      return (uint32_t)SERL_INT_ERROR;
   }

   sci_info_ptr = &kuart_info;

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if ( sci_info_ptr == NULL )
   {
      return (uint32_t)SERL_INT_ERROR;
   }
#endif

   *io_info_ptr_ptr = sci_info_ptr;

   sci_info_ptr->sci_ptr = sci_ptr;

   /* Save initialization values */
   sci_info_ptr->init = *io_init_ptr;
   
   sci_info_ptr->flags = IO_SERIAL_STOP_BITS_1;

   /* Setup HW according to low power mode, if enabled */
   clock = sci_info_ptr->init.clock_speed;
   flags = IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE;

   /* Enable HW */
   /* Enable module clocks to be able to write registers */
   bsp_serial_io_init (channel);

   /* Setup baudrate */
   _kuart_change_baudrate (sci_ptr, clock, sci_info_ptr->init.baud_rate);
   
   /* 8-bit mode. Normal operation */
   sci_ptr->C1 = 0;

   /* Disable wakeups */
   sci_ptr->C2 &= (~ (UART_C2_RWU_MASK));
   sci_ptr->C4 &= (~ (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
   sci_ptr->MA1 = 0;
   sci_ptr->MA2 = 0;

   /* Disable all error interrupts */
   sci_ptr->C3 = 0;

   /* Set watermark in the almost full TX buffer */
   if (((sci_ptr->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK) >> UART_PFIFO_TXFIFOSIZE_SHIFT) == 0)
   {
      /* 1 dataword in D */
      sci_ptr->TWFIFO = UART_TWFIFO_TXWATER(0);
   }
   else
   {
      uint8_t txsize = 1 << (((sci_ptr->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK) >> UART_PFIFO_TXFIFOSIZE_SHIFT) + 1);

      /* Watermark for TX buffer generates interrupts below & equal to watermark */
      sci_ptr->TWFIFO = UART_TWFIFO_TXWATER(txsize - 1);
   }

   /* Watermark for RX buffer generates interrupts above & equal to watermark */
   sci_ptr->RWFIFO = UART_RWFIFO_RXWATER(1);

   /* both RE,TE must be disabled before enable FIFO */
   sci_ptr->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);

   /* Enable TX FIFO, enable RX FIFO */
   sci_ptr->PFIFO |= UART_PFIFO_TXFE_MASK | UART_PFIFO_RXFE_MASK;

   /* Flush RX / TX buffers */
   sci_ptr->CFIFO |= UART_CFIFO_RXFLUSH_MASK | UART_CFIFO_TXFLUSH_MASK;

   /* Module enable/disable */
   if (flags & IO_PERIPHERAL_MODULE_ENABLE)
   {
       //_kuart_polled_peripheral_enable (sci_ptr);
   }
   else
   {
       //_kuart_polled_peripheral_disable (sci_ptr);
   }

   /* Disable module clocks if required */
   if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
   {
       //_bsp_serial_io_init (channel, IO_PERIPHERAL_CLOCK_DISABLE);
   }

   return SERL_INT_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_polled_deinit
* Returned Value   : MQX_OK or a mqx error code.
* Comments         :
*    This function de-initializes the SCI.
*
*END*********************************************************************/

uint32_t _kuart_polled_deinit
   (
      /* [IN] the initialization information for the device being opened */
      kuart_init_struct_t * io_init_ptr,

      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr
   )
{
//TO DO
	return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_polled_ioctl
* Returned Value   : uint32_t MQX_OK or a mqx error code.
* Comments         :
*    This function performs miscellaneous services for
*    the I/O device.
*
*END*********************************************************************/

uint32_t _kuart_polled_ioctl
   (
      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr,

      /* [IN] The command to perform */
      uint32_t                    cmd,

      /* [IN] Parameters for the command */
      uint32_t                *param_ptr
   )
{ /* Body */
	return IO_OK;
}
/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_init
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    This function initializes the SCI in interrupt mode.
*
*END*********************************************************************/

uint32_t _kuart_int_init
   (
      /* [IN] the interrupt I/O initialization information */
      io_serial_int_device_struct_t * int_io_dev_ptr,

      /* [IN] the rest of the name of the device opened */
      char                       *open_name_ptr
   )
{ /* Body */
   kuart_init_struct_t * sci_init_ptr;
   uint32_t                     result;

   sci_init_ptr = int_io_dev_ptr->dev_init_data_ptr;
   result = _kuart_polled_init((void *)sci_init_ptr, &int_io_dev_ptr->dev_info_ptr, open_name_ptr);
   if (result != IO_OK) {
       return(result);
   }/* Endif */

   soc_install_isr(bsp_get_serial_vector(sci_init_ptr->device), _kuart_int_rx_tx_isr, int_io_dev_ptr);
   bm_int_init(bsp_get_serial_vector(sci_init_ptr->device), 0, TRUE);
   return(IO_OK);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_deinit
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    This function de-initializes the UART in interrupt mode.
*
*END*********************************************************************/

uint32_t _kuart_int_deinit
   (
      /* [IN] the interrupt I/O initialization information */
      kuart_init_struct_t * io_init_ptr,

      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr
   )
{ /* Body */
   return(IO_OK);
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_enable
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    This function enables the UART interrupts mode.
*
*END*********************************************************************/

uint32_t _kuart_int_enable
   (
      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr
   )
{ /* Body */
   uint8_t                  flags = IO_PERIPHERAL_MODULE_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE;
   UART_MemMapPtr          sci_ptr = io_info_ptr->sci_ptr;

   /* Enable module clocks to be able to write registers */
   //_bsp_serial_io_init (io_info_ptr->init.device, IO_PERIPHERAL_CLOCK_ENABLE);

   /* Enable/disable module */
   if (flags & IO_PERIPHERAL_MODULE_ENABLE)
   {
      _kuart_int_peripheral_enable (sci_ptr);
   }
   else
   {
      _kuart_int_peripheral_disable (sci_ptr);
   }

   /* Disable module clocks if required */
   if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
   {
      //_bsp_serial_io_init (io_info_ptr->init.device, IO_PERIPHERAL_CLOCK_DISABLE);
   }

   return IO_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_err_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial error interrupts.
*
*************************************************************************/

void _kuart_int_err_isr
   (
      /* [IN] the address of the device specific information */
      void   *parameter
   )
{ /* Body */

   io_serial_int_device_struct_t *        int_io_dev_ptr = parameter;
   kuart_info_struct_t *                  sci_info_ptr = int_io_dev_ptr->dev_info_ptr;
   UART_MemMapPtr                         sci_ptr = sci_info_ptr->sci_ptr;
   uint16_t                                stat = sci_ptr->S1;

   ++sci_info_ptr->interrupts;

   if(stat & UART_S1_OR_MASK) {
      ++sci_info_ptr->rx_overruns;
   }
   if(stat & UART_S1_PF_MASK) {
      ++sci_info_ptr->rx_parity_errors;
   }
   if(stat & UART_S1_NF_MASK) {
      ++sci_info_ptr->rx_noise_errors;
   }
   if(stat & UART_S1_FE_MASK) {
      ++sci_info_ptr->rx_framing_errors;
   }

   /*
   if "framming error" or "overrun" error occours 
   perform 'S1' cleanup. if not, 'S1' cleanup will be 
   performed during regular reading of 'D' register.
   */
   if (stat & (UART_S1_OR_MASK | UART_S1_FE_MASK))
   {
      // reading register 'D' to cleanup 'S1' may cause 'RFIFO' underflow
      sci_ptr->D;
      // if 'RFIFO' underflow detected, perform flush to reinitialize 'RFIFO'
      if (sci_ptr->SFIFO & UART_SFIFO_RXUF_MASK)
      {
         sci_ptr->CFIFO |= UART_CFIFO_RXFLUSH_MASK;
         sci_ptr->SFIFO |= UART_SFIFO_RXUF_MASK;
      }
      // set errno. transmit is corrupted, nothing to process
      // TODO: add valid errno. IO_ERROR causes signed/unsigned assignment warning
      // _task_set_error(IO_ERROR);
   }

}  /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_rx_tx_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial input interrupts.
*
*************************************************************************/

void _kuart_int_rx_tx_isr
   (
      /* [IN] the address of the device specific information */
      void   *parameter
   )
{ /* Body */
   io_serial_int_device_struct_t *        int_io_dev_ptr = parameter;
   kuart_info_struct_t *                  sci_info_ptr = int_io_dev_ptr->dev_info_ptr;
   UART_MemMapPtr                         sci_ptr = sci_info_ptr->sci_ptr;
   volatile int32_t                        c;
   uint16_t                                stat = sci_ptr->S1;

   ++sci_info_ptr->interrupts;

   /* 
   process error flags in case:
   - there is single interrupt line for both data and errors
   - _kuart_int_err_isr has lower priority than _kuart_int_rx_tx_isr
   - error interrupts are not enabled
   */
   if(stat & UART_S1_OR_MASK) {
      ++sci_info_ptr->rx_overruns;
   }
   if(stat & UART_S1_PF_MASK) {
      ++sci_info_ptr->rx_parity_errors;
   }
   if(stat & UART_S1_NF_MASK) {
      ++sci_info_ptr->rx_noise_errors;
   }
   if(stat & UART_S1_FE_MASK) {
      ++sci_info_ptr->rx_framing_errors;
   }

   /*
   if "framming error" or "overrun" error occours 
   perform 'S1' cleanup. if not, 'S1' cleanup will be 
   performed during regular reading of 'D' register.
   */
   if (stat & (UART_S1_OR_MASK | UART_S1_FE_MASK))
   {
      // reading register 'D' to cleanup 'S1' may cause 'RFIFO' underflow
      sci_ptr->D;
      // if 'RFIFO' underflow detected, perform flush to reinitialize 'RFIFO'
      if (sci_ptr->SFIFO & UART_SFIFO_RXUF_MASK)
      {
         sci_ptr->CFIFO |= UART_CFIFO_RXFLUSH_MASK;
         sci_ptr->SFIFO |= UART_SFIFO_RXUF_MASK;
      }
      // set errno. transmit is corrupted, nothing to process
      // TODO: add valid errno. IO_ERROR causes signed/unsigned assignment warning
      // _task_set_error(IO_ERROR);
      return;
   }

   /*
   try if RX buffer has some characters.
   */
   if (stat & UART_S1_RDRF_MASK) {
      // reading 'D' register performs also cleanup of 'S1'
      c = sci_ptr->D;
      if (!_io_serial_int_addc(int_io_dev_ptr, c)) {
          sci_info_ptr->rx_dropped_input++;
      }
      sci_info_ptr->rx_chars++;
   }

   /* try if TX buffer is still not full */
   if (stat & UART_S1_TDRE_MASK) {
      c = _io_serial_int_nextc(int_io_dev_ptr);
      if (c >= 0) {
          sci_ptr->D = c;
      } else {
         /* All data sent, disable transmit interrupt */
         sci_ptr->C2 &= ~UART_C2_TIE_MASK;
      }
      sci_info_ptr->tx_chars++;
   }

}  /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_putc
* Returned Value   : none
* Comments         :
*   This function is called to write out the first character, when
* the output serial device and output ring buffers are empty.
*
*END*********************************************************************/

void _kuart_int_putc
   (
      /* [IN] the address of the device specific information */
      io_serial_int_device_struct_t * int_io_dev_ptr,

      /* [IN] the character to write out now */
      char                       c
   )
{ /* Body */
   kuart_info_struct_t *                  sci_info_ptr;
   UART_MemMapPtr                         sci_ptr;

   sci_info_ptr = int_io_dev_ptr->dev_info_ptr;
   sci_ptr = sci_info_ptr->sci_ptr;

   while (!(sci_ptr->S1 & UART_S1_TDRE_MASK)) {
      /* Wait while buffer is full */
   } /* Endwhile */

   sci_ptr->D = c;
   sci_info_ptr->tx_chars++;

   /* Enable transmit iterrupt */
   sci_ptr->C2 |= UART_C2_TIE_MASK;

} /* Endbody */

/* EOF */
