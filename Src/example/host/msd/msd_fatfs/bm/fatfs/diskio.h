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
 * $FileName: diskio.h$
 * $Version : 
 * $Date    : 
 *
 * Comments: Low level disk interface module include file
 *
 *   
 *
 *END************************************************************************/

/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2013
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "derivative.h"
#include "hidef.h"
#include "mem_util.h"

#include "usb_host_hub_sm.h"
#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"

/* Status of Disk Functions */
typedef uint8_t	DSTATUS;

/* Results of Disk Functions */
typedef enum 
{
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR,		/* 4: Invalid Parameter */
    RES_NOTMEM      /* 5: Not enough heap memory */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (uint8_t pdrv);
DSTATUS disk_status (uint8_t pdrv);
DRESULT disk_read (uint8_t pdrv, uint8_t*buff, uint32_t sector, uint8_t count);
DRESULT disk_write (uint8_t pdrv, uint8_t* buff, uint32_t sector, uint8_t count);
DRESULT disk_ioctl (uint8_t pdrv, uint8_t cmd, void* buff);
uint32_t get_fattime (void);

/* Disk Status Bits (DSTATUS) */
#define STA_OK			0x00	/* Drive is ready */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

/* Command code for disk_ioctrl fucntion */
/* Generic command (mandatory for FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define ERASE_BLOCK_SIZE	1 	/* Unknown erasable block size*/

/* Generic command (not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

/* USB MSD ioctl command */
#define UFI_REQUEST_SENSE_CMD		    0x81
#define UFI_INQUIRY_CMD					0x82
#define UFI_READ_FORMAT_CAPACITY_CMD	0x83
#define UFI_READ_CAPACITY_CMD			0x84
#define UFI_TEST_UNIT_READY_CMD			0x85
#define UFI_MODE_SENSE_CMD              0x86
#define UFI_READ10_CMD                  0x87

#define GET_VENDOR_INFO                 0x91
#define GET_PRODUCT_ID                  0x92
#define GET_PRODUCT_REV                 0x93

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */

/* Storage information*/
#define MSD_DEFAULT_SECTOR_SIZE  (512)

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS   16
#define  MAX_FRAME_SIZE             1024
#define  HOST_CONTROLLER_NUMBER     0
#define  BUFF_IN_SIZE 				0x40c
#define  USBCFG_MAX_INSTANCE        (4)

#define  MAX_RETRY_TIMES            (3)
typedef struct device_struct
{
   uint32_t                        dev_state;  /* Attach/detach state */
   usb_device_instance_handle      dev_handle;
   usb_interface_descriptor_handle intf_handle;
   class_handle                     CLASS_HANDLE; /* Class-specific info */
} device_struct_t;


#ifdef __cplusplus
}
#endif

#endif
