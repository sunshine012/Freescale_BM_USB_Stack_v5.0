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
* $FileName: soc_i2c.h
* $Version :
* $Date    :
*
* Comments:
*
*  
*
*END*************************************************************************/
 #ifndef _SOC_I2C_H_
 #define _SOC_I2C_H_

/*INCLUDES*-----------------------------------------------------------------*/

/*MACROS*-------------------------------------------------------------------*/

/*GLOBAL FUNCTION PROTOTYPES*-----------------------------------------------*/
uint32_t    i2c_get_vector(uint8_t);

void        i2c_Init(uint8_t i2c_channel);
void        i2c_SetTXMode(uint8_t i2c_channel);
void        i2c_SetRXMode(uint8_t i2c_channel);
void        i2c_SetSlaveMode(uint8_t i2c_channel);
void        i2c_SetMasterMode(uint8_t i2c_channel);
void        i2c_WriteByte(uint8_t i2c_channel, uint8_t data);
uint8_t     i2c_ReadByte(uint8_t i2c_channel);
void        i2c_Wait(uint8_t i2c_channel);
void        i2c_Start(uint8_t i2c_channel);
void        i2c_Stop(uint8_t i2c_channel);
void        i2c_RepeatedStart(uint8_t i2c_channel);
bool        i2c_GetACK(uint8_t i2c_channel);
void        i2c_GiveACK(uint8_t i2c_channel);
void        i2c_GiveNACK(uint8_t i2c_channel);

#endif
/* EOF */
