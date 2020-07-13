@echo OFF
SET CWD=%CD%

setlocal
cd %CWD%\..\..\..\usb_core\device\build\make\usbd_bm_twrk64f120m
cmd /c clean_gcc_arm nopause
cd %CWD%\..\..\..\usb_core\host\build\make\usbh_bm_twrk64f120m
cmd /c clean_gcc_arm nopause
cd %CWD%\..\..\..\usb_core\otg\build\make\usbotg_bm_twrk64f120m
cmd /c clean_gcc_arm nopause

if not "%1" == "nopause" (pause)
