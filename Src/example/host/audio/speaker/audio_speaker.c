/**HEADER********************************************************************
*
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: audio_speaker.c$
* $Version : 3.8.3.0$
* $Date    : Aug-9-2012$
*
* Comments:
*
*   This file is an example of device drivers for the Audio host class. This example
*   demonstrates the audio transfer capability of audio host class with audio devices.
*
*END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#else
#include "derivative.h"
#include "hidef.h"
#include "mem_util.h"
#include "rtc_kinetis.h"
#endif
#include "usb_host_audio.h"

#include "usb_host_hub_sm.h"

#include "audio_speaker.h"
#include "hidkeyboard.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "sd_card.h"
#include <hwtimer.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif
#endif

/***************************************
**
** Macros
****************************************/

/***************************************
**
** Global functions
****************************************/

/***************************************
**
** Local functions
****************************************/
static usb_status check_device_type (usb_audio_ctrl_desc_it_t*,usb_audio_ctrl_desc_ot_t*, char * *,uint8_t *);
static void USB_Prepare_Data(void);

/***************************************
**
** Global variables
****************************************/
uint8_t                                      device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
uint8_t                                   wav_buff[MAX_ISO_PACKET_SIZE];
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
uint8_t                                   sd_buff_1[MAX_SD_READ]; /* sd card buffer */
uint8_t                                   sd_buff_2[MAX_SD_READ];
#endif
uint32_t                           packet_size; /* number of bytes the host send to the device each mS */
uint8_t                                   resolution_size;
uint8_t                                   audio_state = AUDIO_IDLE;
uint32_t                                   read_data_err = FALSE;
audio_control_device_struct_t      audio_control = { 0 };
audio_control_device_struct_t      audio_stream  = { 0 };
usb_audio_stream_desc_format_type_t*     frm_type_desc = NULL;
usb_audio_ctrl_desc_fu_t*                fu_desc = NULL;
audio_command_t audio_com;
static uint16_t                  cur_volume, min_volume, max_volume, res_volume;
static uint16_t                  physic_volume;
uint8_t                            host_cur_volume = 5;
uint16_t                          device_volume_step;
/* Following are buffers for USB, should be aligned at cache boundary */
uint8_t *g_cur_mute;
uint8_t *g_cur_vol;
uint8_t *g_max_vol;
uint8_t *g_min_vol;
uint8_t *g_res_vol;

volatile uint32_t                          buffer_1_free = 1; /* buffer is free */
volatile uint32_t                          buffer_2_free = 1;
volatile uint8_t                           buffer_read = 1; /* buffer need to be read */
extern uint8_t                             file_open_count;
extern uint8_t                             sd_card_state;
//extern volatile USB_KEYBOARD_DEVICE_STRUCT kbd_hid_device;
extern os_event_handle                     usb_audio_fu_request;
extern os_event_handle                     usb_keyboard_event;
extern os_event_handle                     sd_card_event;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
extern FILE_PTR                            file_ptr;
HWTIMER                                    audio_timer;/* hwtimer handle */
extern const HWTIMER_DEVIF_STRUCT          BSP_HWTIMER1_DEV;
#define keyboard_task_fuc keyboard_task_stun
#define timer_task_fuc timer_task_stun
#else
extern const unsigned char wav_data[];
extern const uint16_t wav_size;
#define keyboard_task_fuc keyboard_task
#define timer_task_fuc timer_task
static   timer_object_t time_obj;
static   uint8_t   time_index;

#endif

os_event_handle USB_ctr_Event;
//#define USB_EVENT_CTRL           (0x01)



/***************************************
**
** Local variables
****************************************/
static endpoint_descriptor_t*                 endp;
/* Transfer Types */
static char *TransferType[4] =
{
    "Control",
    "Isochronous",
    "Bulk",
    "Interrupt"
};
/* Sync Types */
static char *SyncType[4] =
{
    "No synchronization",
    "Asynchronous",
    "Adaptive",
    "Synchrounous"
};
/* Data Types */
static char *DataType[4] =
{
    "Data endpoint",
    "Feedback endpoint",
    "Implicit feedback",
    "Reserved"
};

/* Input Terminal types */
static char *it_type_string[NUMBER_OF_IT_TYPE] =
{
    "Input Undefined",
    "Microphone",
    "Desktop Microphone",
    "Personal Microphone",
    "Omni directional Microphone",
    "Microphone array",
    "Processing Microphone array",
};

/* Output Terminal types */
static char *ot_type_string[NUMBER_OF_OT_TYPE] =
{
    "Output Undefined",
    "Speaker",
    "Headphones",
    "Head Mounted Display Audio",
    "Desktop Speaker",
    "Room Speaker",
    "Communication Speaker",
    "Low Frequency Effects Speaker",
};
static char             *device_string;
uint8_t                 g_interface_control_number = 0;
uint8_t                 g_interface_stream_number = 0;


usb_device_interface_struct_t*   g_interface_control_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
usb_device_interface_struct_t*   g_interface_stream_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];

/*
** Globals
*/

