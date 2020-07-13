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
* $FileName: soc_i2s.c
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

#if (I2S_ENABLE)
#include "soc.h"
#include "soc_i2s.h"

#define ELEMENTS_OF(x) ( sizeof(x)/sizeof(x[0]) )

uint32_t i2s_vectors[] =
{
    I2S0_Tx_IRQn + 16,
    I2S0_Rx_IRQn + 16
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : i2s_get_vector
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector for specified PIT module.
*
*END*----------------------------------------------------------------------*/
uint32_t i2s_get_vector
(
    /* [IN] I2S channel */
    uint8_t channel
)
{
    if (channel < ELEMENTS_OF(i2s_vectors)) 
    {
        return i2s_vectors[channel];
    } 
    else 
    {
        return 0;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : sai_format_in_data
* Returned Value   : formatted input data
* Comments         : Function performs SAI input data formatting
*
*END*********************************************************************/
uint32_t sai_format_in_data
(
    /* [IN] data to format */
    uint8_t* input,

    /* [IN] Requested format */
    uint8_t  size
)
{/* Body */
    int32_t         retval = 0;
    uint8_t         i;

    for (i = 0; i < size; i++)
    {
        retval |= ((uint32_t) *(input + i) << i * 8);
    }

    if (retval & (1 << ((size * 8) - 1)))
    {
        switch (size)
        {
        case 1:
            retval |= 0xFFFFFF00;
            break;
        case 2:
            retval |= 0xFFFF0000;
            break;
        case 3:
            retval |= 0xFF000000;
        default:
            break;
        }
    }
    return(~((unsigned) retval));
}/* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : sai_init
* Returned Value   : 
* Comments         :
*    This function initializes SAI device.
*
*END*********************************************************************/
void sai_init(ksai_info_struct_t* ksai_init_ptr, uint8_t flags, uint32_t fs_freq, uint16_t clk_mult, uint8_t channels)
{
    uint8_t             channel     = ksai_init_ptr->hw_channel;
    uint32_t            vector      = i2s_get_vector(channel);    
    uint32_t            mclk_freq   = 0;
    uint8_t             data_bits   = ksai_init_ptr->data_bits;
    
    /* SAI board specific IO pins initialization */
    sai_io_init(channel);
    
    /* SAI board specific registers initialization */
    sai_hw_init(ksai_init_ptr, flags);
    
    sai_set_master_mode(ksai_init_ptr, flags);
    
    sai_set_io_data_format(ksai_init_ptr, flags, channels);
    
    mclk_freq = sai_set_mclk_freq(channel, fs_freq, clk_mult);
    
    sai_set_data_bits(ksai_init_ptr, data_bits, flags);
    
    sai_set_fs_freq(ksai_init_ptr, flags, mclk_freq, fs_freq);

    soc_install_isr(vector, sai_tx_irq_handler, NULL);
    bm_int_init(vector, 5, TRUE);
    
} /* End body */

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : sai_io_init
 * Returned Value   : 
 * Comments         :
 *    This function performs BSP-specific initialization related to SAI
 *
 *END*----------------------------------------------------------------------*/
void sai_io_init( uint32_t dev_num )
{
    switch(dev_num)
    {
        case 0:
            /* Enable system clock to the I2S module */
            /* Enable SAI clock gate control & DMAMUX0 clock gate controlntrol */
            HW_SIM_SCGC6_SET(SIM_SCGC6_I2S_MASK);
            
            HW_SIM_SCGC5_SET(SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK);
            /* Enable SSI pins */
            BW_PORT_PCRn_MUX(HW_GPIOC, 6, 6);
            
            /* GPIO for SSI0_BLCK */
            BW_PORT_PCRn_MUX(HW_GPIOB, 18, 4);                  /* Configure Port for TX Bit Clock */
            
            /* GPIO for SSI0_FS */
            BW_PORT_PCRn_MUX(HW_GPIOB, 19, 4);                  /* Configure port for TX Frame Sync */
            
            /* GPIO for SSI0_XD */
            BW_PORT_PCRn_MUX(HW_GPIOC, 1, 6);                  /* Configure port for TX Data */
            BW_PORT_PCRn_MUX(HW_GPIOC, 5, 4);                   /* Configure port for RX Data */
            break;
        default:
            break;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : sai_hw_init
* Returned Value   : 
* Comments         :
*    This function initializes an SAI hardware control registers.
*
*END*********************************************************************/
void sai_hw_init(ksai_info_struct_t* ksai_init_ptr, uint32_t io_mode)
{  
    uint8_t         channel = ksai_init_ptr->hw_channel;
    uint8_t         data_bits  = ksai_init_ptr->data_bits;
    uint8_t         i2s_mode   = ksai_init_ptr->mode;
    uint8_t         clk_source = ksai_init_ptr->clock_source;
    uint8_t         bclk_mode  = ksai_init_ptr->clock_mode;
    uint16_t        fract = 0;
    uint16_t        div = 0;
    bool            set_tx = io_mode & I2S_IO_WRITE;
    bool            set_rx = io_mode & I2S_IO_READ;
    
    /* 
    ** Set input clock of the divider to system clock
    */
    HW_I2S_MCR_CLR(channel, I2S_MCR_MICS_MASK);
    
    /* ================ General transmitter configuration =================== */ 
    if (set_tx)
    {
        uint8_t tx_channel = ksai_init_ptr->tx_channel;
        /* 
        ** Software reset -> reset internal transmitter logic including the FIFO writer
        */
        HW_I2S_TCSR_WR(channel, 0);
        HW_I2S_TCSR_SET(channel, I2S_TCSR_SR_MASK);
        HW_I2S_TCSR_CLR(channel, I2S_TCSR_SR_MASK);
        /*
        ** Enable transmitter in debug mode - this must be done else synchronous mode 
        ** will not work while debugging.
        */
        HW_I2S_TCSR_SET(channel, I2S_TCSR_DBGE_MASK);
        /* 
        ** Set FIFO watermark
        */
        BW_I2S_TCR1_TFW(channel, I2S_TX_FIFO_DEFAULT_WATERMARK);
        /* 
        ** Set the synch mode, clock polarity, master clock source and bit clock 
        */
  
        HW_I2S_TCR2_WR(channel, 0);
        HW_I2S_TCR2_SET(channel, I2S_TCR2_BCP_MASK);    /* Bit clock active low           */
        BW_I2S_TCR2_MSEL(channel, 1);                   /* MCLK from divider (CLKGEN)     */
        BW_I2S_TCR2_DIV(channel, 3);                    /* Bit clock frequency = MCLK / 8 */
        /*
        ** First word in frame sets start of word flag
        */
        HW_I2S_TCR3_WR(channel, 0);
        BW_I2S_TCR3_WDFL(channel, 0);
        /* 
        ** Enable selected transmitter channel
        */
        switch (tx_channel)
        {
            case 0:
                BW_I2S_TCR3_TCE(channel,1);
                break;
            case 1:
                BW_I2S_TCR3_TCE(channel,2);
                break;
            default:
                break;
        }
        /*
        ** Configure frame size, sync width, MSB and frame sync
        */
        HW_I2S_TCR4_WR(channel, 0);
        BW_I2S_TCR4_FRSZ(channel, 1);                   /* Two words in each frame          */
        BW_I2S_TCR4_SYWD(channel, data_bits - 1);       /* Synch width as long as data word */
        HW_I2S_TCR4_SET(channel, I2S_TCR4_MF_MASK);     /* MSB shifted out first            */
        HW_I2S_TCR4_SET(channel, I2S_TCR4_FSE_MASK);    /* Frame sync one bit early         */
        /* 
        ** Fist bit shifted is always MSB (bit with highest index in register)
        ** and word length is set according to user configuration.
        */
        HW_I2S_TCR5_WR(channel, 0);
        BW_I2S_TCR5_WNW(channel, data_bits - 1);    /* Word length for Nth word is data_bits   */
        BW_I2S_TCR5_W0W(channel, data_bits - 1);    /* Word length for 0th word is data_bits   */
        BW_I2S_TCR5_FBT(channel, data_bits);        /* First bit shifted is on data_bits index */
      
        /*
        ** Reset word mask 
        */
        HW_I2S_TMR_WR(channel, 0);
    }
    
    /* ================== General receiver configuration ==================== */  
    if (set_rx)
    {
        uint8_t rx_channel = ksai_init_ptr->rx_channel;
        /* 
        ** Software reset -> reset internal receiver logic including the FIFO writer
        */
        HW_I2S_RCSR_WR(channel, 0);
        HW_I2S_RCSR_SET(channel, I2S_RCSR_SR_MASK);
        HW_I2S_RCSR_CLR(channel, I2S_RCSR_SR_MASK);
        
        /*
        ** Enable receiver in debug mode - this must be done else synchronous mode 
        ** will not work while debugging.
        */
        HW_I2S_RCSR_SET(channel, I2S_RCSR_DBGE_MASK);
        /* 
        ** Set FIFO watermark
        */
        BW_I2S_RCR1_RFW(channel, I2S_RX_FIFO_DEFAULT_WATERMARK);
        /* 
        ** Set the clock polarity and master clock source 
        */
        HW_I2S_RCR2_WR(channel, 0);
        HW_I2S_RCR2_SET(channel, I2S_RCR2_BCP_MASK);                /* Bit clock active low           */
        BW_I2S_RCR2_MSEL(channel, 1);                               /* MCLK from divider (CLKGEN)     */
        BW_I2S_RCR2_DIV(channel, (DEFAULT_BCLK_DIV / 2) - 1);       /* Bit clock frequency = MCLK / 8 */

        /*
        ** First word in frame sets start of word flag
        */
        BW_I2S_RCR3_WDFL(channel, 0);
        /* 
        ** Enable selected receiver channel
        */
        switch (rx_channel)
        {
            case 0:
                BW_I2S_RCR3_RCE(channel,1);
                break;
            case 1:
                BW_I2S_RCR3_RCE(channel,2);
                break;
            default:
                break;
        }
        /*
        ** Configure frame size, sync width, MSB and frame sync
        */
        HW_I2S_RCR4_WR(channel, 0);
        BW_I2S_RCR4_FRSZ(channel, 1);                   /* Two words in each frame          */
        BW_I2S_RCR4_SYWD(channel, data_bits - 1);       /* Synch width as long as data word */
        HW_I2S_RCR4_SET(channel, I2S_RCR4_MF_MASK);     /* MSB shifted out first            */
        HW_I2S_RCR4_SET(channel, I2S_RCR4_FSE_MASK);    /* Frame sync one bit early         */
   
        /* 
        ** Fist bit shifted is always MSB (bit with highest index in register)
        ** and word length is set according to user configuration.
        */
        HW_I2S_RCR5_WR(channel, 0);
        BW_I2S_RCR5_WNW(channel, data_bits - 1);    /* Word length for Nth word is data_bits   */
        BW_I2S_RCR5_W0W(channel, data_bits - 1);    /* Word length for 0th word is data_bits   */
        BW_I2S_RCR5_FBT(channel, data_bits);        /* First bit shifted is on data_bits index */
      
        /*
        ** Reset word mask 
        */
        HW_I2S_RMR_WR(channel, 0);
    }
    
    /*================= I2S clock source specific code =======================*/
    switch (clk_source)
    {
        case I2S_CLK_INT:
            /* 
            ** Set divider to get the 12.2880 MHz master clock
            */
            sai_find_mclk_freq_div(SYS_CLK, &fract, &div, 12288000);
            BW_I2S_MDR_DIVIDE(channel, div - 1);
            BW_I2S_MDR_FRACT(channel, fract - 1);
            while(BR_I2S_MCR_DUF(channel)) {}; /* wait until new value is set */

            /* 
            ** MCLK signal pin is configured as an output from the MCLK divider
            ** and the MCLK divider is enabled.
            */
            HW_I2S_MCR_SET(channel, I2S_MCR_MOE_MASK);
            while(BR_I2S_MCR_DUF(channel)) {};
            break;
            
        case I2S_CLK_EXT:
            /* 
            ** MCLK signal pin is configured as an input that bypasses
            ** the MCLK divider.
            */
            HW_I2S_MCR_CLR(channel, I2S_MCR_MOE_MASK);
            while(BR_I2S_MCR_DUF(channel)){};
            break;
        default:
            break;
    }
    /*================= I2S mode of operation specific code ===================*/
    if (i2s_mode & I2S_TX_MASTER)
    {
        /* 
        ** Transmit bit clock generated internally
        */
        HW_I2S_TCR2_SET(channel, I2S_TCR2_BCD_MASK);
        /*
        ** Transmit frame sync generated internally
        */
        HW_I2S_TCR4_SET(channel, I2S_TCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_RX_MASTER)
    {
        /*
        ** Receive bit clock generated internally
        */
        HW_I2S_RCR2_SET(channel, I2S_RCR2_BCD_MASK);
        /* 
        ** Receive frame sync generated internally
        */
        HW_I2S_RCR4_SET(channel, I2S_RCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_TX_SLAVE)
    {
        /*
        ** Transmit bit clock generated externally
        */
        HW_I2S_TCR2_CLR(channel, I2S_TCR2_BCD_MASK);
        /* 
        ** Transmit frame sync generated externally
        */
        HW_I2S_TCR4_CLR(channel, I2S_TCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_RX_SLAVE)
    {
        /* 
        ** Receive bit clock generated externally
        */
        HW_I2S_RCR2_CLR(channel, I2S_RCR2_BCD_MASK);
        /*
        ** Receive frame sync generated externally
        */
        HW_I2S_RCR4_CLR(channel, I2S_RCR4_FSD_MASK);
    }
    /*========================== I2S clock setup =============================*/
    sai_set_bclk_mode(channel, bclk_mode);
}

/*FUNCTION****************************************************************
*
* Function Name    : sai_set_bclk_mode
* Returned Value   : 0 if everything is OK.
* Comments         :
*       This function modifies SAI registers to set up synchronous mode and
*       clock swapping.
*
*END*********************************************************************/
bool sai_set_bclk_mode(uint8_t channel, uint8_t bclk_mode)
{
    if ((bclk_mode & I2S_TX_SYNCHRONOUS) && (bclk_mode & I2S_RX_SYNCHRONOUS))
    {
        /* Invalid combination of bclk modes */
        return 0;
    }
    /*
    ** Transmitter clock:
    */
    if (bclk_mode & I2S_TX_ASYNCHRONOUS) /* Transmitter is asynchronous */
    {
       HW_I2S_TCR2_CLR(channel, I2S_TCR2_SYNC_MASK);
    }
    else if (bclk_mode & I2S_TX_SYNCHRONOUS) /* Transmitter is synchronous */
    {
       /*
       ** If transmitter is synchronous, receiver must be asynchronous
       */
       if (HW_I2S_RCR2_RD(channel) & I2S_RCR2_SYNC_MASK)
        {
           /* Invalid combination of bclk modes */
           return 0;
        }
       BW_I2S_TCR2_SYNC(channel, 1);
    }

    if (bclk_mode & I2S_TX_BCLK_NORMAL) /* Transmitter BCLK not swapped */
    {
        HW_I2S_TCR2_CLR(channel, I2S_TCR2_BCS_MASK);
    }
    else if (bclk_mode & I2S_TX_BCLK_SWAPPED) /* Transmitter BCLK swapped */
    {
        HW_I2S_TCR2_SET(channel, I2S_TCR2_BCS_MASK);
        /*
        ** When Tx is synch. BCS bit must be set also for Rx.
        */
        if (bclk_mode & I2S_TX_SYNCHRONOUS) 
        {
            HW_I2S_RCR2_SET(channel, I2S_RCR2_BCS_MASK);
        }
    }
   
    /*
    ** Receiver clock:
    */
    if (bclk_mode & I2S_RX_ASYNCHRONOUS) /* Receiver is asynchronous */
    {
        HW_I2S_RCR2_CLR(channel, I2S_RCR2_SYNC_MASK);
    }
    else if (bclk_mode & I2S_RX_SYNCHRONOUS) /* Receiver is synchronous */
    {
        /*
        ** If receiver is synchronous, transmitter must be asynchronous
        */
        if (HW_I2S_TCR2_RD(channel) & I2S_TCR2_SYNC_MASK)
        {
            /* Invalid combination of bclk modes */
            return 0;
        }
        BW_I2S_RCR2_SYNC(channel, 1);
    }
   
    if (bclk_mode & I2S_RX_BCLK_NORMAL) /* Receiver BCLK not swapped */
    {
        HW_I2S_RCR2_CLR(channel, I2S_RCR2_BCS_MASK);
    }
    else if (bclk_mode & I2S_RX_BCLK_SWAPPED) /* Receiver BCLK swapped */
    {
        HW_I2S_RCR2_SET(channel, I2S_RCR2_BCS_MASK);
        /*
        ** When Rx is synch. BCS bit must be set also for Tx.
        */
        if (bclk_mode & I2S_RX_SYNCHRONOUS)
        {
            HW_I2S_TCR2_SET(channel, I2S_TCR2_BCS_MASK);
        }
    }
    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : sai_set_master_mode
* Returned Value   : 
* Comments         :
*    This function sets up master mode for TX and RX
*
*END*********************************************************************/
void sai_set_master_mode(ksai_info_struct_t* ksai_init_ptr, uint8_t flags)
{
    uint8_t         channel = ksai_init_ptr->hw_channel;
    uint8_t         mode    = ksai_init_ptr->mode;
    bool            set_tx_as_master = ((mode & I2S_TX_MASTER) >> 4) && (flags & I2S_IO_WRITE);
    bool            set_rx_as_master = (mode & I2S_RX_MASTER) && (flags & I2S_IO_READ);
    /* 
    ** If the master clock source is internal, enable it and recalculate
    ** values of frequencies in the internal info structure.
    */
    if (ksai_init_ptr->clock_source == I2S_CLK_INT)
    {
        HW_I2S_MCR_SET(channel, I2S_MCR_MOE_MASK);
    }
    /*
    ** Else disable master clock signal and set pin as input. Also reset values
    ** of frequencies in the info structure.
    */
    else if (ksai_init_ptr->clock_source == I2S_CLK_EXT)
    {
        HW_I2S_MCR_CLR(channel, I2S_MCR_MOE_MASK);
    }
    /*
    ** In case we want to set the transmitter and it is enabled, device is busy.
    */
    if ( BR_I2S_TCSR_TE(channel) && (set_tx_as_master))
    {
        return;
    }
    if ((ksai_init_ptr->clock_mode & I2S_TX_ASYNCHRONOUS) && (set_tx_as_master))
    {
        HW_I2S_TCR2_SET(channel, I2S_TCR2_BCD_MASK);
        HW_I2S_TCR4_SET(channel, I2S_TCR4_FSD_MASK);
        HW_I2S_TCR4_SET(channel, I2S_TCR4_FSP_MASK);        
        /* 
        ** If the the receiver is synchronous settings must same as the
        ** transmitter settings.
        */
        if (ksai_init_ptr->clock_mode & I2S_RX_SYNCHRONOUS)
        {
            HW_I2S_RCR2_SET(channel, I2S_RCR2_BCD_MASK);
            HW_I2S_RCR4_SET(channel, I2S_RCR4_FSD_MASK);
        }
        /* 
        ** If transmitter have swapped bit clock. set receiver bit clock to be
        ** generated internally
        */
        if (ksai_init_ptr->clock_mode & I2S_TX_BCLK_SWAPPED)
        {
            HW_I2S_RCR2_SET(channel, I2S_RCR2_BCD_MASK);
        }
    }

    /*
    ** In case we want to the set receiver and it is enabled, device is busy.
    */
    if ((BR_I2S_RCSR_RE(channel)) && (set_rx_as_master))
    {
        return;
    }
    if ((ksai_init_ptr->clock_mode & I2S_RX_ASYNCHRONOUS) && (set_rx_as_master))
    {
        
        HW_I2S_RCR2_SET(channel, I2S_RCR2_BCD_MASK);
        HW_I2S_RCR4_SET(channel, I2S_RCR4_FSD_MASK);          
        /* 
        ** If the the transmitter is synchronous settings must same as the
        ** receiver settings.
        */
        if (ksai_init_ptr->clock_mode & I2S_TX_SYNCHRONOUS)
        {
            
            HW_I2S_TCR2_SET(channel, I2S_TCR2_BCD_MASK);
            HW_I2S_TCR4_SET(channel, I2S_TCR4_FSD_MASK);  
        }
        /* 
        ** If receiver have swapped bit clock. Set transmitter bit clock to be
        ** generated internally.
        */
        if (ksai_init_ptr->clock_mode & I2S_RX_BCLK_SWAPPED)
        {
            HW_I2S_TCR2_SET(channel, I2S_TCR2_BCD_MASK);
        }
    }

}
/*FUNCTION****************************************************************
*
* Function Name    : sai_set_mclk_freq
* Returned Value   : None
* Comments         :
*    This function sets master clock frequency.
*
*END*********************************************************************/
uint32_t sai_set_mclk_freq(uint8_t channel, uint32_t fs_freq, uint32_t clk_mult)
{
    uint32_t mclk_freq = fs_freq * clk_mult;
    uint16_t fract, div;
    sai_find_mclk_freq_div(SYS_CLK, &fract, &div, mclk_freq);

    BW_I2S_MDR_DIVIDE(channel, div - 1);
    BW_I2S_MDR_FRACT(channel, fract - 1);
    while(BR_I2S_MCR_DUF(channel)) {}; /* wait until new value is set */
    
    return mclk_freq;
}

/*FUNCTION****************************************************************
*
* Function Name    : sai_set_fs_freq
* Returned Value   : None
* Comments         :
*    This function sets clock frequency.
*
*END*********************************************************************/
void sai_set_fs_freq(ksai_info_struct_t* ksai_init_ptr, uint8_t flags, uint32_t mclk_freq, uint32_t sample_rate)
{
    uint8_t channel = ksai_init_ptr->hw_channel;
    uint32_t bclk_freq, bclk_div;
    if (ksai_init_ptr->clock_source == I2S_CLK_INT)
    {
        /* Get bit clock value for selected sample rate*/
        bclk_freq = sample_rate * ksai_init_ptr->data_bits * 2;
        /* Get divider value for calculated bit clock frequency */
        bclk_div = mclk_freq / bclk_freq;
    
        if ((flags & I2S_IO_WRITE) || (ksai_init_ptr->clock_mode & I2S_RX_SYNCHRONOUS))
        {
            /* Set bit clock divider in TCR2 register */
            HW_I2S_TCR2_CLR(channel, I2S_TCR2_DIV_MASK);
            BW_I2S_TCR2_DIV(channel, (bclk_div / 2) - 1); 
        }
        if ((flags & I2S_IO_READ) || (ksai_init_ptr->clock_mode & I2S_TX_SYNCHRONOUS))
        {
            /* Set bit clock divider in RCR2 register */ 
            HW_I2S_RCR2_CLR(channel, I2S_RCR2_DIV_MASK);
            BW_I2S_RCR2_DIV(channel, (bclk_div / 2) - 1); 
        }
        
    }
    else
    {
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : sai_find_mclk_freq_div
* Returned Value   : Master clock frequency  
* Comments         :
*    This function tries to find master clock divider value required for 
*    selected master clock frequency.
*
*END*********************************************************************/
void sai_find_mclk_freq_div(uint32_t source_freq, uint_16_ptr frct, uint_16_ptr div, uint32_t frequency)
{
    uint32_t    closest_mclk = 0;
    uint32_t    temp_fract = 0;
    uint32_t    temp_div = 0;
    uint32_t    iter_mclk = 0;
    uint16_t    fract = 0;
    uint16_t    divide = 0;
    bool        found = FALSE;

    /*
    ** Find best divider value for selected mclk frequency.
    */
    for (fract = 1; fract < FRACT_MAX; fract++)
    {
        for(divide = 1; divide < DIV_MAX; divide++)
        {
            iter_mclk = (source_freq / divide) * fract;
            if (iter_mclk < frequency) break; /* Value too low, try next setting */
            if ((iter_mclk - frequency) == 0) /* Found value that matches exactly */
            {
                found = TRUE;
                break;
            }
            /* 
            ** If a new iteration of mclk is better than last that we have adapt 
            ** it as the new best value.
            */
            else if ((closest_mclk == 0) || ((iter_mclk % frequency) < (closest_mclk % frequency)))
            {
                temp_fract = fract;
                temp_div = divide;
                closest_mclk = iter_mclk;
            }
        }
        if (found) break;
    }
    /* 
    ** If we failed to find exact match use closest value we have.
    */
    if (found == FALSE)
    {
       fract = temp_fract;
       divide = temp_div;
       frequency = closest_mclk;
    }
    *frct = fract;
    *div = divide;
}

void sai_set_io_data_format(ksai_info_struct_t* ksai_init_ptr, uint8_t flags, uint8_t channels)
{
    uint8_t channel = ksai_init_ptr->hw_channel;
    /* Setup frame sync for transmitter */
    if ((flags & I2S_IO_WRITE) || (ksai_init_ptr->clock_mode & I2S_RX_SYNCHRONOUS))
    {
        HW_I2S_TCR4_CLR(channel, I2S_TCR4_FRSZ_MASK);
        BW_I2S_TCR4_FRSZ(channel, 1);
        /* Mask second transmitter channel if there is only one data channel */
        if (channels == 1)
        {
            HW_I2S_TMR_WR(channel, 0x02);
        }
    }
    /* Setup frame sync for receiver */
    if ((flags & I2S_IO_READ) || (ksai_init_ptr->clock_mode & I2S_TX_SYNCHRONOUS))
    {
        HW_I2S_RCR4_CLR(channel, I2S_RCR4_FRSZ_MASK);
        BW_I2S_RCR4_FRSZ(channel, 1);
        /* Mask second receiver channel if there is only one data channel */
        if (channels == 1)
        {
            HW_I2S_RMR_WR(channel, 0x02);
        }
    }
}

void sai_set_data_bits(ksai_info_struct_t* ksai_init_ptr, uint8_t data_bits, uint8_t flags)
{
    uint8_t channel = ksai_init_ptr->hw_channel;
    
    if ((data_bits < 8) || (data_bits > 32))
    {
        return;
    }

    /* Setup transmitter */
    if ((flags & I2S_IO_WRITE) || (ksai_init_ptr->clock_mode & I2S_RX_SYNCHRONOUS))
    {
        /* Set 0th word length */
        HW_I2S_TCR5_CLR(channel, I2S_TCR5_W0W_MASK);
        BW_I2S_TCR5_W0W(channel, data_bits - 1);
        /* Set Nth word length */
        HW_I2S_TCR5_CLR(channel, I2S_TCR5_WNW_MASK);
        BW_I2S_TCR5_WNW(channel, data_bits - 1);
        /* Set first bit shifted to highest index in register */
        HW_I2S_TCR5_CLR(channel, I2S_TCR5_FBT_MASK);
        BW_I2S_TCR5_FBT(channel, data_bits);
        /* Set sync width to match word length */
        HW_I2S_TCR4_CLR(channel, I2S_TCR4_SYWD_MASK);
        BW_I2S_TCR4_SYWD(channel, data_bits - 1);
    }
    
    /* Setup receiver */
    if ((flags & I2S_IO_READ) || (ksai_init_ptr->clock_mode & I2S_TX_SYNCHRONOUS))
    {
        /* Set 0th word length */
        HW_I2S_RCR5_CLR(channel, I2S_RCR5_W0W_MASK);
        BW_I2S_RCR5_W0W(channel, data_bits - 1);
        /* Set Nth word length */
        HW_I2S_RCR5_CLR(channel, I2S_RCR5_WNW_MASK);
        BW_I2S_RCR5_WNW(channel, data_bits - 1);
        /* Set first bit shifted to highest index in register */
        HW_I2S_RCR5_CLR(channel, I2S_RCR5_FBT_MASK);
        BW_I2S_RCR5_FBT(channel, data_bits);
        /* Set sync width to match word length */
        HW_I2S_RCR4_CLR(channel, I2S_RCR4_SYWD_MASK);
        BW_I2S_RCR4_SYWD(channel, data_bits - 1);
    }
}

void sai_tx_enable(uint8_t channel)
{
    HW_I2S_TCSR_SET(channel, I2S_TCSR_TE_MASK);
}
void sai_rx_enable(uint8_t channel)
{
    HW_I2S_RCSR_SET(channel, I2S_RCSR_RE_MASK);
}

void sai_tx_int_enable(uint8_t channel)
{
    HW_I2S_TCSR_SET(channel, I2S_TCSR_FRIE_MASK);
}

void sai_tx_int_disable(uint8_t channel)
{
    HW_I2S_TCSR_CLR(channel, I2S_TCSR_FRIE_MASK);
}
uint8_t sai_get_FIFO_request_status(uint8_t channel)
{
    return ((BR_I2S_TCSR_FRF(channel)) ? 1 : 0);
}
void sai_set_transmit_data(uint8_t channel, uint8_t tx_channel, uint32_t value)
{
    HW_I2S_TDRn_WR(channel, tx_channel, value);
}

uint32_t sai_get_write_FIFO_pointer(uint8_t channel, uint8_t tx_channel)
{
    return BR_I2S_TFRn_WFP(channel, tx_channel);
}

uint32_t sai_get_read_FIFO_pointer(uint8_t channel, uint8_t tx_channel)
{
    return BR_I2S_TFRn_RFP(channel, tx_channel);
}

void sai_clear_word_flag(uint8_t channel)
{
    HW_I2S_TCSR_SET(channel, I2S_TCSR_WSF_MASK);
}
#endif /* I2S_ENABLE */
/* EOF */
