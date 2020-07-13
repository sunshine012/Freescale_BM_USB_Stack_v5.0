#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212/MK22F51212
endif
ifeq ($(CONFIG),release)
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212
INCLUDE += $(USB_ROOTDIR)/platform/soc/MK22F51212/MK22F51212
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif




