#!/bin/sh
set -eu

if [ "$#" -lt 1 ]; then
    echo "Usage: ./debug.sh <serial-port> [esp32|esp32-s3|tdisplay]"
    exit 1
fi

port="$1"
device="${2:-esp32}"

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
        exit 1
        ;;
esac

sh build.sh "$device"
arduino-cli upload --input-dir build --fqbn "$fqbn" -p "$port"
arduino-cli monitor -p "$port" -c baudrate=115200
