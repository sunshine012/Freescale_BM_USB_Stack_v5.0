/**HEADER*********************************************************************
*
* Copyright (c) 2004-2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: soc_sci.c
* $Version :
* $Date    :
*
* Comments: uart driver for K22
*
*  
*
*END*************************************************************************/
#include "types.h"
#include "user_config.h"

#if (PRINTF_ENABLE)
#include "bsp.h"
#include "soc_sci.h"

#include "derivative.h"

const kuart_init_struct_t _bsp_sci1_init = {
   /* queue size         */ 64,
   /* Channel            */ 2,
   /* Clock Speed        */ SCI_CLK,
   /* Baud rate          */ 115200,
   /* RX/TX Int vect     */ UART2_RX_TX_IRQn+16,
   /* ERR Int vect       */ UART2_ERR_IRQn+16,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_serial_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding UART
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_serial_base_address(uint8_t dev_num) {
    void   *addr;

    switch(dev_num) {
    case 0:
        addr = (void *)UART0_BASE_PTR;
        break;
    case 1:
        addr = (void *)UART1_BASE_PTR;
        break;
    case 2:
        addr = (void *)UART2_BASE_PTR;
        break;
    case 3:
        addr = (void *)UART3_BASE_PTR;
        break;
    case 4:
        addr = (void *)UART4_BASE_PTR;
        break;
    case 5:
        addr = (void *)UART5_BASE_PTR;
        break;
    default:
        addr = 0;
    }
    return addr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_get_serial_vector
* Returned Value   : Vector number of specified channel
* Comments         :
*    This function returns the base register address of the corresponding UART
*
*END*----------------------------------------------------------------------*/
uint32_t bsp_get_serial_vector(uint8_t dev_num) 
{
    uint32_t   vector_num;
    switch(dev_num) {
    case 0:
        vector_num = UART0_RX_TX_IRQn;
        break;
    case 1:
        vector_num = UART1_RX_TX_IRQn;
        break;
    case 2:
        vector_num = UART2_RX_TX_IRQn;
        break;
    case 3:
        vector_num = UART3_RX_TX_IRQn;
        break;
    case 4:
        vector_num = UART4_RX_TX_IRQn;
        break;
    case 5:
        vector_num = UART5_RX_TX_IRQn;
        break;
    default:
        vector_num = 0;
    }
    return (vector_num + 16);
}
#endif
