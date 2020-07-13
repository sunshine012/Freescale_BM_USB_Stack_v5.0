@echo OFF
set NOPAUSE=%1

mingw32-make TOOL=gcc_arm CONFIG=debug LINKER_FILE=$(USB_ROOTDIR)/platform/bsp/twrk70f120m/gcc_arm/MK70FN1M0_OTG_Flash.ld clean 
if errorlevel 1 (
set NOPAUSE=0
pause
)

mingw32-make TOOL=gcc_arm CONFIG=release LINKER_FILE=$(USB_ROOTDIR)/platform/bsp/twrk70f120m/gcc_arm/MK70FN1M0_OTG_Flash.ld clean 
if errorlevel 1 (
set NOPAUSE=0
pause
)

if not "%1" == "nopause" (pause)
