/**HEADER********************************************************************
*
* Copyright (c) 2008, 2014 Freescale Semiconductor;
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
* $FileName: init_gpio.c
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains board-specific pin initialization functions.
*
*END************************************************************************/

#include "types.h"
#include "user_config.h"
#include "bsp.h"
#include "derivative.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_serial_io_init
* Returned Value   : MQX_OK for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to serial
*
*END*----------------------------------------------------------------------*/
void bsp_serial_io_init
(
    /* [IN] Serial device number */
    uint8_t dev_num
)
{
    /* Setup GPIO for UART devices */
    switch (dev_num)
    {
        case 0:
            break;

        case 1:
            break;

        case 2:
            HW_SIM_SCGC5_SET(SIM_SCGC5_PORTE_MASK);
            //SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
            /* Enable the UART2_TXD function on PTE16 */   
            BW_PORT_PCRn_MUX(HW_PORTE, 16, 3);
            //PORTE_PCR16 = PORT_PCR_MUX(0x3); 
            /* Enable the UART2_RXD function on PTE17 */
            BW_PORT_PCRn_MUX(HW_PORTE, 17, 3);
            //PORTE_PCR17 = PORT_PCR_MUX(0x3); 
            /* Enable the clock  */
            HW_SIM_SCGC4_SET(SIM_SCGC4_UART2_MASK);
            //SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
            break;

        default:
            break;
    }
}
/* EOF */
