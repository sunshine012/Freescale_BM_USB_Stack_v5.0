/**HEADER********************************************************************
* 
* Copyright (c) 2004 -2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: sgtl5000.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains functions for the SGTL5000
*
*END************************************************************************/

#include "types.h"
#include "derivative.h"
#include "soc_i2c.h"
#include "soc_i2s.h"
#include "audio_speaker.h"
#include "sgtl5000.h"

extern ksai_info_struct_t     ksai_init;
extern uint8_t                audio_data_buff[];
extern volatile uint32_t      data_queued;
volatile uint8_t              sai_event = 0;
uint16_t                      data_index = 0;

static void sgtl_WriteReg(uint16_t reg, uint16_t reg_val)
{
    uint8_t buffer[4];
    buffer[0] = (uint8_t)((reg >> 8) & 0xFF);
    buffer[1] = (uint8_t)(reg & 0xFF);
    buffer[2] = (uint8_t)((reg_val >> 8) & 0xFF);
    buffer[3] = (uint8_t)(reg_val & 0xFF);
    
    i2c_Start(I2C_CHANNEL);
    
    /* Device address */
    i2c_WriteByte(I2C_CHANNEL, (SGTL5000_I2C_ADDR << 1) | 0);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);
        
    /* Reg address byte 1 */
    i2c_WriteByte(I2C_CHANNEL, buffer[0]);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);
 
    /* Reg address byte 0 */
    i2c_WriteByte(I2C_CHANNEL, buffer[1]);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);
    
    /* Data byte 1 */
    i2c_WriteByte(I2C_CHANNEL, buffer[2]);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);
    
    /* Data byte 0 */
    i2c_WriteByte(I2C_CHANNEL, buffer[3]);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);
    
    i2c_Stop(I2C_CHANNEL);
    Pause();
}

static uint16_t sgtl_ReadReg(uint16_t addr)
{
    char result[2] = {0};

    i2c_Start(I2C_CHANNEL);

    i2c_WriteByte(I2C_CHANNEL,(SGTL5000_I2C_ADDR << 1) | 0);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);

    i2c_WriteByte(I2C_CHANNEL, addr>>8);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);

    i2c_WriteByte(I2C_CHANNEL, addr&0xff);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);

    i2c_RepeatedStart(I2C_CHANNEL);
    i2c_WriteByte(I2C_CHANNEL, (SGTL5000_I2C_ADDR << 1) | 1);
    i2c_Wait(I2C_CHANNEL);
    i2c_GetACK(I2C_CHANNEL);

    i2c_SetRXMode(I2C_CHANNEL);

    i2c_GiveACK(I2C_CHANNEL);
    result[0] = i2c_ReadByte(I2C_CHANNEL);
    i2c_Wait(I2C_CHANNEL);

    i2c_GiveNACK(I2C_CHANNEL);
    result[0] = i2c_ReadByte(I2C_CHANNEL);
    i2c_Wait(I2C_CHANNEL);

    i2c_Stop(I2C_CHANNEL);
    result[1] = i2c_ReadByte(I2C_CHANNEL);
    Pause();

    return (result[0]<<8) | result[1];
}

void sgtl_ModifyReg(uint16_t reg, uint16_t clr_mask, uint16_t set_mask)
{
    volatile uint16_t reg_val;
    reg_val = sgtl_ReadReg(reg);
    reg_val &= clr_mask;
    reg_val |= set_mask;
    sgtl_WriteReg(reg, reg_val);
}


