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
 * @file RealTimerCounter.h
 *
 * @author
 *
 * @version
 *
 * @date    
 *
 * @brief   This is a header file for Real Time Counter (RTC)
 *****************************************************************************/

#ifndef _RTC_HEADER_H
#define _RTC_HEADER_H
/******************************************************************************
 * Includes
 *****************************************************************************/

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
#define ERR_SUCCESS             0
#define ERR_INVALID_PARAM       -1
#define ERR_TIMER_QUEUE_FULL    -2
#define INVALID_TIME_COUNT      0xFFFFFFFF
/******************************************************************************
 * Types
 *****************************************************************************/

/* Timer Callback Function Prototype */
#ifdef TIMER_CALLBACK_ARG
typedef void (*pfntimer_callback_t)(uint8_t index, void * arg);
#else
typedef void (*pfntimer_callback_t)(void);
#endif
/* Timer Object Structure */
typedef struct _timer_object
{
    uint32_t          ms_count;          /* msec Timeout Value */
    pfntimer_callback_t pfn_timer_callback; /* Callback Function */
#ifdef TIMER_CALLBACK_ARG
    void*             arg;              /* Callback Function Argument */
#endif
} timer_object_t;

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
extern uint8_t TimerQInitialize(uint8_t ControllerId);
extern uint8_t AddTimerQ(timer_object_t* pTimerObject, uint8_t *timer_index);
extern uint8_t RemoveTimerQ(uint8_t index);

#endif /* _RTC_HEADER_H */
