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


#define BSP_CLOCK_SRC       (8000000ul)                         /* crystal, oscillator freq. */
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

/*****************************************************************************
 * Local Variables
 *****************************************************************************/


/*****************************************************************************
 * Local function prototypes
 *****************************************************************************/
static int32_t pll_init(
    unsigned char init_osc, 
    unsigned char osc_select, 
    int32_t crystal_val, 
    unsigned char hgo_val, 
    unsigned char erefs_val, 
    unsigned char pll_select, 
    signed char prdiv_val, 
    signed char vdiv_val, 
    unsigned char mcgout_select
);
static void trace_clk_init(void);
static void fb_clk_init(void);

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
    int32_t mcg_clk_hz;
#if defined(SYNCH_MODE)
    int32_t pll_0_clk_khz;
#elif defined(ASYNCH_MODE)
    int32_t pll_1_clk_khz;
#endif

    /* Point the VTOR to the new copy of the vector table */
#if (defined(__CWCC__) || defined(__IAR_SYSTEMS_ICC__) || defined(__CC_ARM))
    SCB_VTOR = (uint32_t)___VECTOR_RAM;
#elif defined(__arm__)
  //SCB_VTOR = (uint32_t)__VECTOR_RAM;
#elif defined(__GNUC__)
    SCB_VTOR = (uint32_t)__cs3_interrupt_vector;
#endif
    /*
     * Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTA_MASK);
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTB_MASK);
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTC_MASK);
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTD_MASK);
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTE_MASK);
    HW_SIM_SCGC5_SET(SIM_SCGC5_PORTF_MASK);

    /* releases hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
     * if ACKISO is set you must clear ackiso before calling pll_init 
     * or pll init hangs waiting for OSC to initialize
     * if osc enabled in low power modes - enable it first before ack
     * if I/O needs to be maintained without glitches enable outputs and modules first before ack.*/
    if (BR_PMC_REGSC_ACKISO)
        HW_PMC_REGSC_SET(PMC_REGSC_ACKISO_MASK);

#if defined(NO_PLL_INIT)
    mcg_clk_hz = 21000000; /* FEI mode */
#elif defined (ASYNCH_MODE)  
    /* Set the system dividers */
    /* NOTE: The PLL init will not configure the system clock dividers,
     * so they must be configured appropriately before calling the PLL
     * init function to ensure that clocks remain in valid ranges.
     */
    BW_SIM_CLKDIV1_OUTDIV1(0);
    BW_SIM_CLKDIV1_OUTDIV2(1);
    BW_SIM_CLKDIV1_OUTDIV3(1);
    BW_SIM_CLKDIV1_OUTDIV4(5);

    /* Initialize PLL0 */
    /* PLL0 will be the source for MCG CLKOUT so the core, system, FlexBus, and flash clocks are derived from it */ 
    mcg_clk_hz = pll_init(OSCINIT,   /* Initialize the oscillator circuit */
            OSC_0,     /* Use CLKIN0 as the input clock */
            CLK0_FREQ_HZ,  /* CLKIN0 frequency */
            LOW_POWER,     /* Set the oscillator for low power mode */
            CLK0_TYPE,     /* Crystal or canned oscillator clock input */
            PLL_0,         /* PLL to initialize, in this case PLL0 */
            PLL0_PRDIV,    /* PLL predivider value */
            PLL0_VDIV,     /* PLL multiplier */
            MCGOUT);       /* Use the output from this PLL as the MCGOUT */

    /* Check the value returned from pll_init() to make sure there wasn't an error */
    if (mcg_clk_hz < 0x100)
        while(1);

    /* Initialize PLL1 */
    /* PLL1 will be the source for the DDR controller, but NOT the MCGOUT */   
    pll_1_clk_khz = (pll_init(NO_OSCINIT, /* Don't init the osc circuit, already done */
            OSC_0,      /* Use CLKIN0 as the input clock */
            CLK0_FREQ_HZ,  /* CLKIN0 frequency */
            LOW_POWER,     /* Set the oscillator for low power mode */
            CLK0_TYPE,     /* Crystal or canned oscillator clock input */
            PLL_1,         /* PLL to initialize, in this case PLL1 */
            PLL1_PRDIV,    /* PLL predivider value */
            PLL1_VDIV,     /* PLL multiplier */
            PLL_ONLY) / 1000);   /* Don't use the output from this PLL as the MCGOUT */

    /* Check the value returned from pll_init() to make sure there wasn't an error */
    if ((pll_1_clk_khz * 1000) < 0x100)
        while(1);   

