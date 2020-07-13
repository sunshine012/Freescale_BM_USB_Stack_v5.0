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
* $FileName: sh_audio.c$
* $Version :
* $Date    :
*
* Comments:
*
*   This file contains the MQX shell commands for audio devices.
*
*END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include <lwevent.h>
#include <bsp.h>
#else
#include "derivative.h"
#include "hidef.h"
#include "mem_util.h"
#endif

#include "usb_host_hub_sm.h"
#include "usb_host_audio.h"
#include "sh_audio.h"
#include "wav.h"
#include "shell.h"
#include <string.h>
#include <hwtimer.h>

#include "audio_microphone.h"

/***************************************
**
** Global variables
****************************************/
extern uint8_t                                  wav_sdcard_buff0[REC_BLOCK_SIZE];
extern uint8_t                                  wav_sdcard_buff1[REC_BLOCK_SIZE];
/* audio speaker or audio microphone */
extern char  device_direction;
extern endpoint_descriptor_t*                     endp;
extern volatile audio_control_device_struct_t     audio_control;
extern volatile audio_control_device_struct_t     audio_stream;
extern volatile uint8_t *g_cur_mute;
extern volatile uint8_t *g_cur_vol;
extern volatile uint8_t *g_cur_samp;
extern HWTIMER audio_timer;
uint32_t                                        remainder = 0;
bool                                        final_packet = FALSE;
os_event_handle                             block_rec;

