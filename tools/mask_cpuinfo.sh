#!/usr/bin/env bash
# Bind-mount a fake ARM64 /proc/cpuinfo over the emulator's real one so
# anti-emulator detection that reads /proc/cpuinfo gets ARM identifiers.
# Requires root on the device (LDPlayer/Bluestacks-rooted/etc).
#
# The fake cpuinfo claims to be a Snapdragon 888 (Cortex-A78 cores) — same
# chip as Samsung Galaxy S21 Ultra, which is what the emulator already
# claims via ro.product.model, so the picture is consistent.
#
# Usage: tools/mask_cpuinfo.sh
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh" > /dev/null

FAKE_CPUINFO="$SCRIPT_DIR/fake_cpuinfo_arm64.txt"

if [ ! -f "$FAKE_CPUINFO" ]; then
    echo "[!] $FAKE_CPUINFO missing"
    exit 1
fi

if ! adb get-state > /dev/null 2>&1; then
    echo "[!] No adb device — run bs_connect.sh first"
    exit 1
fi

# Push fake cpuinfo to device
echo "[*] Pushing fake cpuinfo to /data/local/tmp/cpuinfo"
adb push "$FAKE_CPUINFO" /data/local/tmp/cpuinfo > /dev/null

# Check we have root
ROOT_CHECK=$(adb shell 'su -c id 2>&1' 2>&1 | tr -d '\r')
if ! echo "$ROOT_CHECK" | grep -q 'uid=0'; then
    echo "[!] No root on device. Got: $ROOT_CHECK"
    echo "    Enable root in emulator settings and retry."
    exit 1
fi

# Bind-mount fake cpuinfo over /proc/cpuinfo
echo "[*] Bind-mounting /data/local/tmp/cpuinfo over /proc/cpuinfo (needs root)"
adb shell 'su -c "mount --bind /data/local/tmp/cpuinfo /proc/cpuinfo"' 2>&1 | tr -d '\r'

echo ""
echo "=== verify on device ==="
adb shell 'head -3 /proc/cpuinfo' 2>&1 | tr -d '\r' | head -5
echo "..."

echo ""
echo "[*] Done. Now relaunch Warspear:"
echo "    adb shell am start -n com.aigrind.warspear/.LoaderActivity"
echo ""
echo "[*] NOTE: the bind-mount survives until the emulator reboots."
echo "    To undo: adb shell 'su -c \"umount /proc/cpuinfo\"'"