#elif defined (SYNCH_MODE)  
    /* Set the system dividers */
    /* NOTE: The PLL init will not configure the system clock dividers,
     * so they must be configured appropriately before calling the PLL
     * init function to ensure that clocks remain in valid ranges.
     */ 
    BW_SIM_CLKDIV1_OUTDIV1(0);
    BW_SIM_CLKDIV1_OUTDIV2(2);
    BW_SIM_CLKDIV1_OUTDIV3(2);
    BW_SIM_CLKDIV1_OUTDIV4(5);

    /* Initialize PLL1 */
    /* PLL1 will be the source MCGOUT and the DDR controller */   
    mcg_clk_hz = pll_init(OSCINIT, /* Don't init the osc circuit, already done */
            OSC_0,      /* Use CLKIN0 as the input clock */
            CLK0_FREQ_HZ,  /* CLKIN0 frequency */
            LOW_POWER,     /* Set the oscillator for low power mode */
            CLK0_TYPE,     /* Crystal or canned oscillator clock input */
            PLL_1,         /* PLL to initialize, in this case PLL1 */
            PLL1_PRDIV,    /* PLL predivider value */
            PLL1_VDIV,     /* PLL multiplier */
            MCGOUT);   /* Don't use the output from this PLL as the MCGOUT */

    /* Check the value returned from pll_init() to make sure there wasn't an error */
    if (mcg_clk_hz < 0x100)
        while(1);

    /* Initialize PLL0 */
    /* PLL0 is initialized, but not used as the MCGOUT */ 
    pll_0_clk_khz = (pll_init(NO_OSCINIT,   /* Initialize the oscillator circuit */
            OSC_0,     /* Use CLKIN0 as the input clock */
            CLK0_FREQ_HZ,  /* CLKIN0 frequency */
            LOW_POWER,     /* Set the oscillator for low power mode */
            CLK0_TYPE,     /* Crystal or canned oscillator clock input */
            PLL_0,         /* PLL to initialize, in this case PLL0 */
            PLL0_PRDIV,    /* PLL predivider value */
            PLL0_VDIV,     /* PLL multiplier */
            PLL_ONLY) / 1000);       /* Use the output from this PLL as the MCGOUT */

    /* Check the value returned from pll_init() to make sure there wasn't an error */
    if ((pll_0_clk_khz * 1000) < 0x100)
        while(1);

#else
#error "A PLL configuration for this platform is NOT defined"
#endif

    /* For debugging purposes, enable the trace clock and/or FB_CLK so that
     * we'll be able to monitor clocks and know the PLL is at the frequency
     * that we expect.
     */
    trace_clk_init();
    fb_clk_init();

    /* Initialize the DDR if the project option if defined */
#ifdef DDR_INIT
    twr_ddr2_script_init();
