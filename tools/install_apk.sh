#!/usr/bin/env bash
# Install an APK on the running emulator, retrying on transient failures.
# Usage: tools/install_apk.sh <path-to.apk>
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh" > /dev/null

APK="${1:?usage: $0 <path-to.apk>}"
if [ ! -f "$APK" ]; then
    echo "[!] APK not found: $APK"; exit 1
fi

if ! adb get-state > /dev/null 2>&1; then
    echo "[!] No adb device connected. Start emulator first."
    exit 1
fi

echo "[*] Installing $APK..."
adb install -r -d "$APK"

# Best-effort: derive package name and show install path
PKG=$(aapt dump badging "$APK" 2>/dev/null | awk -F"'" '/package: name=/{print $2; exit}')
if [ -n "$PKG" ]; then
    echo ""
    echo "[*] Package: $PKG"
    echo "[*] Install path(s):"
    adb shell pm path "$PKG"
fi
