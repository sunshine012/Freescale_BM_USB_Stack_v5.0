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
* $FileName: sdhc_bsp.c
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
* Function Name    : sdhc_io_init
* Returned Value   : 
* Comments         :
*    This function performs BSP-specific initialization related to sdhc
*
*END*----------------------------------------------------------------------*/
void sdhc_detect_io_init(void)
{
    /* PTE28 */
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTE_MASK);
    BW_PORT_PCRn_MUX(HW_PORTE, 28, 1);
    HW_GPIO_PDDR_CLR(HW_PORTE,(0x01 << 28));
    HW_PORT_PCRn_SET(HW_PORTE,28,(PORT_PCR_PE_MASK|PORT_PCR_PS_MASK));
    HW_GPIO_PCOR_WR(HW_PORTE,(0x01 << 28));
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : sdhc_detect
* Returned Value   : 0-sd don't insert; 1-sd inserts.
* Comments         :
*    This function performs BSP-specific initialization related to sdhc
*
*END*----------------------------------------------------------------------*/
uint8_t sdhc_detect(void)
{
    /* PTE28 */
    return (!(HW_GPIO_PDIR_RD(HW_PORTE) & (0x01 << 28)));
}
/* EOF */
