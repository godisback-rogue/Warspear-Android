#!/usr/bin/env bash
# Full pipeline: applies our patches to apk/extracted/, rebuilds with apktool,
# and signs with uber-apk-signer. Output: apk/repacked/signed/*.apk
#
# Pre-req: run apk/unpack.sh first to produce apk/extracted/
# Pre-req: build libinjector.so for arm64-v8a + armeabi-v7a first
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
APKTOOL="$PROJECT_ROOT/tools/apktool.jar"
SIGNER="$PROJECT_ROOT/tools/uber-apk-signer.jar"
EXTRACTED="$SCRIPT_DIR/extracted"
OUT_DIR="$SCRIPT_DIR/repacked"
SIGNED_DIR="$OUT_DIR/signed"

for j in "$APKTOOL" "$SIGNER"; do
    [ -f "$j" ] || { echo "[!] missing $j — run tools/setup_apk_tools.sh"; exit 1; }
done
[ -d "$EXTRACTED" ] || { echo "[!] $EXTRACTED missing — run apk/unpack.sh"; exit 1; }

mkdir -p "$OUT_DIR" "$SIGNED_DIR"

# ─── apply patches (idempotent) ───────────────────────────────────────────
"$SCRIPT_DIR/patches/apply.sh" "$EXTRACTED"

# ─── rebuild apk ──────────────────────────────────────────────────────────
UNSIGNED="$OUT_DIR/warspear-modded.apk"
echo "[*] Building $UNSIGNED from $EXTRACTED ..."
java -jar "$APKTOOL" b -o "$UNSIGNED" "$EXTRACTED"

# ─── sign with debug keystore ─────────────────────────────────────────────
echo "[*] Signing with uber-apk-signer (debug keystore — auto-generated) ..."
java -jar "$SIGNER" --apks "$UNSIGNED" --out "$SIGNED_DIR"

SIGNED=$(ls "$SIGNED_DIR"/*-debugSigned.apk 2>/dev/null | head -1)
if [ -n "$SIGNED" ]; then
    echo ""
    echo "[*] Final APK: $SIGNED ($(du -h "$SIGNED" | cut -f1))"
    echo ""
    echo "[*] Install on device:"
    echo "    adb uninstall com.aigrind.warspear     # if previously installed"
    echo "    adb install $SIGNED"
else
    echo "[!] No signed APK found in $SIGNED_DIR"
    ls -la "$SIGNED_DIR"
    exit 2
fi
