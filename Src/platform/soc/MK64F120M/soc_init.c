/**HEADER*********************************************************************
*
* Copyright (c) 2011, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: soc_init.c
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

#if ((defined __CWCC__) || (defined __IAR_SYSTEMS_ICC__) || (defined __CC_ARM)|| (defined __arm__))
    extern uint32_t ___VECTOR_RAM[];                         /* Get vector table that was copied to RAM */
#elif defined(__GNUC__)
    extern uint32_t __cs3_interrupt_vector[];
#endif

/*****************************************************************************
 * Local Variables
 *****************************************************************************/

/*****************************************************************************
 * Local function prototypes
 *****************************************************************************/
static int32_t pll_init(void);
/*****************************************************************************
 *
 *    @name     SYS_Init
 *
 *    @brief    This function Initializes the system
 *
 *    @param    None
 *
 *    @return   None
 *
 ****************************************************************************
 * Initializes the MCU, MCG, KBI, RTC modules
 ***************************************************************************/
void SYS_Init(void)
{
    /* Point the VTOR to the new copy of the vector table */
#if (defined(__CWCC__) || defined(__IAR_SYSTEMS_ICC__) || defined(__CC_ARM))
    SCB_VTOR = (uint32_t)___VECTOR_RAM;
#elif defined(__arm__)
    //SCB_VTOR = (uint32_t)__VECTOR_RAM;
#elif defined(__GNUC__)
    SCB_VTOR = (uint32_t)__cs3_interrupt_vector;
#endif
    /* SIM Configuration */
    (void)pll_init();
}

/*****************************************************************************
 * @name     pll_init
 *
 * @brief:   Initialization of the MCU.
 *
 * @param  : None
 *
 * @return : None
 *****************************************************************************
 * It will configure the MCU to disable STOP and COP Modules.
 * It also set the MCG configuration and bus clock frequency.
 ****************************************************************************/
static int32_t pll_init(void)
{
    /* System clock initialization */
    /* SIM_SCGC5: PORTA=1 */
    /* Enable clock gate for ports to enable pin routing */
    if ( *((uint8_t*) 0x03FFU) != 0xFFU) {
        HW_MCG_C3_WR(*((uint8_t*) 0x03FFU));
        HW_MCG_C4_WR((HW_MCG_C4_RD() & 0xE0U) | ((*((uint8_t*) 0x03FEU)) & 0x1FU));
    }
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTA_MASK);
    /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=1,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
    BW_SIM_CLKDIV1_OUTDIV1(0);
    BW_SIM_CLKDIV1_OUTDIV2(1);
    BW_SIM_CLKDIV1_OUTDIV3(2);
    BW_SIM_CLKDIV1_OUTDIV4(4);
    /* SIM_SOPT1: OSC32KSEL=2 */
    HW_SIM_SOPT1_CLR(SIM_SOPT1_USBSSTBY_MASK);
    HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);
    
    /* PORTA_PCR18: ISF=0,MUX=0 */
    HW_PORT_PCRn_CLR(HW_PORTA,18,(uint32_t)0x01000700);
    /* Switch to FBE Mode */
    /* OSC_CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */

    /* MCG_C7: OSCSEL=0 */

    /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=0,LP=0,IRCS=1 */
    BW_MCG_C2_RANGE(2);
    HW_MCG_C2_SET(MCG_C2_IRCS_MASK);

    /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */

    BW_MCG_C1_CLKS(2);
    BW_MCG_C1_FRDIV(5);
    HW_MCG_C1_CLR(MCG_C1_IREFS_MASK);
    /* MCG_C4: DMX32=0,DRST_DRS=0 */

    BW_MCG_C4_DMX32(0);
    BW_MCG_C4_DRST_DRS(0);
    /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=1,PRDIV0=0x13 */

    BW_MCG_C5_PRDIV0(0x13);

    /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=8 */

    BW_MCG_C6_VDIV0(0x18);

    while(BR_MCG_S_IREFST) { /* Check that the source of the FLL reference clock is the external reference clock. */
    }

    while(BR_MCG_S_CLKST != 2) {    /* Wait until external reference clock is selected as MCG output */
    }
    /* Switch to PBE Mode */
    /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=0,LP=0,IRCS=1 */

    HW_MCG_C6_SET(MCG_C6_PLLS_MASK);
    while(BR_MCG_S_CLKST != 2) {    /* Wait until external reference clock is selected as MCG output */
    }
    while(BR_MCG_S_LOCK0 == 0) { /* Wait until locked */
    }
    /* Switch to PEE Mode */
    /* MCG_C1: CLKS=0,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
    BW_MCG_C1_CLKS(0);
    while(BR_MCG_S_CLKST != 3) {    /* Wait until external reference clock is selected as MCG output */
    }
    /* MCG_C6: CME0=1 */
    HW_MCG_C6_SET(MCG_C6_CME0_MASK);
        
        return 1;
}
/* EOF */

