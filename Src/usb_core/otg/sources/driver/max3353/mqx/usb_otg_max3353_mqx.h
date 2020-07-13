/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 Freescale Semiconductor;
* All Rights Reserved
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
* $FileName: usb_otg_max3353_mqx.h$
* $Version : 
* $Date    : 
*
* Comments : This is the header file for the MAX3353 access functions
*
*         
*****************************************************************************/
#ifndef __usb_otg_max3353_mqx_h__
#define __usb_otg_max3353_mqx_h__

#include "usb_otg_khci_max3353_prv.h"
enum
{
    MAX3353_OK,
    MAX3353_NOT_PRESENT
};

#define MAX3353_MID             0x6A0B5333
#define MAX3353_PID             0x485A4200

#define ID_GND_EVENT            0x20
#define ID_FLOAT_EVENT          0x40
#define VBUS_LOW_EVENT          0x10
#define VBUS_HIGH_EVENT         0x01
#define ID_FLOAT_EN_MASK        0x10
#define ID_GND_EN_MASK          0x08
#define VBUS_VALID_EN_MASK      0x01
#define SESSION_SESSEND_EN_MASK 0x04
#define SESSION_VALID_EN_MASK   0x01

#define VBUS_VALID_ED_MASK      0x01
#define SESSION_VALID_ED_MASK   0x02

/* Public constants */
/* MAX3353 I2C slave address */
#define MAX3353_SLAVE_ADDR       0x2C
/* MAX3353 Registers */
#define MAX3353_REG_MANU_0       0x00
#define MAX3353_REG_MANU_1       0x01
#define MAX3353_REG_MANU_2       0x02
#define MAX3353_REG_MANU_3       0x03
#define MAX3353_REG_PRODID_0     0x04
#define MAX3353_REG_PRODID_1     0x05
#define MAX3353_REG_PRODID_2     0x06
#define MAX3353_REG_PRODID_3     0x07
#define MAX3353_REG_CTRL_1       0x10
#define MAX3353_REG_CTRL_2       0x11
#define MAX3353_REG_STATUS       0x13
#define MAX3353_REG_INT_MASK     0x14
#define MAX3353_REG_INT_EDGE     0x15
#define MAX3353_REG_INT_LATCH    0x16

/* MAX3353 register bits */
#define OTG_STAT_VBUS_VALID      0x01
#define OTG_STAT_SESS_VALID      0x02
#define OTG_STAT_SESS_END        0x04
#define OTG_STAT_ID_GND          0x08
#define OTG_STAT_ID_FLOAT        0x10
#define OTG_STAT_A_HNP           0x20
#define OTG_STAT_B_HNP           0x40

#define OTG_INT_VBUS_VALID_CHG   0x01
#define OTG_INT_SESS_VALID_CHG   0x02
#define OTG_INT_SESS_END_CHG     0x10
#define OTG_INT_ID_GND_CHG       0x20
#define OTG_INT_ID_FLOAT_CHG     0x40
#define OTG_INT_A_HNP_CHG        0x80

#define OTG_CTRL_2_SDWN          0x01
#define OTG_CTRL_2_VBUS_CHG1     0x02
#define OTG_CTRL_2_VBUS_CHG2     0x04
#define OTG_CTRL_2_VBUS_DRV      0x08
#define OTG_CTRL_2_VBUS_DISCHG   0x10
#define OTG_CTRL_1_IRQ_PUSH_PULL 0x02
#define OTG_CTRL_1_PDOWN_DP      0x40
#define OTG_CTRL_1_PDOWN_DM      0x80

#define USB_OTG_MAX3353_ISR_EVENT (0x02)

#ifdef __cplusplus
extern "C" {
#endif

/* Public functions */
usb_status _usb_otg_max3353_init(usb_otg_max3353_call_struct_t * otg_max3353_call_ptr);
usb_status _usb_otg_max3353_get_status(usb_otg_max3353_call_struct_t * otg_max3353_call_ptr);
usb_status _usb_otg_max3353_set_vbus(usb_otg_max3353_call_struct_t * otg_max3353_call_ptr, bool enable);
usb_status _usb_otg_max3353_set_pull_downs(usb_otg_max3353_call_struct_t * otg_max3353_call_ptr, uint8_t bitfield );
#ifdef __cplusplus
}
#endif

#endif  /* __usb_otg_max3353_mqx_h__ */


