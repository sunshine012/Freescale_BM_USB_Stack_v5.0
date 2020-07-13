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
* $FileName: soc_pit.c
* $Version :
* $Date    :
*
* Comments:
*
*  
*
*END*************************************************************************/
#include "types.h"
#include "user_config.h"
#if (DELAY_ENABLE) || (MAX_TIMER_OBJECTS)
#include "soc.h"
#include "soc_pit.h"
#include "adapter.h"
#define ELEMENTS_OF(x) ( sizeof(x)/sizeof(x[0]) )
static const void* pit_address[] =
{
    (void*)REGS_PIT_BASE
};

uint32_t pit_vectors[] =
{
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    INT_PIT0,
    INT_PIT1,
    INT_PIT2,
    INT_PIT3,
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    PIT0_IRQn,
    PIT1_IRQn,
    PIT2_IRQn,
    PIT3_IRQn,
#endif
};

/*GLOBAL FUNCTIONS*---------------------------------------------------------*/
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_get_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding PIT
*    module.
*
*END*----------------------------------------------------------------------*/
void* pit_get_base_address
(
    /* [IN] PIT index */
    uint8_t dev_num
)
{
    if (dev_num < ELEMENTS_OF(pit_address)) 
    {
        return (void*)pit_address[dev_num];
    }
    return NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_get_vector
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector for specified PIT module.
*
*END*----------------------------------------------------------------------*/
uint32_t pit_get_vector
(
    /* [IN] PIT channel */
    uint8_t channel
)
{
    if (channel < ELEMENTS_OF(pit_vectors)) 
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        return pit_vectors[channel];
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        return pit_vectors[channel] + 16;
#endif
    } 
    else 
    {
        return 0;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_init_freq
* Returned Value   : None
* Comments         : This function initializes the PIT frequence
*
*
*END*----------------------------------------------------------------------*/
int8_t pit_init_freq
(
    /* [IN] PIT index */
    uint8_t timer,
    /* [IN] Timer channel */
    uint8_t channel,
    /* [IN] Ticks per second */
    uint32_t tickfreq,
    /* [IN] Clock speed in Hz */
    uint32_t clk,
    /* [IN] Unmask the timer after initializing */
    bool unmask_timer
)
{
    /* Enable PIT Module Clock */
    HW_SIM_SCGC6_SET(SIM_SCGC6_PIT_MASK);
    /* Enable PIT module */
    HW_PIT_MCR_CLR(PIT_MCR_FRZ_MASK | PIT_MCR_MDIS_MASK);
    /* Set counter reload value and counter value */
    HW_PIT_LDVALn_WR(channel, (uint16_t)(clk/tickfreq));
    /* Clear interrupt flag */
    HW_PIT_TFLGn_SET(channel, PIT_TFLG_TIF_MASK);
    if (unmask_timer)
    {
        /* Enable Timer Interrupt */
        pit_unmask_int(timer,channel);
    }
    return PIT_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_mask_int
* Returned Value   : None
* Comments         : This function mask interrupt of given timer
*
*
*END*----------------------------------------------------------------------*/
void pit_mask_int
(
    /* [IN] Timer to use */
    uint8_t timer,
    /* [IN] Timer channel */
    uint8_t channel
)
{
    HW_PIT_TCTRLn_CLR(channel, PIT_TCTRL_TEN_MASK);
    HW_PIT_TCTRLn_CLR(channel, PIT_TCTRL_TIE_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_unmask_int
* Returned Value   : None
* Comments         : This function unmask interrupt of given timer
*
*
*END*----------------------------------------------------------------------*/
void pit_unmask_int
(
    /* [IN] Timer to use */
    uint8_t timer,
    /* [IN] Timer channel */
    uint8_t channel
)
{
    HW_PIT_TCTRLn_SET(channel, PIT_TCTRL_TEN_MASK);
    HW_PIT_TCTRLn_SET(channel, PIT_TCTRL_TIE_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_check_int_flag
* Returned Value   : TRUE or FALSE
* Comments         : This function checks timer interrupt flag
*
*
*END*----------------------------------------------------------------------*/
bool pit_check_int_flag
(
    /* [IN] Timer to use */
    uint8_t timer,
    /* [IN] Timer channel */
    uint8_t channel
)
{
    return (((uint32_t)(HW_PIT_TFLGn_RD(channel) & PIT_TFLG_TIF_MASK)) ? TRUE : FALSE);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : pit_clear_int
* Returned Value   : None
* Comments         : This function clear flag interrupt
*
*
*END*----------------------------------------------------------------------*/
void pit_clear_int
(
    /* [IN] Timer to use */
    uint8_t timer,
    /* [IN] Timer channel */
    uint8_t channel
)
{
    HW_PIT_TFLGn_SET(channel, PIT_TFLG_TIF_MASK);
}
/* EOF */
#endif
