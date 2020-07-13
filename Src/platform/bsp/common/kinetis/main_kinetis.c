/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
**************************************************************************
*
* $FileName: main_kinetis.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#include <types.h>
#include "soc.h"
#include "bsp.h"
#include "derivative.h"
#include "hidef.h"
#include "user_config.h"

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

#if MAX_TIMER_OBJECTS
extern uint8_t TimerQInitialize(uint8_t ControllerId);
#endif
extern void Main_Task(uint32_t param);
extern void SYS_Init(void);
extern void sci_init(uint32_t channel);
extern void delay_init(void);
extern void POLL_init(void);
extern void APP_init(void);
extern void APP_task(void);
extern void Poll(void); 


/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/


/****************************************************************************
 * Global Variables
 ****************************************************************************/
volatile uint8_t kbi_stat;       /* Status of the Key Pressed */
#ifdef USE_FEEDBACK_ENDPOINT
extern uint32_t feedback_data;
#endif

/******************************************************************************
 * @name        main
 *
 * @brief       This routine is the starting point of the application
 *
 * @param       None
 *
 * @return      None
 *
 *****************************************************************************
 * This function initializes the system, enables the interrupts and calls the
 * application
 *****************************************************************************/
#if defined (__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    /* Initialize the system */
    SYS_Init();

#if PRINTF_ENABLE
    sci_init(SCI_CHANNEL);
#endif
#if MAX_TIMER_OBJECTS
    (void)TimerQInitialize(0);
#endif

#if DELAY_ENABLE
    delay_init();
#endif

#if TASK_ENABLE
    POLL_init(); 
#endif
    /* Initialize the USB Main task */
    APP_init();
   
    /*
    ** Infinite loop, waiting for events requiring action
    */
    for ( ; ; )
    {
        APP_task();
#if TASK_ENABLE
        Poll();
#endif
    } /* Endfor */;

#if defined(__GNUC__)
    return 0;
#endif
}

/* EOF */
