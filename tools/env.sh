# Source this file: `source tools/env.sh`
# Sets up ANDROID_HOME, NDK path, and adds CLI tools to PATH.

export ANDROID_HOME="${ANDROID_HOME:-$HOME/Android/Sdk}"
export ANDROID_SDK_ROOT="$ANDROID_HOME"

# Pick the newest installed NDK
if [ -d "$ANDROID_HOME/ndk" ]; then
    export ANDROID_NDK_ROOT="$(ls -1d "$ANDROID_HOME/ndk"/*/ 2>/dev/null | sort -V | tail -1)"
    export ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT%/}"
fi

export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/emulator:$ANDROID_HOME/build-tools/33.0.2:$PATH"

if [ -n "$ANDROID_NDK_ROOT" ]; then
    export PATH="$ANDROID_NDK_ROOT:$PATH"
fi

echo "[env] ANDROID_HOME    = $ANDROID_HOME"
echo "[env] ANDROID_NDK_ROOT= ${ANDROID_NDK_ROOT:-<none installed yet>}"
echo "[env] adb             = $(command -v adb || echo not found)"
echo "[env] emulator        = $(command -v emulator || echo not found)"