/* Table of driver capabilities this application wants to use */
static  usb_host_driver_info_t DriverInfoTable[] =
{
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_CONTROL,     /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_control_event  /* Application call back function   */
    },
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_STREAMING,   /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_stream_event   /* Application call back function   */
    },  
	{
	     {0x00,0x00},                  /* Vendor ID per USB-IF             */
	     {0x00,0x00},                  /* Product ID per manufacturer      */
	     USB_CLASS_HID,                /* Class code                       */
	     USB_SUBCLASS_HID_NONE,        /* Sub-Class code                   */
	     USB_PROTOCOL_HID_NONE,        /* Protocol                         */
	     0,                            /* Reserved                         */
	     usb_host_hid_keyboard_event      /* Application call back function   */
	},
    /* USB 1.1 hub */
   {

       {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
       USB_CLASS_HUB,                /* Class code                       */
       USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
       USB_PROTOCOL_HUB_ALL,          /* Protocol                         */
      0,                            /* Reserved                         */
       usb_host_hub_device_event     /* Application call back function   */
   },
    {
        {0x00,0x00},                  /* All-zero entry terminates        */
        {0x00,0x00},                  /* driver info list.                */
        0,
        0,
        0,
        0,
        NULL
    }
};

usb_host_handle        host_handle;

#define MAX_SUPPORTED_USAGE_ID 40

#define MAIN_TASK          (10)
#define AUDIO_TASK         (12)
#define HID_KEYB_TASK      (11)
#define SDCARD_TASK        (13)
#define SHELL_TASK         (14)

extern void main_task(uint32_t param);
extern void audio_task(uint32_t);
extern void sdcard_task(uint32_t temp);
extern void shell_task(uint32_t temp);
extern void keyboard_task(uint32_t param);
extern void keyboard_task_stun(uint32_t param);
extern void timer_task(uint32_t param);
extern void timer_task_stun(uint32_t param);

os_event_handle                         usb_audio_fu_request;
os_event_handle                         usb_keyboard_event;
os_event_handle                         sd_card_event;
os_event_handle                         usb_timer_event;
#define timer_out_event           0x01
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,        main_task,      2000L,  8L,  "Main",      MQX_AUTO_START_TASK},
 //   { AUDIO_TASK,       Audio_Task,     4000L,  10L, "Audio",     MQX_USER_TASK},
    { HID_KEYB_TASK,    keyboard_task_stun,  4000L,  9L,  "Keyboard",  MQX_USER_TASK},
    { HID_KEYB_TASK,    timer_task_stun,  1000L,  9L,  "Timer",  MQX_USER_TASK},
    { SDCARD_TASK,      sdcard_task,    2000L,  11L, "Sdcard",    MQX_USER_TASK},
    { SHELL_TASK,       shell_task,     2000L,  12L, "Shell",     MQX_USER_TASK},
    { 0L,                0L,             0L,     0L, 0L,          0L }
};
#endif
static int errcount = 0;
#define AUDIO_SPEAKER_FREQUENCY (1000) /* Frequency in Hz*/
/*FUNCTION*---------------------------------------------------------------
*
* Function Name : audio_timer_isr
* Comments  : Callback called by hwtimer every elapsed period
*
*END*----------------------------------------------------------------------*/
void audio_timer_isr(void *p)
{
	OS_Event_set(usb_timer_event, timer_out_event);
}