#endif

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
static int32_t pll_init(
    unsigned char init_osc, 
    unsigned char osc_select, 
    int32_t crystal_val, 
    unsigned char hgo_val, 
    unsigned char erefs_val, 
    unsigned char pll_select, 
    signed char prdiv_val, 
    signed char vdiv_val, 
    unsigned char mcgout_select
)
{
    unsigned char frdiv_val;
    unsigned char temp_reg;
    unsigned char prdiv, vdiv;
    short i;
    int32_t ref_freq;
    int32_t pll_freq;

    /* If using the PLL as MCG_OUT must check if the MCG is in FEI mode first */
    if (mcgout_select)
    {
        /* check if in FEI mode */
        if (!((BR_MCG_S_CLKST == 0x0) &&                            /* check CLKS mux has selcted FLL output */
                (BR_MCG_S_IREFST) &&                                /* check FLL ref is internal ref clk */
                (!BR_MCG_S_PLLST)))                                 /* check PLLS mux has selected FLL */
        {
            return 0x1;                                                         /* return error code */
        }
    } 
    /* if (mcgout_select)
     * Check if OSC1 is being used as a reference for the MCGOUT PLL
     * This requires a more complicated MCG configuration.
     * At this time (Sept 8th 2011) this driver does not support this option */
    if (osc_select && mcgout_select)
    {
        return 0x80; /* Driver does not support using OSC1 as the PLL reference for the system clock on MCGOUT */
    }

    /* check external frequency is less than the maximum frequency */
    if  (crystal_val > 60000000) {return 0x21;}

    /* check crystal frequency is within spec. if crystal osc is being used as PLL ref */
    if (erefs_val)
    {
        if ((crystal_val < 8000000) || (crystal_val > 32000000)) {return 0x22;} /* return 1 if one of the available crystal options is not available */
    }

    /* make sure HGO will never be greater than 1. Could return an error instead if desired. */
    if (hgo_val > 0)
    {
        hgo_val = 1; /* force hgo_val to 1 if > 0 */
    }

    /* Check PLL divider settings are within spec. */
    if ((prdiv_val < 1) || (prdiv_val > 8)) {return 0x41;}
    if ((vdiv_val < 16) || (vdiv_val > 47)) {return 0x42;}

    /* Check PLL reference clock frequency is within spec. */
    ref_freq = crystal_val / prdiv_val;
    if ((ref_freq < 8000000) || (ref_freq > 32000000)) {return 0x43;}

    /* Check PLL output frequency is within spec. */
    pll_freq = (crystal_val / prdiv_val) * vdiv_val;
    if ((pll_freq < 180000000) || (pll_freq > 360000000)) {return 0x45;}

    /* Determine if oscillator needs to be set up */
    if (init_osc)
    {
        /* Check if the oscillator needs to be configured */
        if (!osc_select)
        {
            /* configure the MCG_C2 register
             * the RANGE value is determined by the external frequency. Since the RANGE parameter affects the FRDIV divide value
             * it still needs to be set correctly even if the oscillator is not being used */
            temp_reg = HW_MCG_C2_RD();
            temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK); /* clear fields before writing new values */

            if (crystal_val <= 8000000)
            {
                temp_reg |= (BF_MCG_C2_IRCS(1) | BF_MCG_C2_HGO0(hgo_val) | BF_MCG_C2_EREFS0(erefs_val));
            }
            else
            {
                /* On rev. 1.0 of silicon there is an issue where the the input bufferd are enabled when JTAG is connected.
                 * This has the affect of sometimes preventing the oscillator from running. To keep the oscillator amplitude
                 * low, RANGE = 2 should not be used. This should be removed when fixed silicon is available. */
                temp_reg |= (BF_MCG_C2_RANGE0(1) | BF_MCG_C2_HGO0(hgo_val) | BF_MCG_C2_EREFS0(erefs_val));
            }
            HW_MCG_C2_WR(temp_reg);
        }
        else
        {
            /* configure the MCG_C10 register
             * the RANGE value is determined by the external frequency. Since the RANGE parameter affects the FRDIV divide value
             * it still needs to be set correctly even if the oscillator is not being used */
            temp_reg = MCG_C10;
            temp_reg &= ~(MCG_C10_RANGE1_MASK | MCG_C10_HGO1_MASK | MCG_C10_EREFS1_MASK); // clear fields before writing new values
            if (crystal_val <= 8000000)
            {
                temp_reg |= (BF_MCG_C10_RANGE1(1) | BF_MCG_C10_HGO1(hgo_val) | BF_MCG_C10_EREFS1(erefs_val));
            }
            else
            {
                /* On rev. 1.0 of silicon there is an issue where the the input bufferd are enabled when JTAG is connected.
                 * This has the affect of sometimes preventing the oscillator from running. To keep the oscillator amplitude
                 * low, RANGE = 2 should not be used. This should be removed when fixed silicon is available. */
                temp_reg |= (BF_MCG_C10_RANGE1(1) | BF_MCG_C10_HGO1(hgo_val) | BF_MCG_C10_EREFS1(erefs_val));
            }
            HW_MCG_C10_WR(temp_reg);
        } 
    }

    if (mcgout_select)
    {
        /* determine FRDIV based on reference clock frequency
         * since the external frequency has already been checked only the maximum frequency for each FRDIV value needs to be compared here. */
        if (crystal_val <= 1250000) {frdiv_val = 0;}
        else if (crystal_val <= 2500000) {frdiv_val = 1;}
        else if (crystal_val <= 5000000) {frdiv_val = 2;}
        else if (crystal_val <= 10000000) {frdiv_val = 3;}
        else if (crystal_val <= 20000000) {frdiv_val = 4;}
        else {frdiv_val = 5;}

        /* Select external oscillator and Reference Divider and clear IREFS to start ext osc
         * If IRCLK is required it must be enabled outside of this driver, existing state will be maintained
         * CLKS=2, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0 */
        temp_reg = HW_MCG_C1_RD();
        temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK); /* Clear values in these fields */
        temp_reg = BF_MCG_C1_CLKS(2) | BF_MCG_C1_FRDIV(frdiv_val); /* Set the required CLKS and FRDIV values */
        MCG_C1 = temp_reg;

        /* if the external oscillator is used need to wait for OSCINIT to set */
        if (erefs_val)
        {
            for (i = 0 ; i < 10000 ; i++)
            {
                if (BR_MCG_S_OSCINIT0) break; /* jump out early if OSCINIT sets before loop finishes */
            }
            if (!(BR_MCG_S_OSCINIT0)) return 0x23; /* check bit is really set and return with error if not set */
        }

        /* wait for Reference clock Status bit to clear */
        for (i = 0 ; i < 2000 ; i++)
        {
            if (!(BR_MCG_S_PLLST)) break; /* jump out early if IREFST clears before loop finishes */
        }
        if (BR_MCG_S_PLLST) return 0x11; /* check bit is really clear and return with error if not set */

        /* Wait for clock status bits to show clock source is ext ref clk */
        for (i = 0 ; i < 2000 ; i++)
        {
            if (BR_MCG_S_CLKST == 0x2) break; /* jump out early if CLKST shows EXT CLK slected before loop finishes */
        }
        if (BR_MCG_S_CLKST != 0x2) return 0x1A; /* check EXT CLK is really selected and return with error if not */

        /* Now in FBE
         * It is recommended that the clock monitor is enabled when using an external clock as the clock source/reference.
         * It is enabled here but can be removed if this is not required. */
        HW_MCG_C6_SET(MCG_C6_CME0_MASK);

        /* Select which PLL to enable */
        if (!pll_select)
        {
            /* Configure PLL0
             * Ensure OSC0 is selected as the reference clock */
            HW_MCG_C5_CLR(MCG_C5_PLLREFSEL0_MASK);

            /* Select PLL0 as the source of the PLLS mux */
            HW_MCG_C11_CLR(MCG_C11_PLLCS_MASK);

            /* Configure MCG_C5
             * If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed in here or in user code. */
            temp_reg = HW_MCG_C5_RD();
            temp_reg &= ~MCG_C5_PRDIV0_MASK;
            temp_reg |= BF_MCG_C5_PRDIV0(prdiv_val - 1);    //set PLL ref divider
            HW_MCG_C5_WR(temp_reg);

            /* Configure MCG_C6
             * The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk
             * The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6 */
            temp_reg = HW_MCG_C6_RD(); /* store present C6 value */
            temp_reg &= ~MCG_C6_VDIV0_MASK; /* clear VDIV settings */
            temp_reg |= MCG_C6_PLLS_MASK | BF_MCG_C6_VDIV0(vdiv_val - 16); /* write new VDIV and enable PLL */
            HW_MCG_C6_WR(temp_reg); /* update MCG_C6 */

            /* wait for PLLST status bit to set */
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S_PLLST) break; /* jump out early if PLLST sets before loop finishes */
            }
            if (!(BR_MCG_S_PLLST)) return 0x16; /* check bit is really set and return with error if not set */

            // Wait for LOCK bit to set
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S_LOCK0) break; /* jump out early if LOCK sets before loop finishes */
            }
            if (!(BR_MCG_S_LOCK0)) return 0x44; /* check bit is really set and return with error if not set */

            /* Use actual PLL settings to calculate PLL frequency */
            prdiv = (BR_MCG_C5_PRDIV0 + 1);
            vdiv = (BR_MCG_C6_VDIV0 + 16);
        }
        else
        {
            /* Configure PLL1
             * Ensure OSC0 is selected as the reference clock */
            HW_MCG_C11_CLR(MCG_C11_PLLREFSEL1_MASK);

            /* Select PLL1 as the source of the PLLS mux */
            HW_MCG_C11_SET(MCG_C11_PLLCS_MASK);
            /* Configure MCG_C11
             * If the PLL is to run in STOP mode then the PLLSTEN2 bit needs to be OR'ed in here or in user code. */
            temp_reg = HW_MCG_C11_RD();
            temp_reg &= ~MCG_C11_PRDIV1_MASK;
            temp_reg |= BF_MCG_C11_PRDIV1(prdiv_val - 1);    /* set PLL ref divider */
            HW_MCG_C11_WR(temp_reg);

            /* Configure MCG_C12
             * The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk
             * The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE2 bit in MCG_C12 */
            temp_reg = HW_MCG_C12_RD(); // store present C12 value
            temp_reg &= ~MCG_C12_VDIV1_MASK; /* clear VDIV settings */
            temp_reg |=  BF_MCG_C12_VDIV1(vdiv_val - 16); /* write new VDIV and enable PLL */
            HW_MCG_C12_WR(temp_reg); /* update MCG_C12 */

            /* Enable PLL by setting PLLS bit */
            HW_MCG_C6_SET(MCG_C6_PLLS_MASK);

            /* wait for PLLCST status bit to set */
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S2_PLLCST) break; /* jump out early if PLLST sets before loop finishes */
            }
            if (!(BR_MCG_S2_PLLCST)) return 0x17; /* check bit is really set and return with error if not set */

            /* wait for PLLST status bit to set */
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S_PLLST) break; /* jump out early if PLLST sets before loop finishes */
            }
            if (!(BR_MCG_S_PLLST)) return 0x16; /* check bit is really set and return with error if not set */

            /* Wait for LOCK bit to set */
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S2_LOCK1) break; /* jump out early if LOCK sets before loop finishes */
            }
            if (!(BR_MCG_S2_LOCK1)) return 0x44; /* check bit is really set and return with error if not set */

            /* Use actual PLL settings to calculate PLL frequency */
            prdiv = (BR_MCG_C11_PRDIV1 + 1);
            vdiv = (BR_MCG_C12_VDIV1 + 16);
        }

        HW_MCG_C1_CLR(MCG_C1_CLKS_MASK); /* clear CLKS to switch CLKS mux to select PLL as MCG_OUT */

        /* Wait for clock status bits to update */
        for (i = 0 ; i < 2000 ; i++)
        {
            if (BR_MCG_S_CLKST == 0x3) break; /* jump out early if CLKST = 3 before loop finishes */
        }
        if (BR_MCG_S_CLKST != 0x3) return 0x1B; /* check CLKST is set correctly and return with error if not */
    }
    else
    {
        /* Setup PLL for peripheral only use */
        if (pll_select)
        {
            if (osc_select)
            {
                HW_MCG_C11_SET(MCG_C11_PLLREFSEL1_MASK); /* Set select bit to choose OSC1 */
            }
            else
            {
                HW_MCG_C11_CLR(MCG_C11_PLLREFSEL1_MASK); /* Clear select bit to choose OSC0 */
            }
            /* Configure MCG_C11
             * If the PLL is to run in STOP mode then the PLLSTEN2 bit needs to be OR'ed in here or in user code. */
            temp_reg = HW_MCG_C11_RD();
            temp_reg &= ~MCG_C11_PRDIV1_MASK;
            temp_reg |= BF_MCG_C11_PRDIV1(prdiv_val - 1);/* set PLL ref divider */
            HW_MCG_C11_WR(temp_reg);

            /* Configure MCG_C12
             * The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE2 bit in MCG_C12 */
            temp_reg = HW_MCG_C12_RD(); /* store present C12 value */
            temp_reg &= ~MCG_C12_VDIV1_MASK; /* clear VDIV settings */
            temp_reg |=  BF_MCG_C12_VDIV1(vdiv_val - 16); /* write new VDIV and enable PLL */
            HW_MCG_C12_WR(temp_reg); /* update MCG_C12 */
            /* Now enable the PLL */
            HW_MCG_C11_SET(MCG_C11_PLLCLKEN1_MASK); /* Set PLLCLKEN2 to enable PLL1 */

            /* Wait for LOCK bit to set */
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S2_LOCK1) break; /* jump out early if LOCK sets before loop finishes */
            }
            if (!(BR_MCG_S2_LOCK1)) return 0x44; /* check bit is really set and return with error if not set */

            /* Use actual PLL settings to calculate PLL frequency */
            prdiv = (BR_MCG_C11_PRDIV1 + 1);
            vdiv = (BR_MCG_C12_VDIV1 + 16);
        }
        else
        {
            /* Setup and enable PLL0
             * Select ref source */
            if (osc_select)
            {
                HW_MCG_C5_SET(MCG_C5_PLLREFSEL0_MASK); /* Set select bit to choose OSC1 */
            }
            else
            {
                HW_MCG_C5_CLR(MCG_C5_PLLREFSEL0_MASK); /* Clear select bit to choose OSC0 */
            }
            /* Configure MCG_C5
             * If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed in here or in user code. */
            temp_reg = HW_MCG_C5_RD();
            temp_reg &= ~MCG_C5_PRDIV0_MASK;
            temp_reg |= BF_MCG_C5_PRDIV0(prdiv_val - 1); /* set PLL ref divider */
            HW_MCG_C5_WR(temp_reg);

            /* Configure MCG_C6
             * The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6 */
            temp_reg = HW_MCG_C6_RD(); /* store present C6 value */
            temp_reg &= ~MCG_C6_VDIV0_MASK; /* clear VDIV settings */
            temp_reg |=  BF_MCG_C6_VDIV0(vdiv_val - 16); /* write new VDIV and enable PLL */
            HW_MCG_C6_WR(temp_reg); /* update MCG_C6 */
            /* Now enable the PLL */
            HW_MCG_C5_SET(MCG_C5_PLLCLKEN0_MASK); /* Set PLLCLKEN to enable PLL0 */

            /* Wait for LOCK bit to set */
            for (i = 0 ; i < 2000 ; i++)
            {
                if (BR_MCG_S_LOCK0) break; /* jump out early if LOCK sets before loop finishes */
            }
            if (!(BR_MCG_S_LOCK0)) return 0x44; /* check bit is really set and return with error if not set */

            /* Use actual PLL settings to calculate PLL frequency */
            prdiv = (BR_MCG_C5_PRDIV0 + 1);
            vdiv = (BR_MCG_C6_VDIV0 + 16);
        }
    }
    return (((crystal_val / prdiv) * vdiv) / 2); /* MCGOUT equals PLL output frequency/2 */
    /* MCU_MK70F12 */
}

/*****************************************************************************
 * @name     trace_clk_init
 *
 * @brief:   Initialization of the MCU.
 *
 * @param  : None
 *
 * @return : None
 *****************************************************************************
 * This function is called to set the trace clock the core clock frequency
 ****************************************************************************/
void trace_clk_init(void)
{
    /* Set the trace clock to the core clock frequency */
    HW_SIM_SOPT2_SET(SIM_SOPT2_TRACECLKSEL_MASK);
    /* Enable the TRACE_CLKOUT pin function on PTF23 (alt6 function) */
    HW_PORT_PCRn_SET(HW_PORTF,23,6);
}

/*****************************************************************************
 * @name     fb_clk_init
 *
 * @brief:   Initialization of the MCU.
 *
 * @param  : None
 *
 * @return : None
 *****************************************************************************
 * This function is called to enable the clock to the Flexbus module
 ****************************************************************************/
void fb_clk_init(void)
{
    /* Enable the clock to the FlexBus module */
    HW_SIM_SCGC7_SET(SIM_SCGC7_FLEXBUS_MASK);

    /* Enable the FB_CLKOUT function on PTC3 (alt5 function) */
    HW_PORT_PCRn_SET(HW_PORTC,3,5);
}
/* EOF */