/*FUNCTION****************************************************************
* 
* Function Name    : InitCodec
* Returned Value   : 
* Comments         :
* 
*
*END*********************************************************************/
void InitCodec()
{ 
    i2c_Init(I2C_CHANNEL);
    
    //--------------- Power Supply Configuration----------------
    // NOTE: This next 2 Write calls is needed ONLY if VDDD is
    // internally driven by the chip
    // Configure VDDD level to 1.2V (bits 3:0)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x0008);
    
    // Power up internal linear regulator (Set bit 9)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x7260);
    // NOTE: This next Write call is needed ONLY if VDDD is
    // externally driven
    // Turn off startup power supplies to save power (Clear bit 12 and 13)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4260);
    // NOTE: The next 2 Write calls is needed only if both VDDA and
    // VDDIO power supplies are less than 3.1V.
    // Enable the internal oscillator for the charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_CLK_TOP_CTRL, 0x0800);
    // Enable charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4A60);
    // NOTE: The next 2 modify calls is only needed if both VDDA and
    // VDDIO are greater than 3.1V
    // Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x006C);

    //------ Reference Voltage and Bias Current Configuration----------
    // NOTE: The value written in the next 2 Write calls is dependent
    // on the VDDA voltage value.
    // Set ground, ADC, DAC reference voltage (bits 8:4). The value should
    // be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
    // The bias current should be set to 50% of the nominal value (bits 3:1)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x004E);
    // Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current
    // (bits 11:8) to the recommended value of 0.36mA for 10kOhm load with 1nF
    // capacitance
    sgtl_WriteReg(CHIP_LINE_OUT_CTRL, 0x0322);

    //----------------Other Analog Block Configurations------------------
    // Configure slow ramp up rate to minimize pop (bit 0)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x004F);
    // Enable short detect mode for headphone left/right
    // and center channel and set short detect current trip level
    // to 75mA
    sgtl_WriteReg(CHIP_SHORT_CTRL, 0x1106);
    // Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
    sgtl_WriteReg(CHIP_ANA_CTRL, 0x0133);

    //----------------Power up Inputs/Outputs/Digital Blocks-------------
    // Power up LINEOUT, HP, ADC, DAC
    sgtl_WriteReg(CHIP_ANA_POWER, 0x6AFF);
    // Power up desired digital blocks
    // I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
    // ADC (bit 6) are powered on
    sgtl_WriteReg(CHIP_DIG_POWER, 0x0073);

    //--------------------Set LINEOUT Volume Level-----------------------
    // Set the LINEOUT volume level based on voltage reference (VAG)
    // values using this formula
    // Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
    // Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
    // left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
    // to 5
    sgtl_WriteReg(CHIP_LINE_OUT_VOL, 0x0505);

    // Configure SYS_FS clock to 48kHz
    // Configure MCLK_FREQ to 256*Fs
    // sgtl_ModifyReg(CHIP_CLK_CTRL, 0xFFC8, 0x0008); // bits 3:2 (1) // 8
    sgtl_WriteReg(CHIP_CLK_CTRL, 0x0008);
    // Configure the I2S clocks in master mode
    // NOTE: I2S LRCLK is same as the system sample clock
    // Data length = 16 bits
    // sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x01B0); // bit 7        (2) // 432
    sgtl_WriteReg(CHIP_I2S_CTRL, 0x01B0);

    // I2S_IN -> DAC -> HP_OUT
    // Route I2S_IN to DAC
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFDF, 0x0010); //(3) // 16
    //sgtl_WriteReg(CHIP_SSS_CTRL, 0x0010);
    // Select DAC as the input to HP_OUT
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFBF, 0x0000); //(4) // 307
    // sgtl_WriteReg(CHIP_ANA_CTRL, 0x0133);

    // TODO: Configure Microphone -> ADC -> I2S_OUT
    // Microphone -> ADC -> I2S_OUT
    // Set ADC input to Microphone
    // sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFF, 0x0004); // Set ADC input to LINE_IN (5) // 311
    //sgtl_WriteReg(CHIP_ANA_CTRL, 0x0137);
    // Route ADC to I2S_OUT
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFFC, 0x0000); // bits 1:0 (6) // 16
    //sgtl_WriteReg(CHIP_SSS_CTRL, 0x0010);

    //---------------- Input Volume Control---------------------
    // Configure ADC left and right analog volume to desired default.
    // Example shows volume of 0dB
    sgtl_WriteReg(CHIP_ANA_ADC_CTRL, 0x0000);
    // Configure MIC gain if needed. Example shows gain of 20dB
    sgtl_ModifyReg(CHIP_MIC_CTRL, 0xFFFD, 0x0001); // bits 1:0 (8) // 1
    //sgtl_WriteReg(CHIP_MIC_CTRL, 0x0001);

    //---------------- Volume and Mute Control---------------------
    // Configure HP_OUT left and right volume to minimum, unmute
    // HP_OUT and ramp the volume up to desired volume.
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, 0x7F7F);
    // Code assumes that left and right volumes are set to same value
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, 0x0000);
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFEF, 0x0000); // bit 5     (9) // 295
    //sgtl_WriteReg(CHIP_ANA_CTRL, 0x0127);
    // LINEOUT and DAC volume control
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFEFF, 0x0000); // bit 8 (10) // 39
    //sgtl_WriteReg(CHIP_ANA_CTRL, 0x0027);
    // Configure DAC left and right digital volume. Example shows
    // volume of 0dB
    sgtl_WriteReg(CHIP_DAC_VOL, 0x3C3C);
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFFB, 0x0000); // bit 2 (11) // 568
    //sgtl_WriteReg(CHIP_ADCDAC_CTRL, 0x0238);
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFF7, 0x0000); // bit 3 (12) // 560
    //sgtl_WriteReg(CHIP_ADCDAC_CTRL, 0x0230);
    // Unmute ADC
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFE ,0x0000); // bit 0 (13) // 38
    //sgtl_WriteReg(CHIP_ANA_CTRL, 0x0026);
}

