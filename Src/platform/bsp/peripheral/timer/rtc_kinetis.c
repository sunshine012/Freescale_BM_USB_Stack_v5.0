/**HEADER********************************************************************
* 
* Copyright (c) 2004-2010, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
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
 **************************************************************************//*!
 *
 * @file RealTimerCounter.c
 *
 * @author
 *
 * @version
 *
 * @date    
 *
 * @brief   This file configures Real Time Counter (RTC) for Timer 
 *          Implementation. It doesn't use the Kinetis RTC (Real Time Clock) 
 *          module.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"          /* Contains User Defined Data Types */
#include "soc.h"
#include "user_config.h"
#include "derivative.h"     /* include peripheral declarations */
#include "bsp.h"
#include "soc_pit.h"
#include "soc_isr.h"
#include "rtc_kinetis.h"
#include "adapter.h"

#if MAX_TIMER_OBJECTS
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static uint8_t TimerInit(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Array of Timer Objects */
timer_object_t g_TimerObjectArray[MAX_TIMER_OBJECTS];
uint32_t ms_count[MAX_TIMER_OBJECTS];
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
uint8_t TimerQInitialize(uint8_t controller_ID);
uint8_t AddTimerQ(timer_object_t* pTimerObject, uint8_t *timer_index);
uint8_t RemoveTimerQ(uint8_t index);
void real_timer_isr(void*);

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/******************************************************************************
 *
 *   @name        TimerQInitialize
 *
 *   @brief       Initializes RTC, Timer Object Queue and System Clock Counter
 *
 *   @param       controller_ID    : Controller ID
 *
 *   @return      None
 *****************************************************************************
 * This function initializes System Clock Counter, Timer Queue and Initializes
 * System Timer
 *****************************************************************************/
uint8_t TimerQInitialize(uint8_t controller_ID)
{
    UNUSED (controller_ID)
    (void)memset(g_TimerObjectArray, (int32_t)NULL, sizeof(g_TimerObjectArray));
    return TimerInit();
}

/******************************************************************************
 *
 *   @name        AddTimerQ
 *
 *   @brief       Adds Timer Object to Timer Queue
 *
 *     @param       pTimerObject    : Pointer to Timer Object
 *
 *   @return      None
 *****************************************************************************
 * Adds Timer Object to Timer Queue
 *****************************************************************************/
uint8_t AddTimerQ(
    timer_object_t*   pTimerObject,
    uint8_t           *timer_index
)
{
    uint8_t index;
    if(pTimerObject == NULL)
        return (uint8_t)ERR_INVALID_PARAM;
    if(pTimerObject->ms_count == (uint32_t)INVALID_TIME_COUNT)
        return (uint8_t)ERR_INVALID_PARAM;
    
    for(index = 0; index < MAX_TIMER_OBJECTS; index++)
    {
        /* Disable Timer Interrupts */
        pit_mask_int(0,1);
        if (g_TimerObjectArray[index].pfn_timer_callback == NULL)
        {
            (void)memcpy(&g_TimerObjectArray[index], pTimerObject, sizeof(timer_object_t)); 
            if (NULL != timer_index)
            {
                *timer_index = index;
            }
            /* Enable Timer Interrupts */
            pit_unmask_int(0,1);
            ms_count[index] = g_TimerObjectArray[index].ms_count;
            break;
        }
      /* Enable Timer Interrupts */
        pit_unmask_int(0,1);
    }
    if(index == MAX_TIMER_OBJECTS)
        return (uint8_t)ERR_TIMER_QUEUE_FULL;
    return index;
}

/******************************************************************************
 *
 *   @name        RemoveTimerQ
 *
 *   @brief       Removes Timer Object from Timer Queue
 *
 *     @param       index    : Index of Timer Object
 *
 *   @return      None
 *****************************************************************************
 * Removes Timer Object from Timer Queue
 *****************************************************************************/
uint8_t RemoveTimerQ(uint8_t index)
{
    if(index >= MAX_TIMER_OBJECTS)
        return (uint8_t)ERR_INVALID_PARAM;
    /* Disable Timer Interrupts */
    pit_mask_int(0,1);
    (void)memset(&g_TimerObjectArray[index], (int32_t)NULL, sizeof(timer_object_t));
    /* Enable Timer Interrupts */
    pit_unmask_int(0,1);
    return (uint8_t)ERR_SUCCESS;
}

/******************************************************************************
 *   @name        TimerInit
 *
 *   @brief       This is RTC initialization function
 *
 *   @return      None
 *
 ******************************************************************************
 * Initializes the RTC module registers
 *****************************************************************************/
static uint8_t TimerInit(void)
{   
    uint32_t vector   = pit_get_vector(1);
    if((PIT_OK != pit_init_freq(0,1,1000,BUS_CLK,FALSE))||(0 == vector))
        return (uint8_t)PIT_INVALID_PARAM;
        /* Install the timer interrupt handler */
    soc_install_isr(vector, real_timer_isr, NULL);
    bm_int_init(vector,0,TRUE);
    return (uint8_t)ERR_SUCCESS;
}

/******************************************************************************
 *   @name        real_timer_isr
 *
 *   @brief       This routine services RTC Interrupt
 *
 *     @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * Services RTC Interrupt. If a Timer Object expires, then removes the object 
 * from Timer Queue and Calls the callback function (if registered)
 *****************************************************************************/
void real_timer_isr(void* param)
{
    uint8_t index;
    if(TRUE == pit_check_int_flag(0,1))
    {
        /* Clear PIT Interrupt */
        pit_clear_int(0,1);
        /* Call Pending Timer CallBacks */
        for (index = 0; index < MAX_TIMER_OBJECTS; index++)
        {
            timer_object_t* ptemp = &g_TimerObjectArray[index];
            if(ptemp->pfn_timer_callback == NULL)
            {
                continue;
            }
            ms_count[index] --;
            if (ms_count[index] <= 0)
            {

                pfntimer_callback_t pfnTimerCallback = ptemp->pfn_timer_callback;
#ifdef TIMER_CALLBACK_ARG
                void *parg = ptemp->arg;
#endif
          //  (void)RemoveTimerQ(index);
#ifdef TIMER_CALLBACK_ARG
                pfnTimerCallback(parg);
#else
                pfnTimerCallback();
#endif
                ms_count[index] = g_TimerObjectArray[index].ms_count;
            }
        }
    }
}
#endif
