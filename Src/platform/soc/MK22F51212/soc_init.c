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


#define BSP_CLOCK_SRC   (8000000ul)                         /* crystal, oscillator freq. */
#define BSP_REF_CLOCK_SRC   (2000000ul)                     /* must be 2-4MHz */

#define BSP_REF_CLOCK_DIV   (BSP_CLOCK_SRC / BSP_REF_CLOCK_SRC)
#define BSP_CLOCK           (BSP_REF_CLOCK_SRC * BSP_CLOCK_MUL)
#define BSP_CORE_CLOCK      (BSP_CLOCK / BSP_CORE_DIV)      /* CORE CLK, max 100MHz */
#define BSP_SYSTEM_CLOCK    (BSP_CORE_CLOCK)                /* SYSTEM CLK, max 100MHz */
#define BSP_BUS_CLOCK       (BSP_CLOCK / BSP_BUS_DIV)       /* max 50MHz */
#define BSP_FLEXBUS_CLOCK   (BSP_CLOCK / BSP_FLEXBUS_DIV)
#define BSP_FLASH_CLOCK     (BSP_CLOCK / BSP_FLASH_DIV)     /* max 25MHz */

enum usbhs_clock
{
    MCGPLL0,
    MCGPLL1,
    MCGFLL,
    PLL1,
    CLKIN
};

/*! Driver operation mode type. */
typedef enum {
    DOM_NONE,
    DOM_RUN,
    DOM_WAIT,
    DOM_SLEEP,
    DOM_STOP,
    DOM_HSRUN
} CPUOperationMode;

typedef struct clock_frequency_divider_struct 
{
    int32_t PRDIV;
    int32_t VDIV;  
}clock_frequency_divider_struct_t;

typedef enum {
  CLOCK_FREQUENCY_72M,
  CLOCK_FREQUENCY_120M 
}CLOCK_TYPE_E;

/* Constants for use in pll_init */
#define NO_OSCINIT          0
#define OSCINIT             1

#define OSC_0               0
#define OSC_1               1

#define LOW_POWER           0
#define HIGH_GAIN           1

#define CANNED_OSC          0
#define CRYSTAL             1

#define PLL_0               0
#define PLL_1               1

#define PLL_ONLY            0
#define MCGOUT              1

#define BLPI                1
#define FBI                 2
#define FEI                 3
#define FEE                 4
#define FBE                 5
#define BLPE                6
#define PBE                 7
#define PEE                 8

/* IRC defines */
#define SLOW_IRC            0
#define FAST_IRC            1

/*
 * Input Clock Info
 */
#define CLK0_FREQ_HZ        50000000
#define CLK0_TYPE           CANNED_OSC

#define CLK1_FREQ_HZ        12000000
#define CLK1_TYPE           CRYSTAL

/* Select Clock source */
#define USB_CLOCK           MCGPLL0

/* The expected PLL output frequency is:
 * PLL out = (((CLKIN/PRDIV) x VDIV) / 2)
 * where the CLKIN can be either CLK0_FREQ_HZ or CLK1_FREQ_HZ.
 * 
 * For more info on PLL initialization refer to the mcg driver files.
 */

#define PLL0_PRDIV          5
#define PLL0_VDIV           24

#define PLL1_PRDIV          5
#define PLL1_VDIV           30
#define PE_WFI() \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
  asm("WFI") \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
//static int32_t mcg_clk_hz;
//static int32_t mcg_clk_khz;
//static int32_t core_clk_khz;
//static int32_t periph_clk_khz;
//static int32_t pll_0_clk_khz;
//static int32_t pll_1_clk_khz;

clock_frequency_divider_struct_t clock_frequency_list[2] = 
{
  {0x02,0x03},
  {0x01,0x06}
};

/*****************************************************************************
 * Local function prototypes
 *****************************************************************************/
static int32_t pll_init(int32_t);
static int32_t Cpu_SetOperationMode(CPUOperationMode);
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
    (void)pll_init(CLOCK_FREQUENCY_72M);
    (void)Cpu_SetOperationMode(DOM_HSRUN);    
     /*Before increasing clock frequency, the PMSTAT register should be polled to determine enter into HSRUN MODE*/
     while(BR_SMC_PMSTAT_PMSTAT!= 0x80);
    