/*FUNCTION****************************************************************
* 
* Function Name    : SetupCodec
* Returned Value   : 
* Comments         :
*     
*
*END*********************************************************************/
void SetupCodec(uint32_t fs_freq)
{
    switch (fs_freq)
    {
        case 8000:
            sgtl_WriteReg(CHIP_CLK_CTRL, 0x0021);
            sgtl_WriteReg(CHIP_I2S_CTRL, 0x0130);
            break;

        case 11025:
            break;

        case 12000:
            break;

        case 16000:
            sgtl_WriteReg(CHIP_CLK_CTRL, 0x0010);
            sgtl_WriteReg(CHIP_I2S_CTRL, 0x0130); // 304
            break;

        case 22050:
            sgtl_WriteReg(CHIP_CLK_CTRL, 0x0015);
            sgtl_WriteReg(CHIP_I2S_CTRL, 0x0130);
            break;

        case 24000:
            break;

        case 32000:
            sgtl_WriteReg(CHIP_CLK_CTRL, 0x0001);
            sgtl_WriteReg(CHIP_I2S_CTRL, 0x0130);
            break;

        case 44100:
            sgtl_WriteReg(CHIP_CLK_CTRL, 0x0005);
            sgtl_WriteReg(CHIP_I2S_CTRL, 0x0130);
            break;

        case 48000:
            sgtl_WriteReg(CHIP_CLK_CTRL, 0x0009);
            sgtl_WriteReg(CHIP_I2S_CTRL, 0x0130);
            break;

        case 96000:
            break;

        default:
            return;
    }
    /* rise volume */
    sgtl_WriteReg(CHIP_DAC_VOL, 0x4C4C);
}

void sai_tx_irq_handler(void)
{
    uint8_t               channel = ksai_init.hw_channel;
    uint32_t              i = 0;
    uint32_t              space;
    uint8_t               tx_channel = ksai_init.tx_channel;
    uint32_t              read_ptr;
    uint32_t              write_ptr;
    uint32_t              value = 0;

    if (data_queued >= 3*DATA_BUFF_SIZE)
    {
        sai_event = 1;
    }

    if (sai_event == 1)
    {
        read_ptr  = sai_get_read_FIFO_pointer(channel, tx_channel);
        write_ptr = sai_get_write_FIFO_pointer(channel, tx_channel);
     
        /* Check FIFO request flag */
        if (sai_get_FIFO_request_status(channel)!=0)
        {   
            if (write_ptr == read_ptr) /* FIFO is empty */
            {
                space = SIZE_OF_FIFO;
            }
            else /* FIFO is not empty */
            {
                int result; 
                
                result = (write_ptr & I2S_TX_WRITE_PTR_MASK) - (read_ptr & I2S_TX_READ_PTR_MASK);
                space = (result < 0) ? - (unsigned)result : result;
            } 
            
            if (data_index >= DATA_BUFF_SIZE*4) 
            {
                data_index = 0;
            }

            /* Copy data from buffer to FIFO */
            for (i = 0; i < space && data_queued > 0 ; i++)
            {
                value = sai_format_in_data(audio_data_buff + data_index, 2);
                sai_set_transmit_data(channel, tx_channel, value);
                data_index += 2;
                data_queued -= 2;
            }
            if (data_queued <= 0)
            {
                data_queued   = 0;
                sai_event     = 0;
                /* Disable SAI interrupt */
                sai_tx_int_disable(channel);
            }
        }
        
        /* Clear Word start flag */
        sai_clear_word_flag(channel);
    }
}

/* EOF */
