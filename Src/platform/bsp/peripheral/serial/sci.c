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
* $FileName: sci.c
* $Version :
* $Date    :
*
* Comments: sci driver for K22
*
*  
*
*END*************************************************************************/
#include "types.h"
#include "user_config.h"

#if (PRINTF_ENABLE)
#include "bsp.h"
#include "soc_sci.h"
#include "derivative.h"

#define UART_BAUDRATE 115200

UART_MemMapPtr g_sci_ptr = NULL;
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : sci_init
* Returned Value   :
* Comments         : This function initilizes the SCI baud rate.
*    
*
*END*----------------------------------------------------------------------*/
uint32_t sci_init(uint32_t channel) 
{
    register uint16_t   ubd;

    /* Enable all of the port clocks. These have to be enabled to configure
    * pin muxing options, so most code will need all of these on anyway.
    */
    g_sci_ptr = _bsp_get_serial_base_address(channel);
    if (g_sci_ptr == NULL)
    {
        return 1;
    }

    bsp_serial_io_init(channel);
    
    g_sci_ptr->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    g_sci_ptr->C2 = 0;

    ubd = (uint16_t)((SCI_CLK)/(UART_BAUDRATE * 16));
    
    g_sci_ptr->BDH = (uint8_t)((ubd >> 8) & UART_BDH_SBR_MASK);
    g_sci_ptr->BDL = (uint8_t)(ubd & UART_BDL_SBR_MASK);

    g_sci_ptr->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
    
    return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_PutChar
* Returned Value   :
* Comments         :
*                     This function sends a char via SCI.
*
*END*----------------------------------------------------------------------*/
#ifdef __CC_ARM
int32_t sendchar (int32_t ch)
#else
void uart_putchar (int8_t ch)
#endif
{
    /* Wait until space is available in the FIFO */
    while (!(g_sci_ptr->S1 & UART_S1_TDRE_MASK)){};

    /* Send the character */
    g_sci_ptr->D = (uint8_t)ch;
#ifdef __CC_ARM
    return 0;
#endif    
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_GetChar
* Returned Value   : the char get via SCI
* Comments         :
*                     This function gets a char via SCI.
*
*END*----------------------------------------------------------------------*/
#ifdef __CC_ARM
int32_t getkey(void)
#else
char uart_getchar(void)
#endif
{
    while (!(g_sci_ptr->S1 & UART_S1_RDRF_MASK));

    /* Return the 8-bit data from the receiver */
    return g_sci_ptr->D; 
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : uart_getchar_no_block
* Returned Value   : TRUE if char available, else FALSE
* Comments         :
*                     This function gets a char via SCI.
*
*END*----------------------------------------------------------------------*/
bool uart_getchar_no_block(int8_t* c)
{
    /* Wait until character has been received */
    if (!(g_sci_ptr->S1 & UART_S1_RDRF_MASK))
    {
        return FALSE;
    }
    else
    {
        *c = g_sci_ptr->D;
        /* Return the 8-bit data from the receiver */
        return TRUE;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : uart_sendchar_no_block
* Returned Value   : TRUE if char sent to tx fifo successfully, else FALSE
* Comments         :
*                     This function sends a char via SCI.
*
*END*----------------------------------------------------------------------*/
bool uart_sendchar_no_block (char ch)
{
    /* Check if space is available in the FIFO */
    if(!(g_sci_ptr->S1 & UART_S1_TDRE_MASK))
    {
        return FALSE;
    }
    else
    {
        /* Send the character */
        g_sci_ptr->D = (uint8_t)ch;
        return TRUE;
    }
}

/********************************************************************/
void out_char (char ch)
{
#ifndef __CC_ARM
    uart_putchar(ch);
#else
    (int32_t)sendchar(ch);
#endif
}
#endif
/* EOF */
