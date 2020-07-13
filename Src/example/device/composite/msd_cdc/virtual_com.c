/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: virtual_com.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file emulates a USB PORT as RS232 PORT.
*****************************************************************************/ 

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include <fio.h>
#else
#endif

#include "virtual_com.h"

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern usb_endpoints_t usb_desc_ep;
extern usb_desc_request_notify_struct_t  desc_callback;

//cdc_handle_t   g_app_handle;
cdc_handle_t g_vcom_handle;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void VCom_USB_App_Callback(uint8_t event_type, void* val,void* arg);
uint8_t VCom_USB_Notif_Callback(uint8_t event, uint16_t value, uint8_t ** data, uint32_t* size, void* arg); 
void Virtual_Com_App(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
static uint8_t g_line_coding[USB_MAX_SUPPORTED_INTERFACES][LINE_CODING_SIZE] =
{
    { (LINE_CODE_DTERATE_IFACE0>> 0) & 0x000000FF,
        (LINE_CODE_DTERATE_IFACE0>> 8) & 0x000000FF,
        (LINE_CODE_DTERATE_IFACE0>>16) & 0x000000FF,
        (LINE_CODE_DTERATE_IFACE0>>24) & 0x000000FF,
        /*e.g. 0x00,0xC2,0x01,0x00 : 0x0001C200 is 115200 bits per second */
        LINE_CODE_CHARFORMAT_IFACE0,
        LINE_CODE_PARITYTYPE_IFACE0,
        LINE_CODE_DATABITS_IFACE0
    }
};

static uint8_t g_abstract_state[USB_MAX_SUPPORTED_INTERFACES][COMM_FEATURE_DATA_SIZE] =
{
    { (STATUS_ABSTRACT_STATE_IFACE0>>0) & 0x00FF,
        (STATUS_ABSTRACT_STATE_IFACE0>>8) & 0x00FF
    }
};

static uint8_t g_country_code[USB_MAX_SUPPORTED_INTERFACES][COMM_FEATURE_DATA_SIZE] =
{
    { (COUNTRY_SETTING_IFACE0>>0) & 0x00FF,
        (COUNTRY_SETTING_IFACE0>>8) & 0x00FF
    }
};
static bool start_app = FALSE;
static bool start_transactions = FALSE;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
static uint8_t *g_curr_recv_buf;
static uint8_t *g_curr_send_buf;
#else
static uint8_t g_curr_recv_buf[DATA_BUFF_SIZE];
static uint8_t g_curr_send_buf[DATA_BUFF_SIZE];
#endif
static uint8_t g_recv_size;
static uint8_t g_send_size;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 
 /**************************************************************************//*!
  *
  * @name  USB_Get_Line_Coding
  *
  * @brief The function returns the Line Coding/Configuraion
  *
  * @param handle:		  handle	 
  * @param interface:	  interface number	   
  * @param coding_data:   output line coding data	  
  *
  * @return USB_OK								When Successfull
  * 		USBERR_INVALID_REQ_TYPE 			when Error
  *****************************************************************************/
 uint8_t USB_Get_Line_Coding(uint32_t handle, 
								 uint8_t interface, 
								 uint8_t * *coding_data)
 {	 
	 UNUSED_ARGUMENT(handle)
	 /* if interface valid */
	 if(interface < USB_MAX_SUPPORTED_INTERFACES)
	 {
		 /* get line coding data*/
		 *coding_data = g_line_coding[interface];
		 return USB_OK;
	 }
	 
	 return USBERR_INVALID_REQ_TYPE;
 }
 
 /**************************************************************************//*!
  *
  * @name  USB_Set_Line_Coding
  *
  * @brief The function sets the Line Coding/Configuraion
  *
  * @param handle: handle	  
  * @param interface:	  interface number	   
  * @param coding_data:   output line coding data	  
  *
  * @return USB_OK								When Successfull
  * 		USBERR_INVALID_REQ_TYPE 			when Error
  *****************************************************************************/
 uint8_t USB_Set_Line_Coding(uint32_t handle, 
								 uint8_t interface, 
								 uint8_t * *coding_data)
 {	 
	 uint8_t count;
 
	 UNUSED_ARGUMENT(handle)
	 
	 /* if interface valid */
	 if(interface < USB_MAX_SUPPORTED_INTERFACES)
	 {
		 /* set line coding data*/
		 for (count = 0; count < LINE_CODING_SIZE; count++) 
		 {			
			 g_line_coding[interface][count] = *((*coding_data+USB_SETUP_PKT_SIZE) + count);
		 }
		 return USB_OK;  
	 }
	 
	 return USBERR_INVALID_REQ_TYPE;
 }
 
 /**************************************************************************//*!
  *
  * @name  USB_Get_Abstract_State
  *
  * @brief The function gets the current setting for communication feature
  * 												 (ABSTRACT_STATE)
  * @param handle:		  handle
  * @param interface:	  interface number	   
  * @param feature_data:   output comm feature data 	
  *
  * @return USB_OK								When Successfull
  * 		USBERR_INVALID_REQ_TYPE 			when Error
  *****************************************************************************/
 uint8_t USB_Get_Abstract_State(uint32_t handle, 
								 uint8_t interface, 
								 uint8_t * *feature_data)
 {	 
	 UNUSED_ARGUMENT(handle)
	 /* if interface valid */
	 if(interface < USB_MAX_SUPPORTED_INTERFACES)
	 {
		 /* get line coding data*/
		 *feature_data = g_abstract_state[interface];
		 return USB_OK;  
	 }
	 
	 return USBERR_INVALID_REQ_TYPE;
 }
 
 /**************************************************************************//*!
  *
  * @name  USB_Get_Country_Setting
  *
  * @brief The function gets the current setting for communication feature
  * 												 (COUNTRY_CODE)
  * @param handle:		  handle	 
  * @param interface:	  interface number	   
  * @param feature_data:   output comm feature data 	
  *
  * @return USB_OK								When Successfull
  * 		USBERR_INVALID_REQ_TYPE 			when Error
  *****************************************************************************/
 uint8_t USB_Get_Country_Setting(uint32_t handle, 
									 uint8_t interface, 
									 uint8_t * *feature_data)
 {	 
	 UNUSED_ARGUMENT(handle)
	 /* if interface valid */
	 if(interface < USB_MAX_SUPPORTED_INTERFACES)
	 {
		 /* get line coding data*/
		 *feature_data = g_country_code[interface];
		 return USB_OK;  
	 }
	 
	 return USBERR_INVALID_REQ_TYPE;
 }
 
 /**************************************************************************//*!
  *
  * @name  USB_Set_Abstract_State
  *
  * @brief The function gets the current setting for communication feature
  * 												 (ABSTRACT_STATE)
  * @param handle:		  handle	 
  * @param interface:	  interface number	   
  * @param feature_data:   output comm feature data 	
  *
  * @return USB_OK								When Successfull
  * 		USBERR_INVALID_REQ_TYPE 			when Error
  *****************************************************************************/
 uint8_t USB_Set_Abstract_State(uint32_t handle, 
								 uint8_t interface, 
								 uint8_t * *feature_data)
 {	 
	 uint8_t count;
	 UNUSED_ARGUMENT(handle)
	 /* if interface valid */
	 if(interface < USB_MAX_SUPPORTED_INTERFACES)
	 {
		 /* set Abstract State Feature*/
		 for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
		 {			
			 g_abstract_state[interface][count] = *(*feature_data + count);
		 }
		 return USB_OK; 
	 }
	 
	 return USBERR_INVALID_REQ_TYPE;
 }
 
 /**************************************************************************//*!
  *
  * @name  USB_Set_Country_Setting
  *
  * @brief The function gets the current setting for communication feature
  * 												 (COUNTRY_CODE)
  * @param handle: handle	  
  * @param interface:	  interface number	   
  * @param feature_data:   output comm feature data 	
  *
  * @return USB_OK								When Successfull
  * 		USBERR_INVALID_REQ_TYPE 			when Error
  *****************************************************************************/
 uint8_t USB_Set_Country_Setting(uint32_t handle, 
									 uint8_t interface, 
									 uint8_t * *feature_data)
 {	 
	 uint8_t count;
	 UNUSED_ARGUMENT (handle)
	 
	 /* if interface valid */
	 if(interface < USB_MAX_SUPPORTED_INTERFACES)
	 {
		 for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
		 {			
			 g_abstract_state[interface][count] = *(*feature_data + count);
		 }
		 return USB_OK; 
	 }
	 
	 return USBERR_INVALID_REQ_TYPE;
 }
 /*****************************************************************************
 *	
 *	 @name		  VCom_TestApp_Init
 * 
 *	 @brief 	  This function initializes the App.
 * 
 *	 @param 	  None
 * 
 *	 @return	  None
 ** 			   
 *****************************************************************************/
void VCom_TestApp_Init(void)
{
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    g_curr_recv_buf = (uint8_t *)OS_Mem_alloc_uncached_align(DATA_BUFF_SIZE, 32);
    g_curr_send_buf = (uint8_t *)OS_Mem_alloc_uncached_align(DATA_BUFF_SIZE, 32);
#endif
    g_recv_size = 0;
    g_send_size= 0;    
}
  /*****************************************************************************
  *  
  *   @name 	   VCom_TestApp_Task
  * 
  *   @brief	   This function runs the App task.
  * 
  *   @param	   None
  * 
  *   @return	   None
  **				
  *****************************************************************************/
 void VCom_TestApp_Task(void)
 {
	  /* call the periodic task function */ 	 
	  USB_CDC_Periodic_Task();			 
  
	 /*check whether enumeration is complete or not */
	  if((start_app==TRUE) && (start_transactions==TRUE))
	  { 	   
		  Virtual_Com_App(); 
	  } 		   
 }

/******************************************************************************
 * 
 *    @name       Virtual_Com_App
 *    
 *    @brief      
 *                  
 *    @param      None
 * 
 *    @return     None
 *    
 *****************************************************************************/
void Virtual_Com_App(void)
{
    /* User Code */ 
    if(g_recv_size) 
    {
        int32_t i;
        
        /* Copy Buffer to Send Buff */
        for (i = 0; i < g_recv_size; i++)
        {
            //printf("Copied: %c\n", g_curr_recv_buf[i]);
        	g_curr_send_buf[g_send_size++] = g_curr_recv_buf[i];
        }
        g_recv_size = 0;
    }
    
    if(g_send_size) 
    {
        uint8_t error;
        uint8_t size = g_send_size;
        g_send_size = 0;

        error = USB_Class_CDC_Send_Data(g_vcom_handle, DIC_BULK_IN_ENDPOINT,
        	g_curr_send_buf, size);

        if(error != USB_OK) 
        {
            /* Failure to send Data Handling code here */
        } 
    }
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void VCom_USB_App_Callback(uint8_t event_type, void* val,void* arg) 
{
    uint32_t handle;
    handle = *((uint32_t *)arg);
    if(event_type == USB_DEV_EVENT_BUS_RESET)
    {
        start_app=FALSE;    
    }
    else if(event_type == USB_DEV_EVENT_CONFIG_CHANGED)
    {
        /* Schedule buffer for receive */
        USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE);
        start_app=TRUE;
    }
    else if(event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    return;
}

/******************************************************************************
 * 
 *    @name        USB_Notif_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle:  handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
 
uint8_t VCom_USB_Notif_Callback
(
    uint8_t event, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
	cdc_handle_t handle;
    uint8_t index;
	uint8_t error = USB_OK;
	handle = *((cdc_handle_t *)arg);
	switch(event)
	{
        case GET_LINE_CODING:
        error = USB_Get_Line_Coding(handle, value, data);
        break;
        case GET_ABSTRACT_STATE:
        error = USB_Get_Abstract_State(handle, value, data);
        break;
        case GET_COUNTRY_SETTING:
        error = USB_Get_Country_Setting(handle, value, data);
        break;
        case SET_LINE_CODING:
        error = USB_Set_Line_Coding(handle, value, data);
        break;
        case SET_ABSTRACT_STATE:
        error = USB_Set_Abstract_State(handle, value, data);
        break;
        case SET_COUNTRY_SETTING:
        error = USB_Set_Country_Setting(handle, value, data);
        break;
        case USB_APP_CDC_DTE_ACTIVATED:
        if(start_app == TRUE)
        {
            start_transactions = TRUE; 
        }
        break;
        case USB_APP_CDC_DTE_DEACTIVATED:
        if(start_app == TRUE)
        {
            start_transactions = FALSE; 
        }
        break;
        case USB_DEV_EVENT_DATA_RECEIVED:
        {
            uint32_t BytesToBeCopied; 
            if((start_app == TRUE) && (start_transactions == TRUE))
            {
                BytesToBeCopied = *size;
                for(index = 0; index < BytesToBeCopied; index++) 
                {
                    g_curr_recv_buf[index] = *(*data + index);
                    //printf("Received: %c\n", g_curr_recv_buf[index]);
                }
                g_recv_size = index;
            }
        }
        break;
        case USB_DEV_EVENT_SEND_COMPLETE:
        {
	        if ((size != NULL) && (*size != 0) && !(*size % DIC_BULK_IN_ENDP_PACKET_SIZE))
	        {
				/* If the last packet is the size of endpoint, then send also zero-ended packet,
				** meaning that we want to inform the host that we do not have any additional
				** data, so it can flush the output.
		             */
	            USB_Class_CDC_Send_Data(g_vcom_handle, DIC_BULK_IN_ENDPOINT, NULL, 0);
	        }else if((start_app == TRUE) && (start_transactions == TRUE))
            {
				if((size != NULL) || ((size == NULL) && (*size == 0)))
				{
					/* User: add your own code for send complete event */
					/* Schedule buffer for next receive event */
					USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 
				}
            }
        }
        break;
        default:
        error = USBERR_INVALID_REQ_TYPE;
    }

    return error;
}


/* EOF */
