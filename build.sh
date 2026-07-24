#!/bin/sh
set -eu

device="${1:-esp32}"

case "$device" in
    esp32)
        fqbn="esp32:esp32:esp32"
        ;;
    esp32-s3|esp32s3)
        device="esp32-s3"
        fqbn="esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc"
        ;;
    tdisplay)
        fqbn="esp32:esp32:ttgo-lora32"
        ;;
    *)
        echo "Unsupported device '$device'." >&2
        echo "Supported devices: esp32, esp32-s3, tdisplay" >&2
        exit 1
        ;;
esac

device_name=$(printf '%s' "$device" | tr '[:lower:]-' '[:upper:]_')
tft_config_file="tft_config_$device.txt"
tft_config=""
if [ -f "$tft_config_file" ]; then
    echo "Using TFT configuration file $tft_config_file."
    user_tft_config=$(tr '\n' ' ' < "$tft_config_file" | sed -e "s/\ /\ -D/g" -e "s/-D$//")
    tft_font="-DLOAD_GLCD=1 -DLOAD_FONT2=1 -DLOAD_FONT4=1 -DLOAD_FONT6=1 -DLOAD_FONT7=1 -DLOAD_FONT8=1 -DLOAD_GFXFF=1 -DSMOOTH_FONT=1"
    tft_config=" -DTFT=1 -DUSER_SETUP_LOADED=1 -D${user_tft_config} ${tft_font} -DSPI_FREQUENCY=27000000 -DSPI_READ_FREQUENCY=20000000"
fi

echo "Building $device ($fqbn)"
arduino-cli compile \
    --build-property "build.partitions=min_spiffs" \
    --build-property "upload.maximum_size=1966080" \
    --build-property "compiler.cpp.extra_flags=-D${device_name}${tft_config}" \
    --build-path build \
    --fqbn "$fqbn" bitcoinSwitch
