/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013- 2014 Freescale Semiconductor;
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
* $FileName: mouse.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file emulates a mouse cursor movements
*         
*****************************************************************************/
 
/******************************************************************************
 * Includes
 *****************************************************************************/
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#include "user_config.h"
#endif
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "mouse.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
//#include "board.h"
#include "fsl_debug_uart.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(FRDM_MK64FN1M0VMD12)
#define BOARD_DEBUG_UART_INSTANCE  0
#else
#define BOARD_DEBUG_UART_INSTANCE  1
#endif

#define DEBUG_UART_BAUD (115200)
#endif

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (FSL_RTOS_MQX)))
static void Main_Task(uint32_t param);
#define MAIN_TASK       10

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L , 0, 0}
};
#endif
/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
/*void TestApp_Init(void);*/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for mouse.c to this structure */
extern usb_desc_request_notify_struct_t     g_desc_callback;
static mouse_global_variable_struct_t       g_mouse;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg); 
uint8_t USB_App_Param_Callback(uint8_t request, uint16_t value, uint8_t ** data, 
    uint32_t* size,void* arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*****************************************************************************
* 
*      @name     move_mouse
*
*      @brief    This function gets makes the cursor on screen move left,right
*                up and down
*
*      @param    None      
*
*      @return   None
*     
* 
******************************************************************************/
static void move_mouse(void) 
{
    static int8_t x = 0, y = 0;  
    static enum { RIGHT, DOWN, LEFT, UP } dir = RIGHT;
    
    switch (dir) 
    {
        case RIGHT:
            g_mouse.rpt_buf[1] = 2; 
            g_mouse.rpt_buf[2] = 0; 
            x++;
            if (x > 100)
            {
                dir++;
            }
            break;
        case DOWN:
            g_mouse.rpt_buf[1] = 0; 
            g_mouse.rpt_buf[2] = 2; 
            y++;
            if (y > 100)
            {
                dir++;
            }
            break;
        case LEFT:
            g_mouse.rpt_buf[1] = (uint8_t)(-2); 
            g_mouse.rpt_buf[2] = 0; 
            x--;
            if (x < 0)
            {
                dir++;
            }
            break;
        case UP:
            g_mouse.rpt_buf[1] = 0; 
            g_mouse.rpt_buf[2] = (uint8_t)(-2); 
            y--;
            if (y < 0)
            {
                dir = RIGHT;
            }
            break;
    }    
    (void)USB_Class_HID_Send_Data(g_mouse.app_handle,HID_ENDPOINT,
        g_mouse.rpt_buf,MOUSE_BUFF_SIZE);
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
void USB_App_Callback(uint8_t event_type, void* val,void* arg) 
{    
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)
    
    switch(event_type)
    {
        case USB_DEV_EVENT_BUS_RESET:
            g_mouse.mouse_init=FALSE;
            break;
        case USB_DEV_EVENT_ENUM_COMPLETE:
            g_mouse.mouse_init = TRUE;
            move_mouse();/* run the coursor movement code */
            break;
        case USB_DEV_EVENT_ERROR: 
            /* user may add code here for error handling 
               NOTE : val has the value of error from h/w*/
            break;
        default: 
            break;
    }   
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Param_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
uint8_t USB_App_Param_Callback
(
    uint8_t request, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    uint8_t error = USB_OK;
    uint8_t index = (uint8_t)((request - 2) & USB_HID_REQUEST_TYPE_MASK); 

    if ((request == USB_DEV_EVENT_SEND_COMPLETE) && (value == USB_REQ_VAL_INVALID))
    {
        if((g_mouse.mouse_init)&& (arg != NULL))
        {
            #if COMPLIANCE_TESTING
                    uint32_t g_compliance_delay = 0x009FFFFF;
                    while(g_compliance_delay--);
            #endif
          move_mouse();/* run the coursor movement code */
        }
        return error;
    }
    /* index == 0 for get/set idle, index == 1 for get/set protocol */
    *size = 0;
    /* handle the class request */
    switch(request) 
    {
        case USB_HID_GET_REPORT_REQUEST :
            *data = &g_mouse.rpt_buf[0]; /* point to the report to send */
            *size = MOUSE_BUFF_SIZE; /* report size */
            break;
              
        case USB_HID_SET_REPORT_REQUEST :
            for(index = 0; index < MOUSE_BUFF_SIZE ; index++) 
            {   /* copy the report sent by the host */
                g_mouse.rpt_buf[index] = *(*data + index);
            }
            break;

        case USB_HID_GET_IDLE_REQUEST :
            /* point to the current idle rate */
            *data = &g_mouse.app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;
                
        case USB_HID_SET_IDLE_REQUEST :
            /* set the idle rate sent by the host */
            g_mouse.app_request_params[index] =(uint8_t)((value & MSB_MASK) >> 
                HIGH_BYTE_SHIFT);
            break;

        case USB_HID_GET_PROTOCOL_REQUEST :
            /* point to the current protocol code 
               0 = Boot Protocol
               1 = Report Protocol*/
            *data = &g_mouse.app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;

        case USB_HID_SET_PROTOCOL_REQUEST :
            /* set the protocol sent by the host 
               0 = Boot Protocol
               1 = Report Protocol*/
               g_mouse.app_request_params[index] = (uint8_t)(value);  
               break;
    }
    return error; 
}

/******************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for mouse (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void APP_init(void)
{       
    hid_config_struct_t   config_struct;
    
    /* initialize the Global Variable Structure */
    OS_Mem_zero(&g_mouse, sizeof(mouse_global_variable_struct_t));
    OS_Mem_zero(&config_struct, sizeof(hid_config_struct_t));
   
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    g_mouse.rpt_buf = (uint8_t*)OS_Mem_alloc_uncached_align(MOUSE_BUFF_SIZE, 32);
    if(NULL == g_mouse.rpt_buf)
    {
        printf("\nMalloc error in APP_init\n");
        return;
    }
    OS_Mem_zero(g_mouse.rpt_buf,MOUSE_BUFF_SIZE);
#endif
    /* Initialize the USB interface */
    printf("begin to test mouse\r\n");
    config_struct.hid_application_callback.callback = USB_App_Callback;
    config_struct.hid_application_callback.arg = &g_mouse.app_handle;
    config_struct.class_specific_callback.callback = USB_App_Param_Callback;
    config_struct.class_specific_callback.arg = &g_mouse.app_handle;
    config_struct.desc_callback_ptr = &g_desc_callback;

    USB_Class_HID_Init(CONTROLLER_ID, &config_struct, &g_mouse.app_handle);    
} 


void APP_task()
{
    USB_HID_Periodic_Task(); 
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (FSL_RTOS_MQX)))

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task( uint32_t param )
{   
    UNUSED_ARGUMENT (param)
    APP_init();
}
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (!defined (FSL_RTOS_MQX))

