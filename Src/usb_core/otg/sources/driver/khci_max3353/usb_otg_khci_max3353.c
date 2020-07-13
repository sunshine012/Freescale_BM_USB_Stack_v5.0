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
* $FileName: usb_otg_khci_max3353.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*         
*****************************************************************************/
#include "usb.h"
#include "usb_otg_main.h"
#include "usb_otg_private.h"
#include "usb_otg_khci.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "usb_otg_max3353_mqx.h"
#else
#include "usb_otg_max3353.h"
#endif
#include "usb_otg_khci_max3353_prv.h"

/* Prototypes of functions */
static usb_status _usb_otg_khci_max3353_preinit(usb_otg_handle* handle);
static usb_status _usb_otg_khci_max3353_init(uint8_t controller_id, usb_otg_handle handle);
static usb_status _usb_otg_khci_max3353_shutdown(usb_otg_handle handle);
static usb_status _usb_otg_khci_max3353_get_status(usb_otg_handle handle);
static usb_status _usb_otg_khci_max3353_set_vbus(usb_otg_handle handle, bool enable);
static usb_status _usb_otg_khci_max3353_set_pull_downs(usb_otg_handle handle, uint8_t bitfield );
static usb_status _usb_otg_khci_max3353_set_dp_pull_up(usb_otg_handle handle, bool enable);
static usb_status _usb_otg_khci_max3353_generate_resume(usb_otg_handle handle , bool enable);
extern void* bsp_usb_otg_get_max3353_init_param(uint8_t controller_id);
const usb_otg_api_functions_struct_t g_usb_khci_max3353_otg_callback_table = {
    /* The otg preinit function */
    _usb_otg_khci_max3353_preinit,
    
    /* The otg init function */
    _usb_otg_khci_max3353_init,

    /* The function to shutdown the otg */
    _usb_otg_khci_max3353_shutdown,

    /* The function to get the otg status */
    _usb_otg_khci_max3353_get_status,

    /* The function to control the bus voltage */
    _usb_otg_khci_max3353_set_vbus,

    /* The function to control the pull downs */
    _usb_otg_khci_max3353_set_pull_downs,

    /* The function to control the dp pull up */
    _usb_otg_khci_max3353_set_dp_pull_up,

    /* The function to start/stop resume signalling */        
    _usb_otg_khci_max3353_generate_resume

};

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_preinit
*  Returned Value : 
*  Comments       :
*       
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_preinit
(
    usb_otg_handle* handle
)
{
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_ptr;
    void                            *khci_call_struct_ptr;
    void                            *max3353_call_struct_ptr;
    
    otg_khci_max3353_ptr = (usb_otg_khci_max3353_struct_t *)OS_Mem_alloc_zero(sizeof(usb_otg_khci_max3353_struct_t));
    
    if (otg_khci_max3353_ptr == NULL)
    {
        return USBERR_ALLOC;
    }    
    khci_call_struct_ptr =  (void*)OS_Mem_alloc_zero(sizeof(usb_otg_khci_call_struct_t));
    if (khci_call_struct_ptr == NULL)
    {
        OS_Mem_free(otg_khci_max3353_ptr);
        otg_khci_max3353_ptr = NULL;
        return USBERR_ALLOC;
    }
    max3353_call_struct_ptr =  (void*)OS_Mem_alloc_zero(sizeof(usb_otg_max3353_call_struct_t));
    if (max3353_call_struct_ptr == NULL)
    {
        OS_Mem_free(khci_call_struct_ptr);    
        OS_Mem_free(otg_khci_max3353_ptr);
        khci_call_struct_ptr = NULL;
        otg_khci_max3353_ptr = NULL;
        return USBERR_ALLOC;
    }
    otg_khci_max3353_ptr->g.init_param = (void*)bsp_usb_otg_get_max3353_init_param(0);
    if (otg_khci_max3353_ptr->g.init_param == NULL)
    {
        return USBERR_ALLOC;
    }
    otg_khci_max3353_ptr->khci_call_ptr = (usb_otg_khci_call_struct_t *)khci_call_struct_ptr;
    otg_khci_max3353_ptr->max3353_call_ptr = (usb_otg_max3353_call_struct_t *)max3353_call_struct_ptr;
    *handle = otg_khci_max3353_ptr; 

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_init
*  Returned Value : 
*  Comments       :
*       
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_init
(
    uint8_t         controller_id, 
    usb_otg_handle handle
)
{
    /* Initialize the OTG controller */
    usb_status                              error;
    usb_otg_khci_max3353_struct_t *         otg_khci_max3353_handle_ptr =(usb_otg_khci_max3353_struct_t *)handle;
    usb_otg_state_struct_t *                otg_handle_ptr = &(otg_khci_max3353_handle_ptr->g);
    usb_khci_max3353_otg_init_struct_t *    khci_max3353_init_param_ptr; 

    otg_khci_max3353_handle_ptr->khci_call_ptr->otg_handle_ptr = otg_handle_ptr;
    khci_max3353_init_param_ptr = (usb_khci_max3353_otg_init_struct_t *)otg_handle_ptr->init_param;
    otg_khci_max3353_handle_ptr->khci_call_ptr->init_param_ptr = &(khci_max3353_init_param_ptr->khci);;
    otg_khci_max3353_handle_ptr->max3353_call_ptr->init_param_ptr = &(khci_max3353_init_param_ptr->max3353);
    otg_khci_max3353_handle_ptr->max3353_call_ptr->otg_handle_ptr = otg_handle_ptr;
    
    error = _usb_otg_khci_init(otg_khci_max3353_handle_ptr->khci_call_ptr);
    if (error != USB_OK)
    {
        return error;
    }
    error = _usb_otg_max3353_init(otg_khci_max3353_handle_ptr->max3353_call_ptr);
    if (error != USB_OK)
    {
        return error;
    }
    return USB_OK;
}
  
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_shutdown
*  Returned Value : 0 successful
*  Comments       :
*       
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_shutdown
(
    usb_otg_handle handle
)
{
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_handle_ptr = (usb_otg_khci_max3353_struct_t *)handle;
    OS_Mem_free(otg_khci_max3353_handle_ptr->khci_call_ptr);
    OS_Mem_free(otg_khci_max3353_handle_ptr->max3353_call_ptr);
    otg_khci_max3353_handle_ptr->khci_call_ptr = NULL;
    otg_khci_max3353_handle_ptr->max3353_call_ptr = NULL;
    return USB_OK;
} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_get_status
*  Returned Value : 0 successful
*  Comments       :
*       
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_get_status
(
    usb_otg_handle handle
)
{
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_handle_ptr = (usb_otg_khci_max3353_struct_t *)handle;
    usb_otg_state_struct_t *        usb_otg_struct_ptr          = (usb_otg_state_struct_t *)handle;

    /* *** Handle the events *** */    
    if (OS_Event_check_bit(usb_otg_struct_ptr->otg_isr_event, USB_OTG_MAX3353_ISR_EVENT))
    {
        OS_Event_clear(usb_otg_struct_ptr->otg_isr_event, USB_OTG_MAX3353_ISR_EVENT);  
        _usb_otg_max3353_get_status(otg_khci_max3353_handle_ptr->max3353_call_ptr);
    }
    if (OS_Event_check_bit(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT))
    {
        OS_Event_clear(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT);
        _usb_otg_khci_get_status(otg_khci_max3353_handle_ptr->khci_call_ptr);  
    }
    return USB_OK;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_set_vbus
*  Returned Value : none
*  Comments       :
*        
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_set_vbus
(
    usb_otg_handle handle, 
    bool            enable
)
{
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_handle_ptr = (usb_otg_khci_max3353_struct_t *)handle;
    return _usb_otg_max3353_set_vbus(otg_khci_max3353_handle_ptr->max3353_call_ptr, enable);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_set_pull_downs
*  Returned Value : none
*  Comments       :
*        
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_set_pull_downs
(
    usb_otg_handle handle, 
    uint8_t         bitfield 
)
{
    usb_status error;    
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_handle_ptr = (usb_otg_khci_max3353_struct_t *)handle;
    error = _usb_otg_khci_set_pull_downs(otg_khci_max3353_handle_ptr->khci_call_ptr, bitfield );
    if (error != USB_OK)
    {
        return error;      
    }
    return _usb_otg_max3353_set_pull_downs(otg_khci_max3353_handle_ptr->max3353_call_ptr, bitfield);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_set_pull_downs
*  Returned Value : none
*  Comments       :
*        
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_set_dp_pull_up
(
    usb_otg_handle handle, 
    bool            enable
)
{
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_handle_ptr = (usb_otg_khci_max3353_struct_t *)handle;    
    return _usb_otg_khci_set_dp_pull_up(otg_khci_max3353_handle_ptr->khci_call_ptr, enable);
    
}
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_otg_khci_max3353_generate_resume
*  Returned Value : none
*  Comments       :
*        
*END*-----------------------------------------------------------------*/
static usb_status _usb_otg_khci_max3353_generate_resume
(
    usb_otg_handle handle , 
    bool            enable
)
{
    usb_otg_khci_max3353_struct_t * otg_khci_max3353_handle_ptr = (usb_otg_khci_max3353_struct_t *)handle;
    return _usb_otg_khci_generate_resume(otg_khci_max3353_handle_ptr->khci_call_ptr, enable);
}