/***************************************
**
** Local variables
****************************************/
extern audio_command_t audio_com;
/*FUNCTION****************************************************************
*
* Function Name    : Shell_play
* Returned Value   : SHELL_EXIT_SUCCESS if everything is ok,
*                    SHELL_EXIT_ERROR otherwise
* Comments         :
*    Shell function for recording wave files
*
*END*********************************************************************/
int32_t Shell_record(int32_t argc, char *argv[])
{
   bool print_usage, shorthelp = FALSE;
   int32_t return_code = SHELL_EXIT_SUCCESS;
   uint32_t requests = 0;
   print_usage = Shell_check_help_request (argc, argv, &shorthelp);
   if (!print_usage)
   {
      if (argc > 2)
      {
         wave_file_header_t header;
         FILE_PTR file_ptr;
         _mqx_int errcode = 0, result = 0;
         uint32_t i = 0;

         if ((USB_DEVICE_INUSE != audio_stream.dev_state)||\
             (device_direction != USB_AUDIO_DEVICE_DIRECTION_OUT))
         {
             printf("  Error: Audio Microphone is not connected\n");
             return (SHELL_EXIT_ERROR);
         }
         if (((endp->bmAttributes>>2)&EP_TYPE_MASK)!=ISOCH_NOSYNC)
         {
             printf("  Error: This sync type of the device is not supported\n");
             return (SHELL_EXIT_ERROR);
         }
         /* Set sampling frequency for microphone device */
      //   if (USB_OK != usb_class_audio_endpoint_command(&audio_com,(void *)g_cur_samp,USB_AUDIO_SET_CUR_SAMPLING_FREQ))
      //          printf("\n Set sampling frequency error");

         /* Fill header struct according to input parameters */
         /* write header to file */
         errcode = SetWaveHeader(&header, argv[1],(uint8_t *)argv[2]);
         switch (errcode)
         {
            case -1:
               printf("  Error: Cannot create file %s.\n", argv[1]);
               return (SHELL_EXIT_ERROR);
               break;

            case -2:
               printf("  Error: Wrong format specified.\n");
               return (SHELL_EXIT_ERROR);
               break;

            default:
               break;
         }
         printf("Audio file properties:\n");
         printf("   - Sample rate      : %d Hz\n", \
                  header.fmt_subchunk.samplerate);
         printf("   - Sample size      : %d bits\n", \
                  header.fmt_subchunk.bitspersample);
         printf("   - Number of channels : %d channels\n", \
                  header.fmt_subchunk.numchannels);
         /* Open file with header and make it ready for data writing */
         file_ptr = fopen(argv[1], "ab");
         if (file_ptr == NULL)
         {
            printf("  Unable to open file: %s\n", argv[1]);
            fclose(file_ptr);
            return (SHELL_EXIT_ERROR);
         }
         fseek(file_ptr, WAVE_HEADER_SIZE, IO_SEEK_SET);
           /* Compute requested number of data blocks */
	     printf("  Subchunk2Size: %d \n", header.data_subchunk.subchunk2size);
         requests = header.data_subchunk.subchunk2size / REC_BLOCK_SIZE;
         remainder = header.data_subchunk.subchunk2size % REC_BLOCK_SIZE;

         block_rec = _mem_alloc_zero(sizeof(LWEVENT_STRUCT));
         if(block_rec == NULL) {
            printf("  Out of memory error.");
            fclose(file_ptr);
            return (SHELL_EXIT_ERROR);
         }
	    block_rec = OS_Event_create(0);
         printf("  Recording...");
         hwtimer_start(&audio_timer);
         _time_delay (50);
         for(i = 0; i < requests; i++)
         {
            result = OS_Event_wait(block_rec, EV_BLOCK0_W_READY | EV_BLOCK1_W_READY, FALSE, 0);
            if (result != MQX_OK)
            {
                printf("  Error: Wait for event block record ready failed.\n");
                fclose(file_ptr);
                _task_set_error(result);
                return (SHELL_EXIT_ERROR);
            }

            if (OS_Event_check_bit(block_rec, EV_BLOCK0_W_READY)) {
			 OS_Event_clear(block_rec, EV_BLOCK0_W_READY);		
               result = fwrite(wav_sdcard_buff0, 1, sizeof(wav_sdcard_buff0), file_ptr);
            }
            else if (OS_Event_check_bit(block_rec, EV_BLOCK1_W_READY)) {
			 OS_Event_clear(block_rec, EV_BLOCK1_W_READY);	
               result = fwrite(wav_sdcard_buff1, 1, sizeof(wav_sdcard_buff1), file_ptr);
            }
            if (result != REC_BLOCK_SIZE)
            {
                    printf("  Error: Write failed. Wrote %d Bytes instead of %d.\n",\
                                    result, REC_BLOCK_SIZE);
            }
         }

         final_packet = TRUE;
         /* Alloc rest of data */
         if(remainder != 0)
         {
             result = OS_Event_wait(block_rec, EV_BLOCK0_W_READY | EV_BLOCK1_W_READY, FALSE, 0);
            if (result != MQX_OK)
            {
                printf("  Error: Wait for event block record ready failed.\n");
                fclose(file_ptr);
                _task_set_error(result);
                return (SHELL_EXIT_ERROR);
            }
             if (OS_Event_check_bit(block_rec, EV_BLOCK0_W_READY)) {
               result = fwrite(wav_sdcard_buff0, 1, remainder, file_ptr);
            }
           else if (OS_Event_check_bit(block_rec, EV_BLOCK1_W_READY)) {
               result = fwrite(wav_sdcard_buff1, 1, remainder, file_ptr);
            }
            if (result != remainder)
            {
                    printf("  Error: Write failed. Wrote %d Bytes instead of %d.\n",\
                                    result, remainder);
            }
         }
         fclose(file_ptr);
         hwtimer_stop(&audio_timer);
         result = _lwevent_destroy(block_rec);
         if (MQX_OK != result)
         {
            printf("Error: Unable to free sdwrite_event.\n");
         }
         if(NULL != block_rec)
             _mem_free(block_rec);
         printf("done\n");
         final_packet = FALSE;

      }
      else
      {
         printf("  Not enough parameters.\n");
      }
   }

   if (print_usage)
   {
      if (shorthelp)
      {
         printf("%s <filename> <length>\n", argv[0]);
      }
      else
      {
         printf("Usage: %s <filename> <length>\n", argv[0]);
         printf("   filename      = filename for recording\n");
         printf("   length        = length in seconds\n");
      }
   }
   return(return_code);
}
/******************************************************************************
*   @name        Shell_mute
*
*   @brief       Servers the mute command
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to mute the audio device
*****************************************************************************/
int32_t Shell_mute(int32_t argc, char *argv[])
{
    bool         print_usage, shorthelp = FALSE;
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

    if (!print_usage)
    {
        if (argc > 1)
        {
            printf("  Error: This command doesn't need parameters\n");
            return (SHELL_EXIT_ERROR);
        }
        else
        {
            if ((USB_DEVICE_INUSE != audio_stream.dev_state)||\
                (device_direction != USB_AUDIO_DEVICE_DIRECTION_OUT))
            {
                printf("  Error: Audio Microphone is not connected\n");
                return (SHELL_EXIT_ERROR);
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
            
            audio_com.class_control_handle = (class_handle)audio_control.class_handle;
            audio_com.callback_fn = usb_host_audio_mute_ctrl_callback;
            audio_com.callback_param = 0;
            usb_class_audio_feature_command(&audio_com,0,(void *)g_cur_mute,USB_AUDIO_SET_CUR_MUTE);
        }
    }

    if (print_usage)
    {
        if (shorthelp)
        {
            printf("%s\n", argv[0]);
        }
        else
        {
            printf("Usage: %s", argv[0]);
        }
    }
    return(SHELL_EXIT_SUCCESS);
}
/* EOF */