#if (defined(CPU_MK22F51212))
    (void)pll_init(CLOCK_FREQUENCY_120M);
#endif
    
}


static int32_t Cpu_SetOperationMode(CPUOperationMode OperationMode)
{
    switch (OperationMode) 
    {
        case DOM_HSRUN:
            HW_SMC_PMPROT_WR(SMC_PMPROT_AHSRUN_MASK);
            /* SMC_PMCTRL: 0x60, SMC_PMPROT: 0x80 */
            while(BR_SMC_PMSTAT_PMSTAT != 0x01) 
            {
            }
            HW_SMC_PMCTRL_SET((uint8_t)0x60UL);
            /* SCB_SCR: SLEEPDEEP=0,SLEEPONEXIT=0 */
            SCB_SCR &= (uint32_t)~0x06UL;
  
#if (defined(CPU_MK22F51212))
            if (BR_MCG_S_CLKST != 3) 
            { /* If in PBE mode, switch to PEE. PEE to PBE transition was caused by wakeup from low power mode. */
                /* MCG_C1: CLKS=0,IREFS=0 */
                HW_MCG_C1_CLR((uint8_t)0xC4U);
                while(BR_MCG_S_LOCK0 == 0x00U) 
                { /* Wait for PLL lock */
                }
            }
#endif
            break;
        default:
            return 0;
    }
    return 0;
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
static int32_t pll_init(int32_t clock_frequency_index)
{
#if (defined(CPU_MK22F51212))
    /* System clock initialization */
    if ( *((uint8_t*) 0x03FFU) != 0xFFU) 
    {
        HW_MCG_C3_WR(*((uint8_t*) 0x03FFU));
        HW_MCG_C4_WR((HW_MCG_C4_RD() & 0xE0U) | ((*((uint8_t*) 0x03FEU)) & 0x1FU));
    }
     /* SIM->SCGC5: PORTA=1 */
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTA_MASK);   /* Enable clock gate for ports to enable pin routing */
    /* SIM->CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=1,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
    BW_SIM_CLKDIV1_OUTDIV1(0);
    BW_SIM_CLKDIV1_OUTDIV2(1);
    BW_SIM_CLKDIV1_OUTDIV3(2);
    BW_SIM_CLKDIV1_OUTDIV4(4);
    /* SIM->SOPT2: PLLFLLSEL=1 */
    BW_SIM_SOPT2_PLLFLLSEL(1);
    /* SIM->SOPT1: OSC32KSEL=3 */
    BW_SIM_SOPT1_OSC32KSEL(3);
    /* PORTA->PCR[18]: ISF=0,MUX=0 */
    HW_PORT_PCRn_CLR(HW_PORTA,18,(uint32_t)0x01000700);
    /* PORTA->PCR[19]: ISF=0,MUX=0 */
    HW_PORT_PCRn_CLR(HW_PORTA,19,(uint32_t)0x01000700);
    /* Switch to FBE Mode */
    /* MCG->C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
    BW_MCG_C2_RANGE(2);
    HW_MCG_C2_SET(MCG_C2_IRCS_MASK);
    HW_MCG_C2_SET(MCG_C2_EREFS_MASK);
    /* OSC->CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
    HW_OSC_CR_SET(0,OSC_CR_ERCLKEN_MASK);
    /* MCG->C7: OSCSEL=0 */
    //MCG->C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
    HW_MCG_C7_CLR(MCG_C7_OSCSEL_MASK);
    /* MCG->C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
    BW_MCG_C1_CLKS(2);
    BW_MCG_C1_FRDIV(3);
    HW_MCG_C1_CLR(MCG_C1_IREFS_MASK);
    HW_MCG_C1_SET(MCG_C1_IRCLKEN_MASK);
    HW_MCG_C4_CLR(MCG_C4_DMX32_MASK);
    BW_MCG_C4_DRST_DRS(0);
    /* MCG->C4: DMX32=0,DRST_DRS=0 */
    //MCG->C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
    BW_MCG_C5_PRDIV0(clock_frequency_list[clock_frequency_index].PRDIV);
    /* MCG->C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
    /* MCG->C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=6 */
    BW_MCG_C6_VDIV0(clock_frequency_list[clock_frequency_index].VDIV);

    while(BR_MCG_S_IREFST) 
    { /* Check that the source of the FLL reference clock is the external reference clock. */
    }
    while((HW_MCG_S_RD() & 0x0CU) != 0x08U) 
    {    /* Wait until external reference clock is selected as MCG output */
    }
    /* Switch to PBE Mode */
    /* MCG->C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=6 */

    HW_MCG_C6_SET(MCG_C6_PLLS_MASK);
    BW_MCG_C6_VDIV0(clock_frequency_list[clock_frequency_index].VDIV);


    while((HW_MCG_S_RD() & 0x0CU) != 0x08U) 
    {    /* Wait until external reference clock is selected as MCG output */
    }
    while(BR_MCG_S_LOCK0 == 0x00U) 
    { /* Wait until locked */
    }
    /* Switch to PEE Mode */
    /* MCG->C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
    BW_MCG_C1_CLKS(0x00);
    BW_MCG_C1_FRDIV(0x03);
    HW_MCG_C1_SET(MCG_C1_IRCLKEN_MASK);
    
    while((HW_MCG_S_RD() & 0x0CU) != 0x0CU) 
    {    /* Wait until output of the PLL is selected */
    }
    /* Set USB input clock to 48MHz  */
    /* SIM->CLKDIV2: USBDIV=4,USBFRAC=1 */
    if(clock_frequency_index == CLOCK_FREQUENCY_120M)
    {
       BW_SIM_CLKDIV2_USBDIV(0);
       BW_SIM_CLKDIV2_USBDIV(4);
       HW_SIM_CLKDIV2_SET(SIM_CLKDIV2_USBFRAC_MASK);
    }
