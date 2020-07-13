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
* $FileName: soc_i2c.c
* $Version :
* $Date    :
*
* Comments: i2c driver for K22
*
*  
*
*END*************************************************************************/
#include "types.h"
#include "user_config.h"

#if (I2C_ENABLE)
#include "soc.h"
#include "soc_i2c.h"

#define ELEMENTS_OF(x) ( sizeof(x)/sizeof(x[0]) )

static uint32_t i2c_set_baudrate (uint32_t, uint32_t);
static const uint32_t BAUDRATE_MULT[] = { 1, 2, 4 };
static const uint32_t BAUDRATE_ICR[]= {  20, 22, 24, 26, 28, 30, 34, 40, 28, 32, 
    36, 40, 44, 48, 56, 68, 48, 56, 64, 72, 80, 88, 104, 128, 80, 96, 112, 128,
    144, 160, 192, 240, 160, 192, 224, 256, 288, 320, 384, 480, 320, 384, 448, 
    512, 576, 640, 768, 960, 640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 
    1536, 1792, 2048, 2304, 2560, 3072, 3840 };

uint32_t i2c_vectors[] =
{
    I2C0_IRQn + 16,  
    I2C1_IRQn + 16,
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Pause
* Returned Value   : None
* Comments         : Pause Routine
*    
*
*END*----------------------------------------------------------------------*/
void Pause(void)
{
    volatile int n;
    for(n=1; n<200; n++) 
    {
        #if (defined(__CWCC__) || defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__))
        asm("nop");
        #elif defined (__CC_ARM)
        __nop();
        #endif
    }
}
/*GLOBAL FUNCTIONS*---------------------------------------------------------*/

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_get_vector
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector for specified I2C module.
*
*END*----------------------------------------------------------------------*/
uint32_t i2c_get_vector
(
    /* [IN] I2C channel */
    uint8_t channel
)
{
    if (channel < ELEMENTS_OF(i2c_vectors)) 
    {
        return i2c_vectors[channel];
    } 
    else 
    {
        return 0;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_Init
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_Init
(
    uint8_t i2c_channel
)
{
    switch (i2c_channel)
    {
    case 0:
        HW_SIM_SCGC4_SET(SIM_SCGC4_I2C0_MASK);
        /* configure GPIO for I2C function */
        BW_PORT_PCRn_MUX(HW_PORTE, 24, 5); /* GPIO mux */
        HW_PORT_PCRn_SET(HW_PORTE, 24, PORT_PCR_ODE_MASK);
        BW_PORT_PCRn_MUX(HW_PORTE, 25, 5); /* GPIO mux */
        HW_PORT_PCRn_SET(HW_PORTE, 25, PORT_PCR_ODE_MASK);
        break;
    case 1:
        HW_SIM_SCGC4_SET(SIM_SCGC4_I2C1_MASK);
        /* configure GPIO for I2C function */
        BW_PORT_PCRn_MUX(HW_PORTC, 10, 2); /* GPIO mux */
        BW_PORT_PCRn_MUX(HW_PORTC, 11, 2); /* GPIO mux */

        break;
    default:
        /* Do nothing if bad dev_num was selected */
        break;
    }

    /* Disable and clear I2C before initializing it */
    HW_I2C_C1_WR(i2c_channel, 0);

    /* Clear out all I2C events */
    HW_I2C_S_WR(i2c_channel, 0);

    /* Set the frequency divider for the nearest found baud rate */
    HW_I2C_F_WR(i2c_channel, i2c_set_baudrate( BUS_CLK, 100000 ));
    /* Enable I2C */
    HW_I2C_C1_SET(i2c_channel, I2C_C1_IICEN_MASK);
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_SetTXMode
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_SetTXMode
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_SET(i2c_channel, I2C_C1_TX_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_SetRXMode
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_SetRXMode
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_CLR(i2c_channel, I2C_C1_TX_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_SetSlaveMode
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_SetSlaveMode
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_CLR(i2c_channel, I2C_C1_MST_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_SetMasterMode
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_SetMasterMode
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_SET(i2c_channel, I2C_C1_MST_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_WriteByte
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_WriteByte
(
    uint8_t i2c_channel, 
    uint8_t data
)
{
    BW_I2C_D_DATA(i2c_channel, data);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_ReadByte
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
uint8_t i2c_ReadByte
(
    uint8_t i2c_channel
)
{
    return BR_I2C_D_DATA(i2c_channel);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_Wait
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_Wait
(
    uint8_t i2c_channel
)
{
    while(!(BR_I2C_S_IICIF(i2c_channel))) {};
    HW_I2C_S_SET(i2c_channel, I2C_S_IICIF_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_Start
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_Start
(
    uint8_t i2c_channel
)
{
    i2c_SetMasterMode(i2c_channel);
    i2c_SetTXMode(i2c_channel);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_Stop
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_Stop
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_CLR(i2c_channel, I2C_C1_MST_MASK);
    HW_I2C_C1_CLR(i2c_channel, I2C_C1_TX_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_RepeatedStart
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_RepeatedStart
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_SET(i2c_channel, I2C_C1_RSTA_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_GetACK
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
bool i2c_GetACK
(
    uint8_t i2c_channel
)
{
    if (!(BR_I2C_S_RXAK(i2c_channel)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_GiveACK
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_GiveACK
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_CLR(i2c_channel, I2C_C1_TXAK_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2c_GiveNACK
* Returned Value   : none
* Comments         :
* 
*
*END*----------------------------------------------------------------------*/
void i2c_GiveNACK
(
    uint8_t i2c_channel
)
{
    HW_I2C_C1_SET(i2c_channel, I2C_C1_TXAK_MASK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : i2c_set_baudrate
* Returned Value   : uint32_t divider register setting
* Comments         :
*    Find closest setting of divider register for given baudrate.
*
*END*********************************************************************/

static uint32_t i2c_set_baudrate 
    (
        /* [IN] Module input clock in Hz */
        uint32_t clock, 
        
        /* [IN] Desired baudrate in Hz */
        uint32_t baudrate
    ) 
{
    #define MAX_ICR 64
    uint32_t mult, icr, min, minmult = 0, minicr = 0;
    int32_t  val;
    
    min = (uint32_t)-1;
    /* We will find the nearest smallest diference in desired and real baudrate.
    ** This is transformed to find smallest clock diference.
    ** IIC baud rate = bus speed (Hz)/(mul * SCL divider) =>
    ** IIC baud rate * mul * SCL divider <= clock
    */
    for (mult = 0; mult < 3; mult++) 
    {
        for (icr = 0; icr < MAX_ICR; icr++) 
        {
            val = BAUDRATE_MULT[mult] * BAUDRATE_ICR[icr] * baudrate - clock;
            if (val < 0) val = -val;
            if (min > val) 
            {
                min = val;
                minmult = mult;
                minicr = icr;
            }
        }
    }
    return I2C_F_MULT(minmult) | I2C_F_ICR(minicr);
}

#endif /* I2C_ENABLE */
/* EOF */
