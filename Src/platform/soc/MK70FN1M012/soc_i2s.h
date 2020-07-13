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
* $FileName: soc_i2s.h
* $Version :
* $Date    :
*
* Comments:
*
*  
*
*END*************************************************************************/

#ifndef SOC_I2S_H_
#define SOC_I2S_H_

typedef struct ksai_info_struct
{
    /* Selected SAI HW module */
    uint8_t hw_channel;
    
    /* I2S TX channel */
    uint8_t tx_channel;

    /* I2S RX channel */
    uint8_t rx_channel;
    /* Clock setup: sync-async; bitclock: normal-swapped */
    uint8_t clock_mode;

    /* I2S mode of operation (master/slave)*/
    uint8_t mode;

    /* Number of valid data bits */
    uint8_t data_bits;
    
    /* I2S module clock source (only affects SAI master mode) */
    uint8_t clock_source;

    
} ksai_info_struct_t;

#define SYS_CLK                             120000000

/* Limits for master clock divider */
#define FRACT_MAX                           256
#define DIV_MAX                             4096

#define I2S_RX_FIFO_FULL_MASK               0x10
/*
** Values of Kinetis watermarks are set to 
** experimentally tested default values.
*/
#define I2S_TX_FIFO_DEFAULT_WATERMARK       4
#define I2S_RX_FIFO_DEFAULT_WATERMARK       4
#define SIZE_OF_FIFO                        8

#define I2S_RX_WRITE_PTR_MASK               0x7
#define I2S_RX_READ_PTR_MASK                0x7
#define I2S_TX_WRITE_PTR_MASK               0x7
#define I2S_TX_READ_PTR_MASK                0x7

#define DEFAULT_BCLK_DIV                    8

#define I2S_CHAR_BIT                        (0x08)

/*
** I2S Clock sources
*/
#define I2S_CLK_INT                         (0x00)
#define I2S_CLK_EXT                         (0x01)

/*
** I2S Bus Modes
*/
#define I2S_MODE_MASTER                     (0x01)
#define I2S_MODE_SLAVE                      (0x02)

/*
** I2S I/O Mode
*/
#define I2S_IO_READ                         (0x04)
#define I2S_IO_WRITE                        (0x08)

/*
** I2S RX/TX clock modes
*/
#define I2S_TX_ASYNCHRONOUS                 (0x01)
#define I2S_TX_SYNCHRONOUS                  (0x02)
#define I2S_RX_ASYNCHRONOUS                 (0x04)
#define I2S_RX_SYNCHRONOUS                  (0x08)

#define I2S_TX_BCLK_NORMAL                  (0x10)
#define I2S_TX_BCLK_SWAPPED                 (0x20)
#define I2S_RX_BCLK_NORMAL                  (0x40)
#define I2S_RX_BCLK_SWAPPED                 (0x80)

#define I2S_TX_MASTER                       (I2S_MODE_MASTER << 4)
#define I2S_TX_SLAVE                        (I2S_MODE_SLAVE  << 4)
 
#define I2S_RX_MASTER                       (I2S_MODE_MASTER)
#define I2S_RX_SLAVE                        (I2S_MODE_SLAVE)

#define I2C_CHANNEL                         0
/*
* Function prototypes
*/

uint32_t    i2s_get_vector(uint8_t channel);
void        sai_tx_irq_handler(void);

void        sai_tx_fifo_err_irq_handler(void);

void        sai_init(ksai_info_struct_t* ksai_init_ptr, uint8_t flags, uint32_t fs_freq, uint16_t clk_mult, uint8_t channels);
void        sai_io_init(uint32_t dev_num);
void        sai_hw_init(ksai_info_struct_t* ksai_init_ptr, uint32_t io_mode);
bool        sai_set_bclk_mode(uint8_t channel, uint8_t bclk_mode);
void        sai_set_master_mode(ksai_info_struct_t* ksai_init_ptr, uint8_t flags);
uint32_t    sai_set_mclk_freq(uint8_t channel, uint32_t fs_freq, uint32_t clk_mult);
void        sai_set_fs_freq(ksai_info_struct_t* ksai_init_ptr, uint8_t flags, uint32_t mclk_freq, uint32_t sample_rate);
void        sai_find_mclk_freq_div( uint32_t source_freq, uint_16_ptr frct, uint_16_ptr div, uint32_t frequency);
void        sai_set_io_data_format(ksai_info_struct_t* ksai_init_ptr, uint8_t flags, uint8_t channels);
void        sai_set_data_bits(ksai_info_struct_t* ksai_init_ptr, uint8_t data_bits, uint8_t flags);
void        sai_tx_enable(uint8_t channel);
void        sai_rx_enable(uint8_t channel);
void        sai_tx_int_enable(uint8_t channel);
void        sai_tx_int_disable(uint8_t channel);
uint32_t    sai_format_in_data(uint8_t* input,uint8_t size);
void        sai_tx_enable(uint8_t channel);
void        sai_rx_enable(uint8_t channel);
void        sai_tx_int_enable(uint8_t channel);
void        sai_tx_int_disable(uint8_t channel);
uint8_t     sai_get_FIFO_request_status(uint8_t channel);
void        sai_set_transmit_data(uint8_t channel, uint8_t tx_channel, uint32_t value);
uint32_t    sai_get_write_FIFO_pointer(uint8_t channel, uint8_t tx_channel);
uint32_t    sai_get_read_FIFO_pointer(uint8_t channel, uint8_t tx_channel);
void        sai_clear_word_flag(uint8_t channel);

#endif /* I2S_H_ */
