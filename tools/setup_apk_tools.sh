#!/usr/bin/env bash
# Download apktool + uber-apk-signer into tools/ (they're standalone JARs).
# Usage: tools/setup_apk_tools.sh
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

APKTOOL_VERSION="2.12.0"
APKSIGNER_VERSION="1.3.0"

if [ ! -f apktool.jar ]; then
    echo "[*] Downloading apktool $APKTOOL_VERSION..."
    wget -q --show-progress -O apktool.jar \
        "https://bitbucket.org/iBotPeaches/apktool/downloads/apktool_${APKTOOL_VERSION}.jar"
fi

if [ ! -f uber-apk-signer.jar ]; then
    echo "[*] Downloading uber-apk-signer $APKSIGNER_VERSION..."
    wget -q --show-progress -O uber-apk-signer.jar \
        "https://github.com/patrickfav/uber-apk-signer/releases/download/v${APKSIGNER_VERSION}/uber-apk-signer-${APKSIGNER_VERSION}.jar"
fi

echo "[*] Done. Tools:"
ls -la apktool.jar uber-apk-signer.jar
echo ""
echo "[*] Quick smoke test:"
java -jar apktool.jar --version
java -jar uber-apk-signer.jar --version
