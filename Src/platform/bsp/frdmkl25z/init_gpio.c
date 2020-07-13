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
* Returned Value   : 
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
            HW_SIM_SCGC5_SET(SIM_SCGC5_PORTA_MASK);
            /* Enable the UART5_TXD function on PTE8 */
            BW_PORT_PCRn_MUX(HW_PORTA, 14, 3); // UART is alt3 function for this pin

            /* Enable the UART5_RXD function on PTE9 */
            BW_PORT_PCRn_MUX(HW_PORTA, 15, 3); // UART is alt3 function for this pin

            /* Enable the clock  */ 
            HW_SIM_SCGC4_SET(SIM_SCGC4_UART0_MASK);
            break;

        case 1:
            break;

        case 2:
            break;

        default:
            break;
    }
}
/* EOF */