static void init_debug_uart_hardware(void)
{
    uint32_t i;

    /* Port Clock enable*/
    for (i = 0; i < HW_PORT_INSTANCE_COUNT; i++)
    {
        clock_manager_set_gate(kClockModulePORT, i, true);
    }

    /* Enable the pins for the selected UART */
    #if defined(FRDM_MK64FN1M0VMD12)
    //clock_manager_set_gate(kClockModuleUART, 2U, true);
    /* configure uart5 to uart tx/rx */
    BW_SIM_SCGC4_UART0(1U);
    BW_PORT_PCRn_MUX(HW_PORTB, 16U, 3U);
    BW_PORT_PCRn_MUX(HW_PORTB, 17U, 3U);
    #elif defined(CPU_MK64FN1M0VMD12)
    BW_SIM_SCGC4_UART1(1U);
    BW_PORT_PCRn_MUX(HW_PORTC, 3U, 3U);
    BW_PORT_PCRn_MUX(HW_PORTC, 4U, 3U);
    #elif defined(CPU_MK22FN512VDC12)
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    BW_PORT_PCRn_MUX(HW_PORTE, 0U, 3U); 
    BW_PORT_PCRn_MUX(HW_PORTE, 1U, 3U); 
    #endif
    
    /* Init uart driver for stdio. */
    debug_uart_init(BOARD_DEBUG_UART_INSTANCE, DEBUG_UART_BAUD);
}

static void Task_Start(void *arg)
{
    APP_init();
#if defined (FSL_RTOS_FREE_RTOS)   
    while (1)
    {
        vTaskDelay(2000/portTICK_RATE_MS);
    }
#endif
}

int main(void)
{
#if defined (FSL_RTOS_UCOSII)
    systick_init();
    init_debug_uart_hardware();
    OSInit();
    //BSP_Init();
    //BSP_Tick_Init();
    //sci_init();
    OS_Task_create(Task_Start, NULL, 9L, 3000L, "Main", NULL);
    OSStart();
#elif defined (FSL_RTOS_UCOSIII)
    OS_ERR err;

    OSInit(&err);
	init_debug_uart_hardware();
	systick_init();

    OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err); /* Enable task round robin. */

    OS_Task_create(Task_Start, NULL, 9L, 3000L, "Main", NULL);

    OSStart(&err);
#elif defined (FSL_RTOS_FREE_RTOS)
    init_debug_uart_hardware();
    OS_Task_create(Task_Start, NULL, 8L, 300L, "Main", NULL);
    vTaskStartScheduler();
#else
    init_debug_uart_hardware();
    sw_timer_init_service();

    POLL_init();

    APP_init();
    /*
    ** Infinite loop, waiting for events requiring action
    */
    for ( ; ; ) 
    {
        //APP_task();
        Poll();
    } /* Endfor */
#endif
    return 1;
}
#endif
/* EOF */
