#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
USB_ROOTDIR            = $(abspath ../../../../..)
OS                     = bm
#-----------------------------------------------
# set board infomation
#-----------------------------------------------

TYPE                    = library
NAME                    = usbh
BOARD                   = twrk64f120m
LIBRARY_ROOTDIR         = $(USB_ROOTDIR)/output/$(BOARD).$(TOOL)
USB_LIBRARY_ROOTDIR     = $(USB_ROOTDIR)/output/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(USB_LIBRARY_ROOTDIR)/$(CONFIG)/usbh/bm
LIBRARY_FILE            = $(LIBRARY_DIR)/usbh.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(USB_ROOTDIR)/usb_core/host/build/bat/usbh_twrk64f120m.bat $(USB_ROOTDIR) $(LIBRARY_DIR) $(TOOL) bm,$(USB_ROOTDIR)/usb_core/host/build/bat/usbh_twrk64f120m.sh $(USB_ROOTDIR) $(LIBRARY_DIR) $(TOOL) bm)


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(USB_ROOTDIR)/adapter/sources
INCLUDE += $(USB_ROOTDIR)/adapter/sources/bm
INCLUDE += $(USB_ROOTDIR)/os/bm
INCLUDE += $(USB_ROOTDIR)/usb_core/hal
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/audio
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/cdc
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/hid
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/hub
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/msd
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/phdc
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/controller
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/controller/khci
INCLUDE += $(USB_ROOTDIR)/usb_core/host/include/twrk64f120m
INCLUDE += $(USB_ROOTDIR)/usb_core/host/include
INCLUDE += $(USB_ROOTDIR)/usb_core/include
endif
ifeq ($(CONFIG),release)
INCLUDE += $(USB_ROOTDIR)/adapter/sources
INCLUDE += $(USB_ROOTDIR)/adapter/sources/bm
INCLUDE += $(USB_ROOTDIR)/os/bm
INCLUDE += $(USB_ROOTDIR)/usb_core/hal
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/audio
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/cdc
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/hid
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/hub
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/msd
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/classes/phdc
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/controller
INCLUDE += $(USB_ROOTDIR)/usb_core/host/sources/controller/khci
INCLUDE += $(USB_ROOTDIR)/usb_core/host/include/twrk64f120m
INCLUDE += $(USB_ROOTDIR)/usb_core/host/include
INCLUDE += $(USB_ROOTDIR)/usb_core/include
endif



#-----------------------------------------------------------
# 'adapter' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/adapter/sources/bm/adapter_bm.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_audio' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/audio/usb_host_audio.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/audio/usb_host_audio_intf.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_cdc' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/cdc/usb_host_cdc.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/cdc/usb_host_cdc_intf.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_common' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/common/usb_classes.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_hid' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/hid/usb_host_hid.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_hub' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/hub/usb_host_hub_sm.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/hub/usb_host_hub.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_msd' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/msd/usb_host_msd_bo.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/msd/usb_host_msd_queue.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/msd/usb_host_msd_ufi.c

#-----------------------------------------------------------
# 'usb_core_host_class_driver_phdc' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/classes/phdc/usb_host_phdc.c

#-----------------------------------------------------------
# 'usb_core_host_controller_driver' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/controller/usb_host_ch9.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/controller/usb_host_common.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/controller/usb_host_dev_mng.c
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/controller/usb_host.c

#-----------------------------------------------------------
# 'usb_core_host_controller_driver_khci' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/controller/khci/khci.c

#-----------------------------------------------------------
# 'usb_core_host_sources_bsp' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/usb_core/host/sources/bsp/twrk64f120m/usb_host_bsp.c


#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(USB_ROOTDIR)/build/common/make/global.mak
-include $(USB_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(USB_ROOTDIR)/build/common/make/verify.mak
include $(USB_ROOTDIR)/build/common/make/lib-process.mak
#----------------------------------------------------------
