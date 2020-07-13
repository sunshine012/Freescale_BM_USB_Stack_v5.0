/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2014 Freescale Semiconductor;
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
* $FileName: usb_otg_main.h$
* $Version : 
* $Date    : 
*
* Comments : This is the header file for the OTG driver
*
*         
*****************************************************************************/

#ifndef USB_OTG_MAIN_H_
#define USB_OTG_MAIN_H_

#include "usb.h"
/* Public constants */

/* Number of available USB device controllers */
#define DEBUG_LOG_TRACE(x) printf("\n%s",x)
/* B state machine indications */
#define OTG_B_IDLE                      ((uint32_t)0x0001)     /* B idle state */
#define OTG_B_IDLE_SRP_READY            ((uint32_t)0x0002)     /* B idle, SRP ready to start */
#define OTG_B_SRP_INIT                  ((uint32_t)0x0004)     /* B srp init state */
#define OTG_B_SRP_FAIL                  ((uint32_t)0x0008)     /* B srp failed to get a response */
#define OTG_B_PERIPHERAL                ((uint32_t)0x0010)     /* B peripheral state */
#define OTG_B_PERIPHERAL_LOAD_ERROR     ((uint32_t)0x0020)     /* B peripheral state (peripheral stack could not be loaded) */
#define OTG_B_PERIPHERAL_HNP_READY      ((uint32_t)0x0040)     /* B peripheral, HNP ready to be performed */
#define OTG_B_PERIPHERAL_HNP_START      ((uint32_t)0x0080)     /* B peripheral, HNP start */
#define OTG_B_PERIPHERAL_HNP_FAIL       ((uint32_t)0x0100)     /* B peripheral, HNP failed */
#define OTG_B_HOST                      ((uint32_t)0x0200)     /* B host state */
#define OTG_B_HOST_LOAD_ERROR           ((uint32_t)0x0400)     /* B host state (host stack could not be loaded) */
#define OTG_B_A_HNP_REQ                 ((uint32_t)0x0800)

/* A state machine indications */
#define OTG_A_IDLE                      ((uint32_t)0x00010000)
#define OTG_A_WAIT_VRISE                ((uint32_t)0x00020000)
#define OTG_A_WAIT_BCON                 ((uint32_t)0x00040000)
#define OTG_A_HOST                      ((uint32_t)0x00080000)
#define OTG_A_SUSPEND                   ((uint32_t)0x00100000)
#define OTG_A_PERIPHERAL                ((uint32_t)0x00200000)
#define OTG_A_WAIT_VFALL                ((uint32_t)0x00400000)
#define OTG_A_VBUS_ERR                  ((uint32_t)0x00800000)
#define OTG_A_WAIT_VRISE_TMOUT          ((uint32_t)0x01000000)
#define OTG_A_WAIT_BCON_TMOUT           ((uint32_t)0x02000000)
#define OTG_A_B_HNP_REQ                 ((uint32_t)0x04000000)
#define OTG_A_BIDL_ADIS_TMOUT           ((uint32_t)0x08000000)
#define OTG_A_ID_TRUE                   ((uint32_t)0x10000000)
#define OTG_A_HOST_LOAD_ERROR           ((uint32_t)0x20000000)
#define OTG_A_PERIPHERAL_LOAD_ERROR     ((uint32_t)0x40000000)
#define OTG_A_AIDL_BDIS_TMOUT           ((uint32_t)0x80000000)
#define USB_STATUS_OTG                 (0x08)
/* Public types */

typedef uint32_t otg_event;
/* Function for getting the active interrupts from the external circuit */
typedef uint32_t (*otg_load_usb_stack)(void);
typedef uint32_t (*otg_unload_usb_stack)(void);
typedef void     (*otg_event_callback)(usb_otg_handle handle, otg_event event);

/* OTG initialization structure type */
#ifdef __CC_ARM
  #pragma push
  #pragma pack(1)
#endif
#ifdef __GNUC__
  #pragma pack(push)
  #pragma pack(1)
#endif
typedef struct otg_init_struct
{

    otg_event_callback      app_otg_callback;
    otg_load_usb_stack      load_usb_host;
    otg_load_usb_stack      load_usb_device;
    otg_unload_usb_stack    unload_usb_host;
    otg_unload_usb_stack    unload_usb_device;
} otg_int_struct_t;

typedef enum
{
    USB_STACK_TYPE_DEVICE = 1,
    USB_STACK_TYPE_HOST,
    USB_STACK_TYPE_OTG
} usb_stack_type_t;

enum
{
    USB_ACTIVE_STACK_NONE,
    USB_ACTIVE_STACK_DEVICE,
    USB_ACTIVE_STACK_HOST
};

typedef struct usb_otg_if_struct
{
   const struct usb_otg_callback_functions_struct   *otg_if;
   void*                                            otg_init_param;
   void*                                            otg_handle;
   usb_stack_type_t                                 stack_type;
} usb_otg_if_struct_t;

/* Public functions */

extern usb_status  _usb_otg_init(uint8_t controller_id, otg_int_struct_t * init_struct_ptr, usb_host_handle *  handle);
extern usb_status  _usb_otg_shut_down(usb_otg_handle  otg_handle);
extern uint32_t    _usb_otg_session_request(usb_otg_handle handle);
extern uint32_t    _usb_otg_bus_request(usb_otg_handle handle);
extern uint32_t    _usb_otg_bus_release(usb_otg_handle handle);
extern uint32_t    _usb_otg_set_a_bus_req(usb_otg_handle otg_handle , bool a_bus_req );
extern uint32_t    _usb_otg_get_a_bus_req(usb_otg_handle otg_handle , bool* a_bus_req );
extern uint32_t    _usb_otg_set_a_bus_drop(usb_otg_handle otg_handle , bool a_bus_drop );
extern uint32_t    _usb_otg_get_a_bus_drop(usb_otg_handle otg_handle , bool* a_bus_drop );
extern uint32_t    _usb_otg_set_a_clear_err( usb_otg_handle otg_handle );

#ifdef __CC_ARM
  #pragma pop
#endif
#ifdef __GNUC__
  #pragma pack(pop)
#endif
#endif /* USB_OTG_MAIN_H_ */
