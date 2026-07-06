#!/usr/bin/env bash
# Decompile an APK with apktool.
# Usage: apk/unpack.sh <input.apk> [output_dir]
# Default output: apk/extracted/
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
APKTOOL="$PROJECT_ROOT/tools/apktool.jar"

if [ ! -f "$APKTOOL" ]; then
    echo "[!] $APKTOOL missing. Run tools/setup_apk_tools.sh first."
    exit 1
fi

INPUT_APK="${1:?usage: $0 <input.apk> [output_dir]}"
OUTPUT_DIR="${2:-$SCRIPT_DIR/extracted}"

if [ ! -f "$INPUT_APK" ]; then
    echo "[!] APK not found: $INPUT_APK"
    exit 1
fi

if [ -d "$OUTPUT_DIR" ]; then
    echo "[*] Removing existing $OUTPUT_DIR"
    rm -rf "$OUTPUT_DIR"
fi

echo "[*] Decompiling $INPUT_APK → $OUTPUT_DIR"
java -jar "$APKTOOL" d -o "$OUTPUT_DIR" "$INPUT_APK"

echo ""
echo "[*] Done. Structure:"
ls -la "$OUTPUT_DIR/"
echo ""
echo "[*] Native libs found:"
find "$OUTPUT_DIR/lib" -name "*.so" 2>/dev/null | head -20