void timer_task(uint32_t param)
{	
	if (OS_Event_check_bit(usb_timer_event, timer_out_event))
    {
		OS_Event_clear(usb_timer_event, timer_out_event);
		audio_com.callback_fn = usb_host_audio_tr_callback;
        audio_com.callback_param = 0;
        {
            if(USB_OK != usb_class_audio_send_data(&audio_com,(unsigned char *)wav_buff,packet_size))
            errcount ++;
        }
        audio_stream.dev_state = USB_DEVICE_IDLE;
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task_stun
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
void timer_task_stun(uint32_t param)
{
    while(1)
    {
		OS_Event_wait(usb_timer_event, timer_out_event, FALSE, 0);
		timer_task(param);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void APP_init(void)
{ /* Body */
    usb_status           status = USB_OK;
    uint32_t task_id;
   #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
	usb_int_dis();
#endif 
    /*
    ** It means that we are going to act like host, so we initialize the
    ** host stack. This call will allow USB system to allocate memory for
    ** data structures, it uses later (e.g pipes etc.).
    */
	status = usb_host_init(CONTROLLER_ID,							/*  */
			&host_handle);							/* Returned pointer */
    if (status != USB_OK) 
    {
        printf("\nUSB Host Initialization failed. STATUS: %x", status);

    }
    /*
    ** Since we are going to act as the host driver, register the driver
    ** information for wanted class/subclass/protocols
    */
    status = usb_host_register_driver_info (
    host_handle,
    DriverInfoTable
    );
    if (status != USB_OK) {
        printf("\nDriver Registration failed. STATUS: %x", status);
    }

    /* Creat lwevents*/
    usb_keyboard_event = OS_Event_create(0);/* manually clear */
    usb_audio_fu_request = OS_Event_create(0);/* manually clear */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
   sd_card_event = OS_Event_create(0);/* manually clear */
#endif
  #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
	usb_int_en();
#endif     
    if (NULL == (g_cur_mute = (uint8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
      printf("\n memory allocation failed.\n");
      return;
    }
    if (NULL == (g_cur_mute = (uint8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
      printf("\n memory allocation failed.\n");
      return;
    }
    if (NULL == (g_cur_vol = (uint8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
      printf("\n memory allocation failed.\n");
      return;
    }
    if (NULL == (g_max_vol = (uint8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
      printf("\n memory allocation failed.\n");
      return;
    }
    if (NULL == (g_min_vol = (uint8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
      printf("\n memory allocation failed.\n");
      return;
    }
    if (NULL == (g_res_vol = (uint8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
      printf("\n memory allocation failed.\n");
      return;
    }
    USB_ctr_Event = OS_Event_create(0);
    usb_timer_event = OS_Event_create(0);

    task_id = OS_Task_create((task_start_t)keyboard_task_fuc, (void*)host_handle, (uint32_t)9, 2000, (char*)"Keyboard", NULL);
    if (task_id == 0) {
        return;
    }

	task_id = OS_Task_create((task_start_t)timer_task_fuc, (void*)host_handle, (uint32_t)9, 1000, (char*)"timer", NULL);
    if (task_id == 0) {
       return;
    }
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    task_id = OS_Task_create((task_start_t)sdcard_task, (void*)host_handle, 10, 2000L, "sd card", NULL);
    if (task_id == 0) {
        return;
    }

    task_id = OS_Task_create((task_start_t)shell_task, (void*)host_handle, 11, 2000L, "shell", NULL);
    if (task_id == 0) {
        return;
    }

#endif
    printf("\n  USB Audio Speaker Host Demo\n");
    /* The main task has done its job, so exit */
} /* Endbody */
typedef enum
{
    AUDIO_GET_MIN_VOLUME,
    AUDIO_GET_MAX_VOLUME,
    AUDIO_GET_RES_VOLUME,
    AUDIO_CONFIG_CHANNEL,
    AUDIO_DONE,
    AUDIO_NONE
} audio_control_state_t;
static audio_control_state_t audio_statue = AUDIO_GET_MIN_VOLUME;
static feature_control_struct_t control_feature;
uint32_t   freq;
void APP_task()
{
    usb_status              status = USB_OK;

    uint8_t bsamfreqtype_index;
    if (OS_Event_check_bit(USB_ctr_Event,USB_EVENT_CTRL)) 
    {
        OS_Event_clear(USB_ctr_Event,USB_EVENT_CTRL);
        status = usb_host_open_dev_interface(host_handle, audio_control.dev_handle, audio_control.intf_handle, (void *)&audio_control.class_handle);
        if (status != USB_OK)
        {
            printf("\nError in _usb_host_open_dev_interface: %x\n", status);
            return;
        } /* Endif */
        audio_stream.dev_state = USB_DEVICE_INTERFACED;
    }

    if (USB_DEVICE_INTERFACED == audio_stream.dev_state)
    {
        audio_com.class_control_handle = (class_handle)audio_control.class_handle;
        audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
        control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
        audio_com.callback_param = (void *)&control_feature;

        /* Get min volume */
        switch( audio_statue )
        {
            case AUDIO_GET_MIN_VOLUME:
				OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                if(USB_OK == usb_class_audio_feature_command(&audio_com,1,(void *)g_min_vol,USB_AUDIO_GET_MIN_VOLUME))
                    audio_statue = AUDIO_GET_MAX_VOLUME;
                else
                    audio_statue = AUDIO_DONE;
                printf("AUDIO_GET_MIN_VOLUME\n");
                break;

            case AUDIO_GET_MAX_VOLUME:
                if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
                {
                    OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                    /* Get max volume */
                    if(USB_OK == usb_class_audio_feature_command(&audio_com,1,(void *)g_max_vol,USB_AUDIO_GET_MAX_VOLUME))
                        audio_statue = AUDIO_GET_RES_VOLUME;
                    else
                        audio_statue = AUDIO_DONE;
                    printf("AUDIO_GET_MAX_VOLUME\n");
                }
                break;

            case AUDIO_GET_RES_VOLUME:
                /* Get res volume */
                if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
                {
					OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
					if(USB_OK == usb_class_audio_feature_command(&audio_com,1,(void *)g_res_vol,USB_AUDIO_GET_RES_VOLUME))
						audio_statue = AUDIO_CONFIG_CHANNEL;
					else
						audio_statue = AUDIO_DONE;
                }
                break;

            case AUDIO_CONFIG_CHANNEL:
				if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
				{
					min_volume = (g_min_vol[1]<<8)|(g_min_vol[0]);
                    max_volume = (g_max_vol[1]<<8)|(g_max_vol[0]);
                    res_volume = (g_res_vol[1]<<8)|(g_res_vol[0]);
                    printf("  %x  %x  Res %x \n", (uint32_t)*(uint32_t *)g_min_vol, (uint32_t)*(uint32_t *)g_max_vol ,res_volume);
                    control_feature.FU = 0;
                    /* Synchronize host volume and device volume */
                    device_volume_step = (uint16_t)(((uint16_t)(max_volume) - (uint16_t)(min_volume))/(HOST_MAX_VOLUME - HOST_MIN_VOLUME));
                    cur_volume = (uint16_t)(min_volume + device_volume_step*host_cur_volume);
                    /* Calculate physical volume(dB) */
                    physic_volume = ((uint16_t)(cur_volume)*39)/10000;
                    g_cur_vol[0] = (uint8_t)((uint16_t)(cur_volume)&0x00FF);
                    g_cur_vol[1] = (uint8_t)((uint16_t)(cur_volume)>>8);
					config_channel(fu_desc,frm_type_desc->bnrchannels);
					audio_statue = AUDIO_DONE;
				}
                break;

            case AUDIO_DONE:
                /* Audio device information */
                printf("Audio device information:\n");
                printf("   - Device type    : %s\n", device_string);
                for (bsamfreqtype_index =0; bsamfreqtype_index < frm_type_desc->bsamfreqtype; bsamfreqtype_index++)
                {
                    printf("   - Frequency device support      : %d Hz\n", ((frm_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16) |
                    ((frm_type_desc->tsamfreq[bsamfreqtype_index][1]) << 8)  |
                    ((frm_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0));
                }
                    printf("   - Bit resolution : %d bits\n", frm_type_desc->bbitresolution);
                    printf("   - Number of channels : %d channels\n", frm_type_desc->bnrchannels);
                    printf("   - Transfer type : %s\n", TransferType[(endp->bmAttributes)&EP_TYPE_MASK]);
                    printf("   - Sync type : %s\n", SyncType[(endp->bmAttributes>>2)&EP_TYPE_MASK]);
                    printf("   - Usage type : %s\n", DataType[(endp->bmAttributes>>4)&EP_TYPE_MASK]);
                if (device_direction == USB_AUDIO_DEVICE_DIRECTION_OUT)
                {
                    printf("The device is unsupported!\n");
                }
                else
                {
                    printf("This audio device supports play audio files with these properties:\n");
                    printf("   - Sample rate    :\n");
                    for (bsamfreqtype_index =0; bsamfreqtype_index < frm_type_desc->bsamfreqtype; bsamfreqtype_index++)
                    {
                        printf("                    : %d Hz\n", ((frm_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16) |
                        ((frm_type_desc->tsamfreq[bsamfreqtype_index][1]) << 8)  |
                        ((frm_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0));
                    }
                    printf("   - Sample size    : %d bits\n", frm_type_desc->bbitresolution);
                    printf("   - Number of channels : %d channels\n", frm_type_desc->bnrchannels);
#if  (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
                    printf("Type play command to play audio files:\n");
                    for (bsamfreqtype_index =0; bsamfreqtype_index < frm_type_desc->bsamfreqtype; bsamfreqtype_index++)
                    {
                        printf("Type play a:\\%dk_%dbit_%dch.wav to play the file\n",\
                        (((frm_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16) |
                        ((frm_type_desc->tsamfreq[bsamfreqtype_index][1]) << 8)  |
                        ((frm_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0))/1000,\
                        frm_type_desc->bbitresolution,\
                        frm_type_desc->bnrchannels);
                    }
                    printf("\nCurrent physical volume: %d dB\n",physic_volume);
#else
					 printf("USB Speaker example will loop playback %dk_%dbit_%dch format aduio.\n",\
	                        (((frm_type_desc->tsamfreq[0][2]) << 16) |
	                        ((frm_type_desc->tsamfreq[0][1]) << 8)  |
	                        ((frm_type_desc->tsamfreq[0][0]) << 0))/1000,\
	                        frm_type_desc->bbitresolution,\
	                        frm_type_desc->bnrchannels);
#endif
                }
                freq = ((frm_type_desc->tsamfreq[0][2]) << 16) |
                        ((frm_type_desc->tsamfreq[0][1]) << 8)  |
                        ((frm_type_desc->tsamfreq[0][0]) << 0);
                usb_class_audio_endpoint_command(&audio_com,&freq, USB_AUDIO_SET_CUR_SAMPLING_FREQ);
                /* After get information of audio inteface, make HID interface is ready to use */   
                audio_stream.dev_state = USB_DEVICE_INUSE;
                packet_size = USB_Audio_Get_Packet_Size(frm_type_desc, 0);
#if  (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
                if (USB_OK != hwtimer_init(&audio_timer, &BSP_HWTIMER1_DEV, BSP_HWTIMER1_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
                {
                    printf("\n hwtimer initialization failed.\n");
                    return;
                }
                hwtimer_set_freq(&audio_timer, BSP_HWTIMER1_SOURCE_CLK, AUDIO_SPEAKER_FREQUENCY);
                hwtimer_callback_reg(&audio_timer,(HWTIMER_CALLBACK_FPTR)audio_timer_isr, NULL);

#else
                USB_Prepare_Data();
                //pit0_init();
                //EnableTimer1Interrupt();
                TimerQInitialize(0);
                time_obj.ms_count = 1;
                time_obj.pfn_timer_callback = (pfntimer_callback_t)audio_timer_isr;
                time_index = AddTimerQ(&time_obj, NULL);
#endif 
                audio_statue = AUDIO_GET_MIN_VOLUME;
                break;

            default:
                break;

        }
    }
#if  (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (AUDIO_PLAYING == audio_state)
    {
        /* Check if SD card disconnected */
        if ((SD_CARD_READY != sd_card_state) || read_data_err)
        {
        printf("  Error: Can't read audio file\n");
        hwtimer_stop(&audio_timer);
        audio_state = AUDIO_IDLE;
        fclose(file_ptr);
        OS_Event_set(sd_card_event, SD_CARD_EVENT_CLOSE);
        file_open_count --;
        read_data_err = FALSE;
        }
        else if(!feof(file_ptr))
        {
            /* Check if audio speaker disconnected */
            if (USB_DEVICE_DETACHED == audio_stream.dev_state)
            {
                printf("	Error: Audio Speaker is disconnected\n");
                hwtimer_stop(&audio_timer);
                audio_state = AUDIO_IDLE;
                fclose(file_ptr);
                OS_Event_set(sd_card_event, SD_CARD_EVENT_CLOSE);
                file_open_count --;
            }
            if(1==buffer_1_free)
            {
                /* write data to buffer 1 */
                if (fread(sd_buff_1,1,MAX_SD_READ,file_ptr) < 0) read_data_err = TRUE;
                buffer_1_free=0;
            }
            if(1==buffer_2_free)
            {
                /* write data to buffer 2 */
                if (fread(sd_buff_2,1,MAX_SD_READ,file_ptr) < 0) read_data_err = TRUE;
                buffer_2_free=0;
            }
        }
        else
        {
            hwtimer_stop(&audio_timer);
            printf("\nFinished");
            audio_state = AUDIO_IDLE;
            fclose(file_ptr);
            file_open_count --;
        }
    }
    OS_Time_delay(1);
#endif	
}  
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/

void main_task ( uint32_t param )
{
    APP_init();
   
    /*
    ** Infinite loop, waiting for events requiring action
    */
    for ( ; ; ) {
    	APP_task(); 
    	
    } /* Endfor */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Prepare_Data
* Returned Value : None
* Comments       :
*    This function prepares data to send.
*
*END*--------------------------------------------------------------------*/
#if   (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
static void USB_Prepare_Data(void)
{
    static uint32_t read_count=0;
    uint32_t i;
    for(i=0;i<packet_size;i++)
    {
        if(1==buffer_read) /* read data from buffer 1 */
        {
            wav_buff[i]=sd_buff_1[read_count];
            read_count++;
            if (MAX_SD_READ==read_count)
            {
                read_count=0;
                buffer_1_free=1;
                buffer_read = 0;
            }
        }
        else                 /* read data from buffer 2 */
        {
            wav_buff[i]=sd_buff_2[read_count];
            read_count++;
            if (MAX_SD_READ==read_count)
            {
                read_count=0;
                buffer_2_free=1;
                buffer_read = 1;
            }
        }
    }
}
#else
static void USB_Prepare_Data(void)
{
   uint32_t resolution_size = packet_size >> 5;
   static uint32_t audio_position = 0;
   uint8_t k, j=0;
   /* copy data to buffer */

   for(k = 0; k < 32; k++, audio_position++)
   {
		 for(j = 0; j < resolution_size; j++)
			 wav_buff[j*32+k] = wav_data[audio_position];

   }

	if(audio_position >= 140000)
		audio_position = 0;
}

#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_device_type
* Returned Value : None
* Comments       :
*    This function check whether the attached device is out-device or in-device.
*
*END*--------------------------------------------------------------------*/
usb_status check_device_type
(
/* [IN] Input terminal descriptor */
usb_audio_ctrl_desc_it_t*      it_desc,

/* [IN] Output terminal descriptor */
usb_audio_ctrl_desc_ot_t*      ot_desc,

/* [OUT] Terminal type name */
char *                  *device_type,

/* [OUT] device direction */
uint8_t                        *direction
)
{
    unsigned char it_type_high, it_type_low, ot_type_low;

    it_type_high = it_desc->wterminaltype[0];
    it_type_low  = it_desc->wterminaltype[1];
    ot_type_low  = ot_desc->wterminaltype[1];

    /* Input terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == it_type_low)
    {
        *direction = USB_AUDIO_DEVICE_DIRECTION_IN;
    }
    /* Input terminal type */
    else if (INPUT_TERMINAL_TYPE == it_type_low)
    {
        /* get type device name */
        *device_type = it_type_string[it_type_high];
    }
    else
    {
        return USBERR_ERROR;
    }

    /* Output terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == ot_type_low)
    {
        if (USB_AUDIO_DEVICE_DIRECTION_IN == (*direction))
        {
            *direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
            return USBERR_ERROR;
        }
        else
        {
            *direction = USB_AUDIO_DEVICE_DIRECTION_OUT;
        }
    }
    /* Output terminal type */
    else if (OUTPUT_TERMINAL_TYPE == ot_type_low)
    {
        /* get type device name */
        *device_type = ot_type_string[it_type_high];
    }
    else
    {
        return USBERR_ERROR;
    }
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Audio_Get_Packet_Size
* Returned Value : None
* Comments       :
*     This function gets the packet size to send to the device each mS.
*
*END*--------------------------------------------------------------------*/
uint32_t USB_Audio_Get_Packet_Size
(
/* [IN] Point to format type descriptor */
usb_audio_stream_desc_format_type_t* format_type_desc,
uint8_t                                bsamfreqtype_index
)
{
    uint32_t packet_size_tmp;
    /* calculate packet size to send to the device each mS.*/
    /* packet_size = (sample frequency (Hz) /1000) * (bit resolution/8) * number of channels */
    packet_size_tmp = ((((format_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16)|
    ((format_type_desc->tsamfreq[bsamfreqtype_index][1])<< 8) |
    ((format_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0) )
    *(format_type_desc->bbitresolution / 8)
    *(format_type_desc->bnrchannels) / 1000);
    return (packet_size_tmp);
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_request_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a mute request is sent successfully.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_request_ctrl_callback
(
/* [IN] pointer to pipe */
usb_pipe_handle  pipe_handle,

/* [IN] user-defined parameter */
void           *user_parm,

/* [IN] buffer address */
unsigned char         *buffer,

/* [IN] length of data transferred */
uint32_t           buflen,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t           status
)
{ /* Body */
    static feature_control_struct_t* control_feature_ptr;
    control_feature_ptr = (feature_control_struct_t*)user_parm;

    switch (control_feature_ptr->FU)
    {
    case USB_AUDIO_CTRL_FU_MUTE:
        {
            OS_Event_set(usb_audio_fu_request, FU_MUTE_MASK);
            break;
        }
    case USB_AUDIO_CTRL_FU_VOLUME:
        {
            OS_Event_set(usb_audio_fu_request, FU_VOLUME_MASK);
            break;
        }
    default:
        break;
    }
    //fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_tr_callback
* Returned Value : None
* Comments       :
*     Called when a ISO packet is sent/received successfully.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_tr_callback(
/* [IN] pointer to pipe */
usb_pipe_handle pipe_handle,

/* [IN] user-defined parameter */
void *user_parm,

/* [IN] buffer address */
unsigned char *buffer,

/* [IN] length of data transferred */
uint32_t buflen,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status
)
{

  	USB_Prepare_Data();
	if( status == USB_OK)
		audio_stream.dev_state =USB_DEVICE_INUSE;
}

usb_interface_descriptor_handle audio_control_get_interface()
{
    return (usb_interface_descriptor_handle)(g_interface_control_info[0]);
}

usb_interface_descriptor_handle audio_stream_get_interface()
{
    return (usb_interface_descriptor_handle)(g_interface_stream_info[1]);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_control_event
* Returned Value : None
* Comments       :
*     Called when control interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_control_event
(
/* [IN] pointer to device instance */
usb_device_instance_handle      dev_handle,

/* [IN] pointer to interface descriptor */
usb_interface_descriptor_handle intf_handle,

/* [IN] code number for event causing callback */
uint32_t                          event_code
)
{
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;
    usb_status status;

    switch (event_code) {
        case USB_CONFIG_EVENT:	 	
            if((audio_control.dev_state == USB_DEVICE_ATTACHED) )
            {
                audio_control.dev_handle  = dev_handle;
                audio_control.intf_handle = audio_control_get_interface();
                audio_control.dev_state   = USB_DEVICE_CONFIGURED;
            }
            else
            {
                printf("Audio device already attached\n");
                //fflush(stdout);
            }
            printf("----- Audio control interface: USB_CONFIG_EVENT -----\n");
            break;
        /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            /* initialize new interface members and select this interface */
            g_interface_control_info[g_interface_control_number] = pHostIntf;
            g_interface_control_number++;
            audio_control.dev_state   = USB_DEVICE_ATTACHED;	
            printf("----- Audio control interface: attach event -----\n");
            //fflush(stdout);
            printf("State = attached");
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            break;

        case USB_INTF_OPENED_EVENT:
            {
                printf("----- Audio control interface: interface event -----\n");
                usb_audio_ctrl_desc_header_t*   header_desc = NULL;
                usb_audio_ctrl_desc_it_t*        it_desc  = NULL;
                usb_audio_ctrl_desc_ot_t*       ot_desc  = NULL;

                /* finds all the descriptors in the configuration */
                if (USB_OK != usb_class_audio_control_get_descriptors(dev_handle,
                		intf_handle,
                		&header_desc,
                		&it_desc,
                		&ot_desc,
                		&fu_desc))
                {
                    ;//break;
                };

                /* set all info got from descriptors to the class interface struct */
                usb_class_audio_control_set_descriptors(audio_control.class_handle,
                	header_desc, it_desc, ot_desc, fu_desc);

                if(USB_OK != check_device_type(it_desc, ot_desc, &device_string, &device_direction))
                {
                    break;
                }
                device_direction = USB_AUDIO_DEVICE_DIRECTION_IN;
                OS_Event_set(usb_keyboard_event, USB_Keyboard_Event_CTRL);
                audio_stream.dev_state = USB_DEVICE_INUSE;
            }
            break;

        case USB_DETACH_EVENT:
            printf("----- Audio control interface: detach event -----\n");
            printf("State = detached");
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            status = usb_host_close_dev_interface(host_handle, audio_control.dev_handle, audio_control.intf_handle, audio_control.class_handle);
            if (status != USB_OK)
            {
                printf("\nError in _usb_host_close_dev_interface: %x\n", status);
            } /* Endif */
            audio_control.dev_handle = NULL;
            audio_control.intf_handle = NULL;
            audio_control.dev_state = USB_DEVICE_DETACHED;
            g_interface_control_number = 0;
            device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
            break;

        default:
            printf("Audio Device: unknown control event\n");
            //fflush(stdout);
            break;
        }
    //fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_stream_event
* Returned Value : None
* Comments       :
*     Called when stream interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_stream_event
(
/* [IN] pointer to device instance */
usb_device_instance_handle      dev_handle,

/* [IN] pointer to interface descriptor */
usb_interface_descriptor_handle intf_handle,

/* [IN] code number for event causing callback */
uint32_t                          event_code
)
{ /* Body */
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;
    usb_status status;
    switch (event_code) {
        case USB_CONFIG_EVENT:
            {
                usb_audio_stream_desc_spepific_as_if_t*     as_itf_desc = NULL;
                usb_audio_stream_desc_specific_iso_endp_t*  iso_endp_spec_desc = NULL;
                if(intf_handle != audio_stream_get_interface())
                    return;
                if((audio_stream.dev_state == USB_DEVICE_IDLE) || (audio_stream.dev_state == USB_DEVICE_DETACHED))
                {
                    audio_stream.dev_handle  = dev_handle;
                    audio_stream.intf_handle = audio_stream_get_interface();
                    audio_stream.dev_state   = USB_DEVICE_CONFIGURED;
                }
                else
                {
                    printf("Audio device already attached\n");
                }

                /* finds all the descriptors in the configuration */
                if (USB_OK != usb_class_audio_stream_get_descriptors(dev_handle,
                        intf_handle,
                        &as_itf_desc,
                        &frm_type_desc,
                        &iso_endp_spec_desc))
                {
                    printf("usb_class_audio_stream_get_descriptors fail!\n");
                    break;
                };
                /* initialize new interface members and select this interface */
                if (USB_OK != usb_host_open_dev_interface(host_handle, dev_handle,
                        intf_handle, (void *)&audio_stream.class_handle))
                {

                    printf("----- Audio stream interface: _usb_host_open_dev_interface fail\n");
                }
                audio_com.class_stream_handle = (class_handle)audio_stream.class_handle;
                /* set all info got from descriptors to the class interface struct */
                usb_class_audio_stream_set_descriptors(audio_stream.class_handle,
                as_itf_desc, frm_type_desc, iso_endp_spec_desc);
                printf("----- Audio stream interface: USB_CONFIG_EVENT -----\n");
            }
            break;
        /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            g_interface_stream_info[g_interface_stream_number] = pHostIntf;
            g_interface_stream_number++;
            printf("----- Audio stream interface: attach event -----\n");
            //fflush(stdout);
            printf("State = attached");	
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            printf("  Class = %d", intf_ptr->bAlternateSetting);
            //fflush(stdout);
            break;
     
        case USB_INTF_OPENED_EVENT:
            OS_Event_set(USB_ctr_Event, USB_EVENT_CTRL);
            break;

        case USB_DETACH_EVENT:
            if(intf_handle != audio_stream_get_interface())
                return;
            status = usb_host_close_dev_interface(host_handle, dev_handle, intf_handle, audio_stream.class_handle);
            if (status != USB_OK)
            {
                printf("error in _usb_hostdev_close_interface %x\n", status);
            }
            audio_stream.dev_handle = NULL;
            audio_stream.intf_handle = NULL;
            audio_stream.dev_state = USB_DEVICE_DETACHED;
            g_interface_stream_number = 0;
#if  (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
            audio_state = AUDIO_IDLE;
            hwtimer_stop(&audio_timer);
            printf("hwtimer_stop\n");
#else
            RemoveTimerQ(time_index);
#endif        
			OS_Event_clear(usb_timer_event, timer_out_event);
			printf("----- Audio stream interface: detach event-----\n");
            break;
        
        default:
            printf("Audio device: unknown data event\n");
            //fflush(stdout);
            break;
    } /* EndSwitch */
} /* Endbody */

/******************************************************************************
*   @name        config_channel
*
*   @brief       Config volume for all channels of speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to unmute and set volume of audio speaker
*****************************************************************************/
void config_channel(usb_audio_ctrl_desc_fu_t* fu_ptr,uint8_t num_channel)
{
    uint8_t i;
    feature_control_struct_t control_feature;
    for (i = 0; i<=num_channel; i++)
    {
        audio_com.class_control_handle = (class_handle)audio_control.class_handle;
        audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
        control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
        audio_com.callback_param = (void *)&control_feature;
        g_cur_mute[0] = FALSE;
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_mute,USB_AUDIO_SET_CUR_MUTE);
        control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
        audio_com.callback_param = (void *)&control_feature;
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_vol,USB_AUDIO_SET_CUR_VOLUME);
    }
}

/******************************************************************************
*   @name        audio_mute_command
*
*   @brief       Mute/unmute audio speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to mute/unmute audio speaker
*****************************************************************************/
void audio_mute_command(void)
{
    feature_control_struct_t control_feature;
    if (( audio_stream.dev_state > USB_DEVICE_DETACHED)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        printf("  err: Audio Speaker is not connected\n");
     //   return;
    }
    g_cur_mute[0] = !g_cur_mute[0];
    if (g_cur_mute[0])
    {
        printf("Mute ...\n");
    }
    else
    {
        printf("UnMute ...\n");
    }
    /* Send set mute request */
    control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
    audio_com.class_control_handle = (class_handle)audio_control.class_handle;
    audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
    audio_com.callback_param = &control_feature;
    usb_class_audio_feature_command(&audio_com,0,(void *)g_cur_mute,USB_AUDIO_SET_CUR_MUTE);
}

/******************************************************************************
*   @name        audio_increase_volume_command
*
*   @brief       Increase audio speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to increase volume of audio speaker
*****************************************************************************/
void audio_increase_volume_command(void)
{
    feature_control_struct_t control_feature;
    static uint32_t i;
    uint8_t max_audio_channel;
    if (( audio_stream.dev_state > USB_DEVICE_DETACHED)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        printf("  err: Audio Speaker is not connected\n");
   //     return;
    }
    max_audio_channel = frm_type_desc->bnrchannels;
    /* Send set mute request */
    cur_volume = (g_cur_vol[1]<<8)|(g_cur_vol[0]);
    if((host_cur_volume + HOST_VOLUME_STEP) > HOST_MAX_VOLUME)
    {
        host_cur_volume = HOST_MAX_VOLUME;
        i = 0;
    }
    else
    {
        host_cur_volume += HOST_VOLUME_STEP;
        i = 1;
    }
    cur_volume += (uint16_t)(i*HOST_VOLUME_STEP*device_volume_step);
    g_cur_vol[0] = (uint8_t)((uint16_t)(cur_volume)&0x00FF);
    g_cur_vol[1] = (uint8_t)((uint16_t)(cur_volume)>>8);
    audio_com.class_control_handle = (class_handle)audio_control.class_handle;
    audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
    control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
    audio_com.callback_param = &control_feature;
    for (i = 0; i <= max_audio_channel; i++)
    {
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_vol,USB_AUDIO_SET_CUR_VOLUME);
    }
    physic_volume = ((uint16_t)(cur_volume)*39)/10000;
    printf("\n Current physical volume: %d dB",physic_volume);
}

/******************************************************************************
*   @name        audio_decrease_volume_command
*
*   @brief       Decrease audio speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to decrease volume of audio speaker
*****************************************************************************/
void audio_decrease_volume_command(void)
{
    feature_control_struct_t control_feature;
    static uint32_t i;
    uint8_t max_audio_channel;
    if (( audio_stream.dev_state > USB_DEVICE_DETACHED)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        printf(" err: Audio Speaker is not connected\n");
    }
    max_audio_channel = frm_type_desc->bnrchannels;
    /* Send set mute request */
    audio_com.class_control_handle = (class_handle)audio_control.class_handle;
    cur_volume = (g_cur_vol[1]<<8)|(g_cur_vol[0]);
    if(host_cur_volume < (HOST_VOLUME_STEP + HOST_MIN_VOLUME))
    {
        host_cur_volume = HOST_MIN_VOLUME;
        i = 0;
    }
    else
    {
        host_cur_volume -= HOST_VOLUME_STEP;
        i = 1;
    }
    cur_volume -= (uint16_t)(i*HOST_VOLUME_STEP*device_volume_step);
    g_cur_vol[0] = (uint8_t)((uint16_t)(cur_volume)&0x00FF);
    g_cur_vol[1] = (uint8_t)((uint16_t)(cur_volume)>>8);
    audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
    control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
    audio_com.callback_param = &control_feature;
    for (i=0 ; i<= max_audio_channel ; i++)
    {
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_vol,USB_AUDIO_SET_CUR_VOLUME);
    }
    physic_volume = ((uint16_t)(cur_volume)*39)/10000;
    printf("\n Current physical volume: %d dB",physic_volume);
}

/* EOF */
