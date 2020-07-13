#!/bin/sh

# expect forward slash paths
USBROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"
OS="${4}"


# copy common files
mkdir -p "${OUTPUTDIR}"
cp -f "${USBROOTDIR}/usb_core/include/usb.h" "${OUTPUTDIR}/usb.h"
cp -f "${USBROOTDIR}/usb_core/include/types.h" "${OUTPUTDIR}/types.h"
cp -f "${USBROOTDIR}/usb_core/include/usb_desc.h" "${OUTPUTDIR}/usb_desc.h"
cp -f "${USBROOTDIR}/usb_core/include/usb_error.h" "${OUTPUTDIR}/usb_error.h"
cp -f "${USBROOTDIR}/usb_core/include/usb_misc.h" "${OUTPUTDIR}/usb_misc.h"
cp -f "${USBROOTDIR}/usb_core/include/usb_opt.h" "${OUTPUTDIR}/usb_opt.h"
cp -f "${USBROOTDIR}/usb_core/include/usb_types.h" "${OUTPUTDIR}/usb_types.h"
cp -f "${USBROOTDIR}/adapter/sources/adapter.h" "${OUTPUTDIR}/adapter.h"
cp -f "${USBROOTDIR}/adapter/sources/adapter_types.h" "${OUTPUTDIR}/adapter_types.h"
cp -f "${USBROOTDIR}/usb_core/device/include/usb_device_config.h" "${OUTPUTDIR}/usb_device_config.h"
cp -f "${USBROOTDIR}/usb_core/device/include/usb_device_stack_interface.h" "${OUTPUTDIR}/usb_device_stack_interface.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class_audio.h" "${OUTPUTDIR}/usb_class_audio.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class_cdc.h" "${OUTPUTDIR}/usb_class_cdc.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class.h" "${OUTPUTDIR}/usb_class.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class_composite.h" "${OUTPUTDIR}/usb_class_composite.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class_hid.h" "${OUTPUTDIR}/usb_class_hid.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class_msc.h" "${OUTPUTDIR}/usb_class_msc.h"
cp -f "${USBROOTDIR}/usb_core/device/sources/classes/include/usb_class_phdc.h" "${OUTPUTDIR}/usb_class_phdc.h"



# cw10 files
if [ "${TOOL}" = "cw10" ]; then
:
fi
# iar files
if [ "${TOOL}" = "iar" ]; then
:
fi
# cw10gcc files
if [ "${TOOL}" = "cw10gcc" ]; then
:
fi
# uv4 files
if [ "${TOOL}" = "uv4" ]; then
:
fi

# mqx files
if [ "${OS}" = "mqx"]; then
cp -f "${USBROOTDIR}/adapter/sources/mqx/adapter_mqx.h" "${OUTPUTDIR}/adapter_mqx.h"
cp -f "${USBROOTDIR}/adapter/sources/mqx/adapter_cfg.h" "${OUTPUTDIR}/adapter_cfg.h"
fi

# bm files
if [ "${OS}" = "bm"]; then
cp -f "${USBROOTDIR}/adapter/sources/bm/adapter_bm.h" "${OUTPUTDIR}/adapter_bm.h"
cp -f "${USBROOTDIR}/adapter/sources/bm/adapter_cfg.h" "${OUTPUTDIR}/adapter_cfg.h"
fi

# sdk files
if [ "${OS}" = "sdk"]; then
cp -f "${USBROOTDIR}/adapter/sources/sdk/adapter_sdk.h" "${OUTPUTDIR}/adapter_sdk.h"
cp -f "${USBROOTDIR}/adapter/sources/sdk/adapter_cfg.h" "${OUTPUTDIR}/adapter_cfg.h"
fi

