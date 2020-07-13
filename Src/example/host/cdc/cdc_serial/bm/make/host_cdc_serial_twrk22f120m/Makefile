#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
USB_ROOTDIR         = $(abspath ../../../../../../..)
LIBRARY_ROOTDIR         = 
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = host_cdc_serial
BOARD               = twrk22f120m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(USB_ROOTDIR)/example/host/cdc/cdc_serial/bm/make/host_cdc_serial_twrk22f120m
INCLUDE += $(USB_ROOTDIR)/example/host/cdc/cdc_serial
INCLUDE += $(USB_ROOTDIR)/platform/arch/cortex_m
INCLUDE += $(USB_ROOTDIR)/platform/bsp/common/kinetis
INCLUDE += $(USB_ROOTDIR)/platform/bsp/common
INCLUDE += $(USB_ROOTDIR)/platform/bsp/peripheral/timer
INCLUDE += $(USB_ROOTDIR)/platform/bsp/peripheral/serial
INCLUDE += $(USB_ROOTDIR)/platform/bsp/twrk22f120m
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212/MK22F51212
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212
INCLUDE += $(USB_ROOTDIR)/os/bm
endif
ifeq ($(CONFIG),release)
INCLUDE += $(USB_ROOTDIR)/example/host/cdc/cdc_serial/bm/make/host_cdc_serial_twrk22f120m
INCLUDE += $(USB_ROOTDIR)/example/host/cdc/cdc_serial
INCLUDE += $(USB_ROOTDIR)/platform/arch/cortex_m
INCLUDE += $(USB_ROOTDIR)/platform/bsp/common/kinetis
INCLUDE += $(USB_ROOTDIR)/platform/bsp/common
INCLUDE += $(USB_ROOTDIR)/platform/bsp/peripheral/timer
INCLUDE += $(USB_ROOTDIR)/platform/bsp/peripheral/serial
INCLUDE += $(USB_ROOTDIR)/platform/bsp/twrk22f120m
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212/MK22F51212
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212
INCLUDE += $(USB_ROOTDIR)/os/bm
endif



#-----------------------------------------------------------
# common 'app' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/example/host/cdc/cdc_serial/cdc_serial.c
SOURCES += $(USB_ROOTDIR)/example/host/cdc/cdc_serial/usb_host_cdc_io.c

#-----------------------------------------------------------
# common 'arch' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/platform/arch/cortex_m/nvic.c

#-----------------------------------------------------------
# common 'bsp' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/platform/bsp/common/kinetis/main_kinetis.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/common/kinetis/wdt_kinetis.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/peripheral/serial/serl_int_kuart.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/peripheral/serial/serl_int.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk22f120m/init_gpio.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/peripheral/timer/pit_delay.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/peripheral/timer/rtc_kinetis.c

#-----------------------------------------------------------
# common 'soc' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/platform/soc/MK22F51212/soc_init.c
SOURCES += $(USB_ROOTDIR)/platform/soc/MK22F51212/soc_pit.c
SOURCES += $(USB_ROOTDIR)/platform/soc/MK22F51212/soc_isr.c
SOURCES += $(USB_ROOTDIR)/platform/soc/MK22F51212/soc_usb.c
SOURCES += $(USB_ROOTDIR)/platform/soc/MK22F51212/soc_sci.c

#-----------------------------------------------------------
# common 'sys' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/os/bm/bmevent.c
SOURCES += $(USB_ROOTDIR)/os/bm/bmmsgq.c
SOURCES += $(USB_ROOTDIR)/os/bm/bmsem.c
SOURCES += $(USB_ROOTDIR)/os/bm/bmisr.c
SOURCES += $(USB_ROOTDIR)/os/bm/mem_util.c
SOURCES += $(USB_ROOTDIR)/os/bm/poll.c
SOURCES += $(USB_ROOTDIR)/os/bm/printf.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(USB_ROOTDIR)/build/common/make/global.mak
-include $(USB_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(USB_ROOTDIR)/build/common/make/verify.mak
include $(USB_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
