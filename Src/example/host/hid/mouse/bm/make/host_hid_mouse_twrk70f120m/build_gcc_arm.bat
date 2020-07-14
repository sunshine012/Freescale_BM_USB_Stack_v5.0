@echo OFF
set NOPAUSE=%1

mingw32-make TOOL=gcc_arm CONFIG=debug LINKER_FILE=$(USB_ROOTDIR)/platform/bsp/twrk70f120m/gcc_arm/MK70FN1M0_HOST_Flash.ld build
if errorlevel 1 (
set NOPAUSE=0
pause
)

REM mingw32-make TOOL=gcc_arm CONFIG=release LINKER_FILE=$(USB_ROOTDIR)/platform/bsp/twrk70f120m/gcc_arm/MK70FN1M0_HOST_Flash.ld build
REM if errorlevel 1 (
REM set NOPAUSE=0
REM pause
REM )

if not "%1" == "nopause" (pause)
