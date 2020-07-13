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
* $FileName: disk.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  RAM Disk has been emulated via this Mass Storage Demo
*****************************************************************************/
 
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "disk.h"

#if SD_CARD_APP

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)

#include "sd_esdhc_kinetis.h"

#else

#include "sd.h"

#endif

#endif

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#if !USBCFG_DEV_MSC
#error This application requires USBCFG_DEV_MSC defined none zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#if HIGH_SPEED_DEVICE

#if !USBCFG_DEV_EHCI
#error This application requires USBCFG_DEV_EHCI defined none zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#endif

extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 8L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};

MQX_FILE_PTR             g_sdcard_handle;
#endif
/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/ 
 /* Add all the variables needed for disk.c to this structure */
extern usb_desc_request_notify_struct_t     desc_callback;
usb_application_callback_struct_t           msc_application_callback;
usb_vendor_req_callback_struct_t            vend_req_callback;
usb_class_specific_callback_struct_t        class_specific_callback;

msc_config_struct_t                         g_msd_config;
disk_struct_t                               g_disk;

#if SD_CARD_APP
	#define USE_SDHC_PROTOCOL    (1)
	#define USE_SPI_PROTOCOL     (0)
#endif

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg);
uint8_t MSD_Event_Callback
(   uint8_t event_type, 
    uint16_t value,  
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) ;
void Disk_App(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
#if SD_CARD_APP

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
 uint32_t g_msc_bulk_out_buff_temp[MSD_RECV_BUFFER_SIZE >> 2];
 uint32_t g_msc_bulk_in_buff_temp[MSD_SEND_BUFFER_SIZE >> 2];
#endif
 uint8_t *g_msc_bulk_out_buff;
 uint8_t *g_msc_bulk_in_buff;
#endif
/*****************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * 
 *    @name       Disk_App
 *    
 *    @brief      
 *                  
 *    @param      None
 * 
 *    @return     None
 *    
 *****************************************************************************/
void Disk_App(void)
{
    /* User Code */ 
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       pointer : 
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg) 
{ 
    if(event_type == USB_DEV_EVENT_BUS_RESET) 
    {
        g_disk.start_app=FALSE;    
    }
    else if(event_type == USB_DEV_EVENT_ENUM_COMPLETE) 
    {
        g_disk.start_app=TRUE;        
    }
    else if(event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    else if(event_type == USB_MSC_DEVICE_GET_SEND_BUFF_INFO)
    {
        if(NULL != val)
        {
#if SD_CARD_APP
            *((uint32_t *)val) = (uint32_t)MSD_SEND_BUFFER_SIZE;
#elif RAM_DISK_APP
            *((uint32_t *)val) = (uint32_t)DISK_SIZE_NORMAL;
#endif
        }
    }
    else if(event_type == USB_MSC_DEVICE_GET_RECV_BUFF_INFO)
    {
        if(NULL != val)
        {
#if SD_CARD_APP
            *((uint32_t *)val) = (uint32_t)MSD_RECV_BUFFER_SIZE;
#elif RAM_DISK_APP
            *((uint32_t *)val) = (uint32_t)DISK_SIZE_NORMAL;
#endif
        }
    }
    
    return;
}

/******************************************************************************
 * 
 *    @name        MSD_Event_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       pointer : 
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
uint8_t MSD_Event_Callback
(   uint8_t event_type, 
    uint16_t value,  
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    lba_app_struct_t* lba_data_ptr;
    uint8_t * prevent_removal_ptr; 
    //uint8_t * load_eject_start_ptr = NULL;   
    device_lba_info_struct_t* device_lba_info_ptr;
	uint8_t error = USB_OK;

    //UNUSED_ARGUMENT (arg)
            
    switch(event_type)
    {
        case USB_DEV_EVENT_DATA_RECEIVED :
            /* Add User defined code -- if required*/
            lba_data_ptr = (lba_app_struct_t*)size;

            #if RAM_DISK_APP
			#elif SD_CARD_APP
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
				SD_Write_Block(lba_data_ptr);
#else
                sd_write_device_sectors(g_sdcard_handle, (lba_data_ptr->offset >> SDCARD_BLOCK_SIZE_POWER),
                     (lba_data_ptr->size >> SDCARD_BLOCK_SIZE_POWER) , 3, lba_data_ptr->buff_ptr,NULL);
#endif
            #endif
            break;
        case USB_DEV_EVENT_SEND_COMPLETE :
            /* Add User defined code -- if required*/
            lba_data_ptr = (lba_app_struct_t*)size;
            /* read data from mass storage device to driver buffer */
            #if RAM_DISK_APP
                if(data != NULL)
                {
                    *data = g_disk.storage_disk + lba_data_ptr->offset;
                }
			#elif SD_CARD_APP
            #endif
            break;
        case USB_MSC_START_STOP_EJECT_MEDIA :
            /*  Code to be added by user for starting, stopping or 
                ejecting the disk drive. e.g. starting/stopping the motor in 
                case of CD/DVD*/
            break;
        case USB_MSC_DEVICE_READ_REQUEST :          
            lba_data_ptr = (lba_app_struct_t*)size;
            
            #if RAM_DISK_APP
                if(data != NULL)
                {
                    *data = g_disk.storage_disk + lba_data_ptr->offset;
                }
			#elif SD_CARD_APP
                if(data != NULL)
                {
                    *data = g_msc_bulk_in_buff;
                }
                lba_data_ptr->buff_ptr = g_msc_bulk_in_buff;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
     		 	SD_Read_Block(lba_data_ptr);
#else
                sd_read_device_sectors(g_sdcard_handle, (lba_data_ptr->offset >> SDCARD_BLOCK_SIZE_POWER),
                    (lba_data_ptr->size >> SDCARD_BLOCK_SIZE_POWER) , 3, lba_data_ptr->buff_ptr,NULL);
#endif
            #endif 
            break;
        case USB_MSC_DEVICE_WRITE_REQUEST :
            lba_data_ptr = (lba_app_struct_t*)size;
            #if RAM_DISK_APP
                if(data != NULL)
                {
                    *data = g_disk.storage_disk + lba_data_ptr->offset;
                }
			#elif SD_CARD_APP
                if(data != NULL)
                {
                    *data = g_msc_bulk_out_buff;
                }
            #endif
            break;
        case USB_MSC_DEVICE_FORMAT_COMPLETE :
            break;
        case USB_MSC_DEVICE_REMOVAL_REQUEST :
            prevent_removal_ptr = (uint8_t *) size;
            if(SUPPORT_DISK_LOCKING_MECHANISM)
            {                
                g_disk.disk_lock = *prevent_removal_ptr;
            }
            else if((!SUPPORT_DISK_LOCKING_MECHANISM)&&(!(*prevent_removal_ptr)))
            {
                /*there is no support for disk locking and removal of medium is enabled*/
                /* code to be added here for this condition, if required */ 
            }
            break;
        case USB_MSC_DEVICE_GET_INFO :
            device_lba_info_ptr = (device_lba_info_struct_t*)size;
            #if RAM_DISK_APP
                device_lba_info_ptr->total_lba_device_supports = TOTAL_LOGICAL_ADDRESS_BLOCKS_NORMAL;  
                device_lba_info_ptr->length_of_each_lab_of_device = LENGTH_OF_EACH_LAB; 
			#elif SD_CARD_APP
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
      		SD_Card_Info(&device_lba_info_ptr->total_lba_device_supports,
      		&device_lba_info_ptr->length_of_each_lab_of_device);
#else
            sd_get_block_size(g_sdcard_handle, &device_lba_info_ptr->length_of_each_lab_of_device);
            sd_get_blocks_num(g_sdcard_handle, &device_lba_info_ptr->total_lba_device_supports);
#endif
            #endif
            device_lba_info_ptr->num_lun_supported = LOGICAL_UNIT_SUPPORTED;
            break;          
        default : 
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
#if SD_CARD_APP

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    g_msc_bulk_in_buff = (uint8_t*)g_msc_bulk_in_buff_temp;
    g_msc_bulk_out_buff = (uint8_t*)g_msc_bulk_out_buff_temp;
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    g_msc_bulk_in_buff = (uint8_t*)OS_Mem_alloc_uncached_align(MSD_SEND_BUFFER_SIZE, 32);
    g_msc_bulk_out_buff = (uint8_t*)OS_Mem_alloc_uncached_align(MSD_SEND_BUFFER_SIZE, 32);
#endif

#endif

    printf("Enter Testapp_init\n");
#if SD_CARD_APP
    printf("Please insert SD card\n");
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
      #if (defined _MK_xxx_H_)
        #if USE_SDHC_PROTOCOL
        sdhc_detect_io_init();
        #endif
                  
        delay_init();
        
        while (!sdhc_detect())
        {
            #if DELAY_ENABLE
            time_delay(1000);
            #else
            for(int i =0 ;i < 1000000; i++){};
            #endif
            printf(".");
        }     /* SD Card inserted */
      #endif
        if(!SD_Init()) return; /* Initialize SD_CARD and SPI Interface */
#else
        g_sdcard_handle = sd_init();

#endif
    printf("\nSD card inserted!\n");

#endif
    OS_Mem_zero(&g_disk, sizeof(disk_struct_t));

    OS_Mem_zero(&g_msd_config,sizeof(msc_config_struct_t)); 

    msc_application_callback.callback = USB_App_Callback;
    msc_application_callback.arg = &g_disk.app_handle;
    
    /* Register the callbacks to lower layers */
    g_msd_config.msc_application_callback = msc_application_callback;
    g_msd_config.vendor_req_callback = vend_req_callback;
    g_msd_config.class_specific_callback.callback = MSD_Event_Callback;
	g_msd_config.class_specific_callback.arg = &g_disk.app_handle;
    g_msd_config.desc_callback_ptr = &desc_callback;

    /* Finally, Initialize the device and USB Stack layers*/
    USB_Class_MSC_Init(CONTROLLER_ID,&g_msd_config,&g_disk.app_handle);

#if 0
    while (TRUE) 
    {
        /* call the periodic task function */      
        USB_MSC_Periodic_Task();           

       /*check whether enumeration is complete or not */
        if(g_disk.start_app==TRUE)
        {        
            Disk_App(); 
        }            
    }/* Endwhile */   
#endif   
} 

void APP_task()
{
	 Disk_App(); 
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls Test_App
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
