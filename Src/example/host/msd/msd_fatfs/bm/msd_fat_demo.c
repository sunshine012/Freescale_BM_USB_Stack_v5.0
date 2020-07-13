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
* $FileName: msd_fat_demo.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file implements MAS FAT demo application.
*
*END************************************************************************/
#include <stdio.h>
#include "ff.h"
#include "diskio.h"
#include "usb_host_msd_ufi.h"
#include "rtc.h"

/* User-defined macros */
#define DIR_OPERATION 1
#define FILE_OPERATION 1

/* Funtion prototypes */
static FRESULT put_rc (FRESULT rc);
uint32_t out_stream ( const uint8_t *p,uint32_t btf);
static void Display_File_Info(FILINFO*Finfo);
static FRESULT List_Directory(const TCHAR * path);
int fat_demo(void);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : put_rc
* Returned Value : None
* Comments       : This function is to display returnCode of FAT's APIs 
*
*END*--------------------------------------------------------------------*/
static FRESULT put_rc 
  (
    /* [IN] return code value*/
    FRESULT rc
  )
{
    const TCHAR *str =
        (TCHAR *)"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
    FRESULT i;

    for (i = FR_OK; i != rc && *str; i++) 
    {
        while (*str++) ;
    }
    printf("  returnCode=%u (FR_%s)\n\r", (uint32_t)rc, str);
    
    if (FR_OK != rc)
    {
        printf("\n\r*------------------------------     DEMO FAILED       ------------------------ *"); 
    }
    
    return rc;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : out_stream
* Returned Value : Returns number of bytes sent or stream status
* Comments       : Sample code of data transfer function to be called back from f_forward
*
*END*--------------------------------------------------------------------*/
 uint32_t out_stream 
   (
      /* [IN] Pointer to the data block to be sent */
      const uint8_t *p, 
      /* [IN] >0: Transfer call (Number of bytes to be sent). 0: Sense call */ 
      uint32_t btf        
   )
{                            
    
    uint32_t cnt = 0;
    if (btf == 0) 
    {  /*Sense call */   
       cnt = 1;
    }
    else 
    {              /* Transfer call */
       do 
       {    /* Repeat while there is any data to be sent */
           printf("%c",*p++);
           cnt++;
       } while (cnt < btf);
    }

    return cnt;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Display_File_Info
* Returned Value : None
* Comments       : This function is to display file information
*
*END*--------------------------------------------------------------------*/
static void Display_File_Info(FILINFO*Finfo) 
{
    printf("    %c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s\n\r", 
            (Finfo->fattrib & AM_DIR) ? 'D' : '-',
            (Finfo->fattrib & AM_RDO) ? 'R' : '-',
            (Finfo->fattrib & AM_HID) ? 'H' : '-',
            (Finfo->fattrib & AM_SYS) ? 'S' : '-',
            (Finfo->fattrib & AM_ARC) ? 'A' : '-',
            (Finfo->fdate.Bits.year) + YEAR_ORIGIN, (Finfo->fdate.Bits.month), Finfo->fdate.Bits.day,
            (Finfo->ftime.Bits.hour), (Finfo->ftime.Bits.minute),
            Finfo->fsize,
#if _USE_LFN
            (Finfo->lfname[0])?&Finfo->lfname[0]:&Finfo->fname[0]
#else
            &Finfo->fname[0]
#endif
            );
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : List_Directory
* Returned Value : FR_OK: sucessful, != FR_OK: failed
* Comments       : List and display all the file and directory in the specified directory
*
*END*--------------------------------------------------------------------*/
static FRESULT List_Directory(const TCHAR *path)
{
  
    FRESULT returnCode = FR_OK; /* return code */
    FILINFO Finfo;      /* File object */
    DIR dir;                  /* Directory object */
    uint16_t dir_no = 0, file_no = 0;
    uint32_t size = 0;

#if _USE_LFN
    /* Allocate memory for Finfo->lfname */
    Finfo.lfname = ff_memalloc(_MAX_LFN);
    Finfo.lfsize = _MAX_LFN;
#endif
    
    printf(" Directory listing...\n\r");
    /* Open root directory */
    returnCode = f_opendir(&dir,path);
    if (returnCode)
    {
#if _USE_LFN
        if(NULL != Finfo.lfname)
        {
            /* Free memory for Finfo->lfname */
            ff_memfree(Finfo.lfname);
        }
#endif
        return(returnCode);
    }
  
    for(;;) 
    {
        returnCode = f_readdir(&dir, &Finfo);
        if ((returnCode != FR_OK) || !Finfo.fname[0])
        {
            break;
        }
        if (Finfo.fattrib & AM_DIR) 
        {
            dir_no++;
        } 
        else 
        {
            file_no++; size += Finfo.fsize;
        }

        Display_File_Info(&Finfo);   
    }
    
    printf("\n\n    %-4u File(s),%10lu bytes total\n    %-4u Dir(s)\n\r", file_no, size, dir_no);
#if _USE_LFN
    if(NULL != Finfo.lfname)
    {
        /* Free memory for Finfo->lfname */
        ff_memfree(Finfo.lfname);
    }
#endif
    return returnCode;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : fat_demo
* Returned Value : 
* Comments       : Test Fat file system
*
*END*--------------------------------------------------------------------*/
int fat_demo(void)
{
    FRESULT returnCode;        /* Result code */
    FATFS fatfs;            /* File system object */
    FATFS *fs;
    FIL fil;                /* File object */
    FILINFO Finfo;            /* File information object */
    char buffer[257];
    char *p_str;
    char *dev_info;
    uint32_t fre_clust, size;
    static const char *str ="Line 4: Write data to file uses f_putc function\0";
    char *str_temp = (char *)str;
    const uint8_t ft[] = {0,12,16,32};


    time_delay(1000);

#if _USE_LFN
    /* Allocate memory for Finfo->lfname */
    Finfo.lfname = ff_memalloc(_MAX_LFN);
    Finfo.lfsize = _MAX_LFN;
#endif

    printf("\n\r******************************************************************************");
    printf("\n\r*                              FatFS DEMO                                    *");
    if(_USE_LFN)
    {
        printf("\n\r*          Configuration:  LNF Enabled, Code page  =%u                       *", _CODE_PAGE);
    }
    else 
    {
        printf("\n\r*          Configuration:  LNF Disabled, Code page =%u                       *", _CODE_PAGE);
    }
    printf("\n\r******************************************************************************");
    
    printf("\n\r******************************************************************************");
    printf("\n\r*                             DRIVER OPERATION                               *");
    printf("\n\r******************************************************************************");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
    /* DM1: Initialize logical driver <f_mount>                                             */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r1. Demo function: f_mount\n\n\r");
    printf("  Initializing logical drive 0...\n\r");
    returnCode = f_mount(0, &fatfs);
    if(returnCode)
    {
        goto Error1;
    }
    printf("  Initialization complete \n\r");
    printf("-----------------------------------------------------------------------------\n\r");
    
    /* Send some SCSI requests first */
    disk_initialize(0);
#if !HIGH_SPEED_DEVICE
    time_delay(1000);
#endif

    /* Get the vendor information and display it */
    printf("\n************************************************************************\n");
    disk_ioctl(0, GET_VENDOR_INFO, &dev_info);
    printf("Vendor Information:     %-1.8s Mass Storage Device\n",dev_info);
    disk_ioctl(0, GET_PRODUCT_ID,  &dev_info);
    printf("Product Identification: %-1.16s\n",dev_info);
    disk_ioctl(0, GET_PRODUCT_REV, &dev_info);
    printf("Product Revision Level: %-1.4s\n",dev_info);
    printf("************************************************************************\n");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM2.  Show logical drive status <f_getfree, f_opendir, f_readdir>                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r2. Demo functions:f_getfree, f_opendir, f_readdir\n\n\r");
    printf("getting drive 0 attributes............... \n\r");
    printf("Logical drive 0 attributes:\n\r");
    /* Get free clusters */
    returnCode = f_getfree(_T("0:"),&fre_clust,&fs);
    if (returnCode)
    {
        goto Error1;
    }
    /* Display drive status */
    printf(" FAT type = FAT%u\n\r Bytes/Cluster = %lu\n Number of FATs = %u\n\r" \
          " Root DIR entries = %u\n\r Sectors/FAT = %lu\n\r Number of clusters = %lu\n\r" \
          " FAT start (lba) = %lu\n DIR start (lba,clustor) = %lu\n Data start (lba) = %lu\n\n\r...\n",\
          ft[fs->fs_type & 3], fs->csize * 512UL, fs->n_fats,\
          fs->n_rootdir, fs->fsize, fs->n_fatent - 2, \
          fs->fatbase, fs->dirbase, fs->database);
    
    printf("%lu KB total disk space.\n\r%lu KB available.\n\r",(((fs->n_fatent - 2)*(fs->csize ))/2), ((fre_clust * (fs->csize ))/2));
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM3.  Make file system <f_mkfs>                                                     */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if _USE_MKFS
    /* Format drive */
    printf("\n\r3. Demo functions:f_mkfs\n\n\r");
    printf("Formatting drive ...............\n\r");
    returnCode = f_mkfs(0,0,4096);
    if (returnCode) 
    {
        goto Error1;
    }
    else
    {
        printf("Format complete\n\r");
    }
    printf("getting drive 0 attributes............... \n\r");
    printf("Logical drive 0 attributes:\n\r");
    /* Display drive status */
    returnCode = f_getfree("0:",&fre_clust,&fs);
    if (returnCode)
    {
        goto Error1;
    }
    printf(" FAT type = FAT%u\n\r Bytes/Cluster = %lu\n\r Number of FATs = %u\n\r" \
          " Root DIR entries = %u\n\r Sectors/FAT = %lu\n\r Number of clusters = %lu\n\r" \
          " FAT start (lba) = %lu\n\r DIR start (lba,clustor) = %lu\n\r Data start (lba) = %lu\n\n...\n\r",\
          ft[fs->fs_type & 3], fs->csize * 512UL, fs->n_fats,\
          fs->n_rootdir, fs->fsize, fs->n_fatent - 2, \
          fs->fatbase, fs->dirbase, fs->database);
    
    printf("%lu KB total disk space.\n\r%lu KB available.\n\r",(((fs->n_fatent - 2)*(fs->csize ))/2), ((fre_clust * (fs->csize ))/2));
#endif
    
    
#if (DIR_OPERATION)  
    printf("\n\r******************************************************************************");
    printf("\n\r*                             DRECTORY OPERATION                             *");
    printf("\n\r******************************************************************************");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM1. Directory listing <f_opendir, f_readdir>                                        */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    
    printf("\n\r1. Demo functions:f_opendir, f_readdir\n\n");
    returnCode = List_Directory(_T(""));
    if (returnCode)
    {
        goto Error1;
    }
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM2. Create directory <f_mkdir>                                                      */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Create Dir_1 as a sub directory of root */
    printf("\n\r2. Demo functions:f_mkdir\n\n\r");
    printf("2.0. Create <Dir_1> \n\r");
    returnCode = f_mkdir(_T("Dir_1"));
    if((returnCode != FR_OK)&&(returnCode != FR_EXIST))
    {
        goto Error1;
    }
    
    /* Create Dir_2 as a sub directory of root */ 
    printf("2.1. Create <Dir_2> \n\r");
    returnCode = f_mkdir(_T("Dir_2"));
    if((returnCode != FR_OK)&&(returnCode != FR_EXIST))
    {
        goto Error1;
    }
    
    /* Create Sub1 as a sub directory of Dir_1 */ 
    printf("2.2. Create <Sub1> as a sub directory of <Dir_1> \n\r");
    returnCode = f_mkdir(_T("Dir_1/sub1"));
    if((returnCode != FR_OK)&&(returnCode != FR_EXIST))
    {
        goto Error1;
    }
    /* List dir */
    printf("2.3. Directory list\n\r");
    returnCode = List_Directory(_T(""));
    if (returnCode)
    {
        goto Error1;
    }
    
    printf("2.4. \"Dir_1\" Directory list\n\r");
    returnCode = List_Directory(_T("Dir_1"));
    if (returnCode)
    {
        goto Error1;
    }
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM3. Get and Change current directory <f_getcwd, f_chdir>                            */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r3. Demo functions:f_getcwd, f_chdir\n\n\r");
    /* Get the current directory */
    printf("3.0. Get the current directory \n\r");
    returnCode = f_getcwd((TCHAR *)buffer,256);
    if (returnCode)
    {
        goto Error1;
    }
    else 
    {
        printf("    CWD: %s\n\r",buffer);
    }
    
    /* Change directory to Dir_1 */
    printf("3.1. Change current directory to <Dir_1>\n\r");
    returnCode = f_chdir(_T("/Dir_1"));
    if (returnCode)
    {
        goto Error1;
    }
    
    /* List dir */
    printf("3.2. Directory listing \n\r");
    returnCode = List_Directory(_T("."));
    if (returnCode)
    {
        goto Error1;
    }
    
    /* Get the current directory */
    printf("3.3. Get the current directory \n\r");
    returnCode = f_getcwd((TCHAR *)buffer,256);
    if (returnCode)
    {
        goto Error1;
    }
    else 
    {
        printf("    CWD: %s\n\r",buffer);
    }
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM4. Get directory status, Change attribute and time <f_stat, f_chmod, f_utime>      */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r4. Demo functions:f_stat(File status), f_chmod, f_utime\n\n\r");
    /* Get directory status */ 
    printf("4.1. Get directory information of <Dir_1>\n\r");
    returnCode = f_stat(_T("../Dir_1"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    /* Change timestamp */
    printf("4.2  Change the timestamp of Dir_1 to 12.25.2010: 23h 30' 20\n\r");
    Finfo.fdate.Bits.year = 2010 - YEAR_ORIGIN;
    Finfo.fdate.Bits.month = 12;
    Finfo.fdate.Bits.day = 25;
    
    
    Finfo.ftime.Bits.hour = 23;
    Finfo.ftime.Bits.minute = 30;
    Finfo.ftime.Bits.second = 20; 
    returnCode = f_utime(_T("../Dir_1"),&Finfo);
    if (returnCode) 
    {
        goto Error1;
    }
    
    /* Chang directory attribute */
    printf("4.3. Set Read Only Attribute to Dir_1\n\r");
    returnCode = f_chmod(_T("../Dir_1"),AM_RDO,AM_RDO);
    if(returnCode)
    {
        goto Error1;
    } 
    
    /* Get directory status */
    printf("4.4. Get directory information (Dir_1)\n\r");
    returnCode = f_stat(_T("../Dir_1"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    printf("-----------------------------------------------------------------------------\n");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM5. Rename, Move directory < f_rename>                                              */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r5. Demo functions:f_rename\n\n\r");
    printf("Rename <sub1> to <sub1_rm> and move it to <Dir_2> \n\r");
    returnCode = f_rename(_T("./sub1"),_T("../Dir_2/sub1_rm"));
    if(returnCode && returnCode != FR_EXIST)
    {
        goto Error1;
    }
    else 
    {
    
        List_Directory(_T("../Dir_2"));
    }
    
    /* Back to home directory */
    /*
    printf("\n\r6. Back to home directory\n\r");
    returnCode = f_chdir(_T(".."));
    if (returnCode)
    {
        goto Error1;
    }
    
    printf("\n\r7. List home files\n\n");
    returnCode = List_Directory(_T(""));
    if (returnCode)
    {
        goto Error1;
    }
    */

    printf("-----------------------------------------------------------------------------\n");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM6. Delele directory < f_unlink>                                                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r6. Demo functions:f_unlink\n\n\r");
    printf(" Delete Dir_1/sub1_rm\n\r");
    returnCode = f_unlink(_T("../Dir_2/sub1_rm"));
    if(returnCode) 
    {
        goto Error1;
    }
    else 
    {
        List_Directory(_T("../Dir_2"));
    }
#endif /* End of #if(DIR_OPERATION) */ 
    
#if (FILE_OPERATION) 
    printf("\n\r******************************************************************************");
    printf("\n\r*                             FILE OPERATION                                 *");
    printf("\n\r******************************************************************************");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM1. Write to  file < f_open,f_write, f_printf, f_putc, f_puts, fclose>              */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r1. Demo functions:f_open,f_write, f_printf, f_putc, f_puts, fclose\n\n\r");
    
    /* Create a new file to write */
    printf("1.0. Create new file <New_F_1> (f_open)\n\r");
    returnCode = f_stat(_T("New_F_1.dat"), &Finfo);
    if (returnCode == FR_OK)
    {
        printf("    File exist\n");
        if (Finfo.fattrib & AM_RDO)
        {
            printf("    Clear readonly attribute\n");
            returnCode = f_chmod(_T("New_F_1.dat"), 0, AM_RDO);
            if (returnCode)
                goto Error1;
        }
    }
    returnCode = f_open(&fil,_T("New_F_1.dat"),FA_WRITE|FA_CREATE_ALWAYS);
    if (returnCode) 
    {
        goto Error1;
    }
    printf("    File size = %4d\n\r",fil.fsize);
    
    /* Write data to file uses f_write function */
    printf("1.1. Write data to <New_F_1>(f_write)\n\r");
    returnCode = f_write(&fil,"Line 1: Write data to  file uses f_write function \n\r",52,&size);
    if (returnCode) 
    {
        goto Error1;
    }
    /* Flush cached data */
    printf("1.2. Flush cached data\n\r");
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    printf("    File size = %4d\n",fil.fsize);
    
    /* Write data to file uses f_printf function */
    printf("1.3. Write data to <New_F_1> (f_printf)\n\r");
    
    if (EOF == f_printf(&fil,_T("Line %d: %s"),2,_T("Write data to file uses f_printf function\n\r"))) 
    {
        goto Error1;
    }
    
    /* Flush cached data */
    printf("1.4. Flush cached data\n\r");
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    printf("    File size = %4d\n\r",fil.fsize);
    
    /* Write data to file uses f_puts function */
    printf("1.5. Write data to <New_F_1> (f_puts)\n\r");
    if (EOF == f_puts(_T("Line 3: Write data to file uses f_puts function\n\r"), &fil)) 
    {
        goto Error1;
    }
    /* Flush cached data */
    printf("1.6. Flush cached data\n\r");
    
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    printf("    File size = %4d\n\r",fil.fsize);
    
    /* Write data to file uses f_putc function */
    printf("1.7. Write data to <New_F_1> uses f_putc function\n\r");
    while(*str_temp)
    {
        if (EOF == f_putc(*str_temp++,&fil))
        {
            goto Error1;
        }
    }
    /* Flush cached data */
    printf("1.8. Flush cached data\n\r");
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    printf("    File size = %4d\n\r",fil.fsize);
    
    /* Close file */
    printf("1.9. Close file <New_F_1>\n\r");
    returnCode = f_close(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM2. Read from a file < f_open,f_read, f_gets, fclose>                       */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r2. Demo functions:f_open,f_read, f_seek, f_gets, f_close\n\n\r");
    
    /* Open New_F_1 to read */
    printf("2.0. Open <New_F_1> to read (f_open)\n\r");
    returnCode = f_open(&fil,_T("New_F_1.dat"),FA_READ);
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Use f_gets to get a string from file */
    printf("2.1. Get a string from file (f_gets)\n\r");
    p_str = buffer;
    p_str = (char *)f_gets((TCHAR*)p_str,256,&fil);
    printf("    %s",buffer);
    
    /* Use f_read to get the rest of file content */
    printf("\n\r2.2. Get the rest of file content (f_read)\n\r");
    fil.fptr++;
    while(fil.fptr < fil.fsize) 
    {
        returnCode = f_read(&fil, buffer, sizeof(buffer)-1,&size);
        if(returnCode)
        {
            goto Error1;
        }
        else
        {
            buffer[size] = '\0';
            printf("    %s",buffer);
        }
    }
    
    /* Close file */
    printf("\n\r2.3. Close file (f_close)\n\r");
    returnCode = f_close(&fil);
    if(returnCode)
    {
        goto Error1;
    }
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM3.  Get file infor, Change attribute and timestamp of file <f_stat, f_utime, f_chmod>*/
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r3. Demo functions:f_stat, f_utime, f_chmod\n\n\r");
    
    printf("3.1. Get  information of <New_F_1> file (f_stat)\n\r");
    returnCode = f_stat(_T("New_F_1.dat"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    /* Change timestamp */
    printf("3.2  Change the timestamp of Dir_1 to 12.25.2010: 23h 30' 20 (f_utime)\n\r");
    Finfo.fdate.Bits.year = 2010 - YEAR_ORIGIN;
    Finfo.fdate.Bits.month = 12;
    Finfo.fdate.Bits.day = 25;
    
    
    Finfo.ftime.Bits.hour = 23;
    Finfo.ftime.Bits.minute = 30;
    Finfo.ftime.Bits.second = 10; 
    
    returnCode = f_utime(_T("New_F_1.dat"),&Finfo);
    if (returnCode) 
    {
        goto Error1;
    }
    
    /* Chang directory attribute */
    printf("3.3. Set Read Only Attribute to <New_F_1> (f_chmod) \n\r");
    returnCode = f_chmod(_T("New_F_1.dat"),AM_RDO,AM_RDO);
    if(returnCode)
    {
        goto Error1;
    } 
    
    /* Get directory status */
    printf("3.4. Get directory information of <New_F_1> (f_stat)\n\r");
    returnCode = f_stat(_T("New_F_1.dat"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    /* Chang directory attribute */
    printf("3.5. Clear Read Only Attribute of <New_F_1> (f_chmod)\n\r");
    returnCode = f_chmod(_T("New_F_1.dat"),0,AM_RDO);
    if(returnCode)
    {
        goto Error1;
    } 
    
    /* Get directory status */
    printf("3.6. Get directory information of <New_F_1>\n\r");
    returnCode = f_stat(_T("New_F_1.dat"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM4.  Change file name <f_rename>                                                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r4. Demo functions:f_ulink\n\n\r");
    printf(" Rename <New_F_1.dat> to  <File_Rm.txt> \n\r");
    returnCode = f_rename(_T("New_F_1.dat"),_T("File_Rm.txt"));
    if(returnCode && returnCode != FR_EXIST)
    {
        goto Error1;
    }
    else 
    {
    
        List_Directory(_T("."));
    }
    
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* TC5.  Truncate file <f_truncate,f_lseek>                                             */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r5. Demo functions:f_truncate\n\n\r");
    printf(" Truncate file <File_Rm.txt>\n\r");
    /* Open File_Rm.txt to read */
    printf("5.0. Open <File_Rm.txt> to write\n\r");
    returnCode = f_open(&fil,_T("File_Rm.txt"),FA_WRITE);
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Seek file pointer */
    printf("5.1. Seek file pointer\n\r    Current file pointer: %4d\n\r",fil.fptr);
    returnCode = f_lseek(&fil, 102);
    printf("    File pointer after seeking: %4d\n\r",fil.fptr);
    
    /* Truncate file */
    printf("5.2. Truncate file\n\r");
    returnCode = f_truncate(&fil); 
    if(returnCode)
    {
        goto Error1;
    }
    printf("    File size = %4d\n\r",fil.fsize);
    
    /* Close file */
    printf("5.3. Close file\n\r");
    returnCode = f_close(&fil);
    if(returnCode)
    {
        goto Error1;
    }
    
    printf("-----------------------------------------------------------------------------\n\r");
#if _FS_TINY
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM6.  Forward file <f_forward>                                                       */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r6. Demo functions:f_forward\n\n\r"); 
    /* Open File_Rm.txt to read */
    printf("6.0. Open <File_Rm.txt> to read\n\r");
    returnCode = f_open(&fil,_T("File_Rm.txt"),FA_READ);
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Forward file to ternimal */ 
    printf("6.1. Forward file to terminal\n\r");
    returnCode = f_forward(&fil, out_stream, 102,&size);   
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Close file */
    printf("\n\r6.2. Close file\n\r");
    returnCode = f_close(&fil);
    if(returnCode)
    {
        goto Error1;
    }
#endif
    
    
    printf("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM7.  Delete a file <f_ulink>                                                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    printf("\n\r7. Demo functions:f_ulink\n\n");
    printf(" Delete <File_Rm.txt>\n\r");
    /* returnCode = f_unlink(_T("File_Rm.txt")); */
    if(returnCode) 
    {
        goto Error1;
    }
    else 
    {
        List_Directory(_T("."));
    }

#endif /* End of #if (FILE_OPERATION) */

#if _USE_LFN
    if(NULL != Finfo.lfname)
    {
        /* Free memory for Finfo->lfname */
        ff_memfree(Finfo.lfname);
    }
#endif
    printf("\n\r*------------------------------   DEMO COMPLETED    ------------------------ *");
    printf("\n\r******************************************************************************\n\r");
    return FR_OK;       
Error1:
#if _USE_LFN
    if(NULL != Finfo.lfname)
    {
        /* Free memory for Finfo->lfname */
        ff_memfree(Finfo.lfname);
    }
#endif
    return(put_rc(returnCode));
}

