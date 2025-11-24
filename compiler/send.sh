#! /usr/bin/env bash

# ---- Check Arguments ----
if [ $# -lt 1 ]; then
    echo "Usage: $0 <program.bin> [serial_device]"
    exit 1
fi

FILE="$1"
DEVICE="${2:-/dev/ttyACM0}"    

# ---- Validate File ----
if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not found."
    exit 1
fi

# ---- Get File Size ----
SIZE=$(stat -c%s "$FILE")

echo "Sending '$FILE' ($SIZE bytes) to $DEVICE"

# ---- Send Size (2-byte little-endian) ----
SIZE_HEX=$(printf '%04x' $SIZE)
printf "\x${SIZE_HEX:2:2}\x${SIZE_HEX:0:2}" > "$DEVICE"

# ---- Send File Data ----
cat "$FILE" > "$DEVICE"

echo "Done."
