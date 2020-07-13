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
* $FileName:    pit_delay.c
* $Version :
* $Date    :
*
* Comments:
*
*
*END************************************************************************/
#include "types.h"
#include "user_config.h"
#if DELAY_ENABLE
#include "bsp.h"
#include "soc.h"
#include "soc_pit.h"
#include "soc_isr.h"
#include "pit_delay.h"
#include "adapter.h"

volatile int32_t delay_count;
static void delay_isr(void*);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TimerInit
* Returned Value   :
* Comments         : Initialize timer module
*    
*
*END*----------------------------------------------------------------------*/
void delay_init(void) 
{ 
    uint32_t vector   = pit_get_vector(0);
    if ((PIT_OK != pit_init_freq(0,0,10000,BUS_CLK,FALSE))||(0 == vector))
    {
        return ;
    }
    /* Install the timer interrupt handler */
    soc_install_isr(vector, delay_isr, NULL);
    bm_int_init(vector,0,TRUE);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : delay_isr 
* Returned Value   :
* Comments         : Timer interrupt service routine
*    
*
*END*----------------------------------------------------------------------*/
void delay_isr(void* param)
{
    if (TRUE == pit_check_int_flag(0,0))
    {
        /* Clear PIT Interrupt */
        pit_clear_int(0,0);
        if (0 < delay_count)
        {
            delay_count--;
        }
        if (delay_count <= 0)
        {
            pit_mask_int(0,0);   
        }
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : time_delay 
* Returned Value   :
* Comments         : Wait until interrupt of timer occur
*    
*
*END*----------------------------------------------------------------------*/
void time_delay(uint32_t delay) 
{
    delay_count = 10*delay;
    pit_unmask_int(0,0);
    while(1)
    {    
        if (0 >= delay_count)
        {
            break;
        }
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : delay 
* Returned Value   :
* Comments         : Wait until interrupt of timer occur
*    
*
*END*----------------------------------------------------------------------*/
void delay(uint32_t delay) 
{
    delay_count = delay;
    pit_unmask_int(0,0);
    while(1)
    {    
        if (0 == delay_count)
        {
            break;
        }
    }
}
/* EOF */
#endif
