/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: usb_otg_khci_max3353_prv.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*         
*****************************************************************************/
#ifndef __usb_otg_khci_max3353_prv_h__
#define __usb_otg_khci_max3353_prv_h__

#include "usb_otg_khci.h"

typedef struct usb_max3353_otg_init_struct {
    void*        int_port;
    uint32_t     int_pin;
    uint32_t     vector;
    uint32_t     priority;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)    
    char*        i2c_dev_id;
#else
    uint8_t      channel;
#endif
    uint8_t      i2c_address;  
} usb_max3353_otg_init_struct_t;
typedef struct usb_khci_max3353_otg_init_struct {
    usb_khci_otg_int_struct_t khci;
    usb_max3353_otg_init_struct_t max3353;
} usb_khci_max3353_otg_init_struct_t;
typedef struct usb_otg_max3353_call_struct
{
    usb_otg_state_struct_t *        otg_handle_ptr;
    usb_max3353_otg_init_struct_t * init_param_ptr;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    void*                           i2c_f_ptr;
#else
    uint8_t                         channel;
#endif
} usb_otg_max3353_call_struct_t;
typedef struct usb_otg_khci_max3353_struct
{
    usb_otg_state_struct_t                g;
    usb_otg_khci_call_struct_t *        khci_call_ptr;
    usb_otg_max3353_call_struct_t *     max3353_call_ptr;
} usb_otg_khci_max3353_struct_t;
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
