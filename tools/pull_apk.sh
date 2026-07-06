#!/usr/bin/env bash
# Pull an APK from the connected device to ws-android/apk/.
# Usage:
#   tools/pull_apk.sh                              # auto-detect Warspear package
#   tools/pull_apk.sh com.aigrind.warspear        # explicit package name
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh" > /dev/null

PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUT_DIR="$PROJECT_ROOT/apk"
mkdir -p "$OUT_DIR"

if ! adb get-state > /dev/null 2>&1; then
    echo "[!] No adb device. Run tools/bs_connect.sh first."
    exit 1
fi

# 1. Find target package
PKG="$1"
if [ -z "$PKG" ]; then
    echo "[*] Searching for Warspear-ish packages..."
    CANDIDATES=$(adb shell pm list packages 2>/dev/null \
        | tr -d '\r' \
        | grep -iE 'warspear|aigrind' \
        | sed 's/^package://')
    if [ -z "$CANDIDATES" ]; then
        echo "[!] No Warspear-like packages found. Installed packages with 'game' in name:"
        adb shell pm list packages 2>/dev/null | tr -d '\r' | grep -i game | head -10
        echo ""
        echo "Install Warspear in Bluestacks first (Play Store), then re-run."
        exit 1
    fi
    PKG=$(echo "$CANDIDATES" | head -1)
    echo "[*] Found: $PKG"
fi

# 2. Get path(s) — apps can have split APKs (base + config)
echo "[*] Resolving install paths for $PKG ..."
PATHS=$(adb shell pm path "$PKG" 2>/dev/null | tr -d '\r' | sed 's/^package://')
if [ -z "$PATHS" ]; then
    echo "[!] Package $PKG not installed on device."
    exit 1
fi

# 3. Pull each part
n=0
for p in $PATHS; do
    name=$(basename "$p")
    dest="$OUT_DIR/${PKG}.${name%.apk}.apk"
    [ "$n" = "0" ] && dest="$OUT_DIR/${PKG}.apk"   # base.apk → <pkg>.apk
    echo "[*] Pulling: $p → $dest"
    adb pull "$p" "$dest" 2>&1 | tail -1
    n=$((n+1))
done

echo ""
echo "[*] Saved $n file(s) in $OUT_DIR/"
ls -la "$OUT_DIR/" | grep -E "\.apk$"
echo ""
echo "[*] Quick inspection (base APK):"
BASE_APK="$OUT_DIR/${PKG}.apk"
if [ -f "$BASE_APK" ]; then
    echo "  Size: $(du -h "$BASE_APK" | cut -f1)"
    if command -v unzip > /dev/null; then
        echo "  Native libs:"
        unzip -l "$BASE_APK" | grep -E '\.so$' | awk '{print "    "$4}' | head -20
        echo "  ABIs present:"
        unzip -l "$BASE_APK" | grep -oE 'lib/[^/]+/' | sort -u
    fi
fi
