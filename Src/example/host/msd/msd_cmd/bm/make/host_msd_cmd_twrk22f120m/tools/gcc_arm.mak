#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/debug/usbh/bm/usbh.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/release/usbh/bm/usbh.a
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
INCLUDE += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/debug/usbh/bm
endif
ifeq ($(CONFIG),release)
INCLUDE += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/release/usbh/bm
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
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk22f120m/gcc_arm/startup.c
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk22f120m/gcc_arm/startup_MK22F51212.S
SOURCES += $(USB_ROOTDIR)/platform/bsp/twrk22f120m/gcc_arm/system_MK22F51212.c



