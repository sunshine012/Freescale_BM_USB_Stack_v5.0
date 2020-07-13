#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(USB_ROOTDIR)/output/twrk64f120m.gcc_arm/debug/usbotg/bm/usbotg.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(USB_ROOTDIR)/output/twrk64f120m.gcc_arm/release/usbotg/bm/usbotg.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/armv7e-m/softfp/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libnosys.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/armv7e-m/softfp/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libnosys.a
endif


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(USB_ROOTDIR)/platform/bsp/twrk64f120m/gcc_arm
INCLUDE += $(USB_ROOTDIR)/output/twrk64f120m.gcc_arm/debug/usbotg/bm
endif
ifeq ($(CONFIG),release)
INCLUDE += $(USB_ROOTDIR)/platform/bsp/twrk64f120m/gcc_arm
INCLUDE += $(USB_ROOTDIR)/output/twrk64f120m.gcc_arm/release/usbotg/bm
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



#-----------------------------------------------------------
# 'gcc_arm' sources
#-----------------------------------------------------------
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk64f120m/gcc_arm/startup.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk64f120m/gcc_arm/startup_MK64FN1M0.S
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk64f120m/gcc_arm/system_MK64FN1M0.c



#-----------------------------------------------------------
# 'debug' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
CC_DEFINE += USBCFG_OTG=1 
endif


#-----------------------------------------------------------
# 'release' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),release)
CC_DEFINE += USBCFG_OTG=1 
endif


