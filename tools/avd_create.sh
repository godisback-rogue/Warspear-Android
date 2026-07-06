#!/usr/bin/env bash
# Create the ws_api33 AVD if it doesn't exist.
# Usage: tools/avd_create.sh [avd_name]
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh" > /dev/null

AVD_NAME="${1:-ws_api33}"
SYSIMG="system-images;android-33;google_apis;x86_64"
DEVICE="pixel_5"

if emulator -list-avds 2>/dev/null | grep -q "^${AVD_NAME}$"; then
    echo "[*] AVD '$AVD_NAME' already exists. Existing AVDs:"
    emulator -list-avds
    exit 0
fi

# Confirm sysimg is installed
if ! sdkmanager --list_installed 2>/dev/null | grep -q "system-images;android-33;google_apis;x86_64"; then
    echo "[!] System image $SYSIMG not installed. Run:"
    echo "    sdkmanager '$SYSIMG'"
    exit 1
fi

echo "[*] Creating AVD '$AVD_NAME' (Pixel 5, API 33, x86_64)..."
echo "no" | avdmanager create avd \
    -n "$AVD_NAME" \
    -k "$SYSIMG" \
    -d "$DEVICE" \
    --force

AVD_DIR="$HOME/.android/avd/${AVD_NAME}.avd"
CONFIG="$AVD_DIR/config.ini"

if [ -f "$CONFIG" ]; then
    echo "[*] Tuning $CONFIG for headless software-rendered usage..."
    # Bump RAM and storage, set keyboard/display
    {
        echo ""
        echo "# tuning added by ws-android avd_create.sh"
        echo "hw.ramSize=2048"
        echo "vm.heapSize=512"
        echo "disk.dataPartition.size=8G"
        echo "hw.keyboard=yes"
        echo "hw.lcd.density=420"
        echo "hw.lcd.width=1080"
        echo "hw.lcd.height=2340"
        echo "showDeviceFrame=no"
        echo "skin.dynamic=yes"
    } >> "$CONFIG"
fi

echo ""
echo "[*] AVD created. Boot with: tools/emu_start.sh $AVD_NAME"
echo "[*] Config: $CONFIG"
