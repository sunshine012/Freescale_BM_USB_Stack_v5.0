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
* $FileName: composite.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file implement composite driver.
*****************************************************************************/ 

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "audio_generator.h"
#include "mouse.h"
#include "usb_class_composite.h"
#include "composite_app.h"

#if ! USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined non-zero in usb_device_config.h. Please recompile usbd with this option.
#endif


/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 #define MAIN_TASK       10
 
#define COMPOSITE_CFG_MAX 2

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);
extern void Main_Task(uint32_t param);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern usb_endpoints_t usb_desc_ep;
comosite_handle_t   g_app_handle;
extern usb_desc_request_notify_struct_t  desc_callback;
extern audio_handle_t   g_audio_handle;
extern MOUSE_GLOBAL_VARIABLE_STRUCT g_mouse;
extern void Main_Task(uint32_t param);
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
static class_config_struct_t       audio_config_callback; 

static class_config_struct_t       Hid_config_callback;

static composite_config_struct_t usb_composite_callback;
static class_config_struct_t       composite_cfg[COMPOSITE_CFG_MAX] = {0};
extern void Audio_USB_App_Callback(uint8_t event_type, void* val,void* arg);
extern void Hid_App_Callback(uint8_t event_type, void* val,void* arg); 
extern void Hid_TestApp_Init(void);
extern void Audio_TestApp_Init(void);
extern  uint8_t Hid_App_Param_Callback(uint8_t request, uint16_t value, 
    				uint8_t ** data, uint32_t* size,void* arg);
extern	uint8_t Audio_App_Param_Callback(uint8_t request, uint16_t value, 
					 uint8_t ** data, uint32_t* size,void* arg);

 /*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for Audio generator
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void APP_init(void)
{
  
   printf("Hid_audio_TestApp_Init\n");
   audio_config_callback.composite_application_callback.callback = Audio_USB_App_Callback;
   audio_config_callback.composite_application_callback.arg = &g_audio_handle;
   audio_config_callback.class_specific_callback.callback = Audio_App_Param_Callback;
   audio_config_callback.class_specific_callback.arg = &g_audio_handle;
   audio_config_callback.desc_callback_ptr = &desc_callback;
   audio_config_callback.type = USB_CLASS_AUDIO;	   
   /* initialize the Global Variable Structure */
   OS_Mem_zero(&g_mouse, sizeof(MOUSE_GLOBAL_VARIABLE_STRUCT));

   /* Initialize the USB interface */

   Hid_config_callback.composite_application_callback.callback = Hid_App_Callback;
   Hid_config_callback.composite_application_callback.arg = &g_mouse.app_handle;
   Hid_config_callback.class_specific_callback.callback = Hid_App_Param_Callback;
   Hid_config_callback.class_specific_callback.arg = &g_mouse.app_handle;
   Hid_config_callback.desc_callback_ptr = &desc_callback;
   Hid_config_callback.type = USB_CLASS_HID;
    
   usb_composite_callback.count = 2;
   usb_composite_callback.class_app_callback = composite_cfg;
   usb_composite_callback.class_app_callback[0] = audio_config_callback;
   usb_composite_callback.class_app_callback[1] = Hid_config_callback;
 
   /* Initialize the USB interface */
   USB_Composite_Init(CONTROLLER_ID, &usb_composite_callback, &g_app_handle);
   USB_Composite_Get_Class_Handle(g_app_handle, USB_CLASS_HID, &g_mouse.app_handle);
   USB_Composite_Get_Class_Handle(g_app_handle, USB_CLASS_AUDIO, &g_audio_handle);
   
   Hid_TestApp_Init();
   Audio_TestApp_Init();
   
  // while(1);
} 

void APP_task(void)
{
	
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls the Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task
   (
      uint32_t param
   )
{   
    UNUSED_ARGUMENT (param)
	APP_init();  

}
/* EOF */
