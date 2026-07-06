#!/usr/bin/env bash
# Boot the AVD headless with software acceleration (no KVM).
# Usage: tools/emu_start.sh [avd_name] [--with-window]
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh"

AVD_NAME="${1:-ws_api33}"
WINDOW_MODE=""
[ "$2" = "--with-window" ] && WINDOW_MODE="--with-window"

if ! emulator -list-avds 2>/dev/null | grep -q "^${AVD_NAME}$"; then
    echo "[!] AVD '$AVD_NAME' not found. Available:"
    emulator -list-avds
    exit 1
fi

# Common flags for no-KVM environment:
#   -no-accel      : force software CPU emulation
#   -gpu swiftshader_indirect : software GL renderer
#   -no-snapshot-save : don't dirty the AVD on shutdown (snapshots can corrupt)
#   -netdelay none -netspeed full : disable network throttling
ARGS=(
    -avd "$AVD_NAME"
    -no-accel
    -gpu swiftshader_indirect
    -no-snapshot-save
    -no-boot-anim
    -netdelay none
    -netspeed full
    -no-audio
)

if [ "$WINDOW_MODE" = "--with-window" ]; then
    echo "[*] Booting $AVD_NAME (with window)..."
    DISPLAY="${DISPLAY:-:0}" emulator "${ARGS[@]}" &
else
    echo "[*] Booting $AVD_NAME (headless)..."
    emulator "${ARGS[@]}" -no-window -no-audio > /tmp/emulator.log 2>&1 &
fi

EMU_PID=$!
echo "[*] Emulator PID: $EMU_PID — log: /tmp/emulator.log"
echo "[*] Waiting for device to come online (this can take 3-5min with -no-accel)..."

# Wait for adb to see the device
adb wait-for-device
echo "[*] Device online. Waiting for boot completion..."

# Wait for sys.boot_completed=1
while [ "$(adb shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" != "1" ]; do
    sleep 5
done

echo "[*] Boot complete."
adb shell getprop ro.product.model
adb shell getprop ro.build.version.release
adb devices