#elif defined (CPU_MK22F12812)
    /* SIM->SCGC5: PORTA=1 */
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTA_MASK); /* Enable clock gate for ports to enable pin routing */
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTC_MASK); /* Enable clock gate for ports to enable pin routing */
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTE_MASK); /* Enable clock gate for ports to enable pin routing */
    /* SIM->CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=1,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
    /* Update system prescalers */
    BW_SIM_CLKDIV1_OUTDIV1(0);     
    BW_SIM_CLKDIV1_OUTDIV2(0);
    BW_SIM_CLKDIV1_OUTDIV4(4);

    /* PORTA->PCR[18]: ISF=0,MUX=0 */
    HW_PORT_PCRn_CLR(HW_PORTA,18,(uint32_t)0x01000700);
    /* PORTA->PCR[19]: ISF=0,MUX=0 */
    HW_PORT_PCRn_CLR(HW_PORTA,19,(uint32_t)0x01000700);
    
    // switch to FEE mode
    // Selects 32 kHz RTC Oscillator
    BW_MCG_C7_OSCSEL(1);
    // Low frequency range selected for the crystal oscillator
    HW_MCG_C2_CLR(MCG_C2_RANGE_MASK);
    // Divide Factor is 1
    HW_MCG_C1_CLR(MCG_C1_FRDIV_MASK);
    // select FLL for MCGOUTCLK
    HW_MCG_C1_CLR(MCG_C1_CLKS_MASK);
    // select external RTC for FLL
    HW_MCG_C1_CLR(MCG_C1_IREFS_MASK);
    // FLLOUTCLK: 32768 * 2929 = 96000000
    HW_MCG_C4_SET(MCG_C4_DRST_DRS_MASK);
    HW_MCG_C4_SET(MCG_C4_DMX32_MASK);
    // wait for FLL
    while(BR_MCG_S_IREFST == 0);
    while(BR_MCG_S_CLKST != 0);
    /* Update system prescalers */
    BW_SIM_CLKDIV1_OUTDIV1(0);
    BW_SIM_CLKDIV1_OUTDIV2(1);
    BW_SIM_CLKDIV1_OUTDIV4(3);
    
    
    // USBCLK 96 / 2 = 48MHz
    HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK);
    BW_SIM_CLKDIV2_USBDIV(1);
#endif
    return 0;
}
/* EOF */
