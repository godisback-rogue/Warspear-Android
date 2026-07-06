#!/usr/bin/env bash
# Apply ws-android patches to an apktool-extracted Warspear APK.
#
# 1. Copy build/<abi>/libinjector.so into extracted/lib/<abi>/
# 2. Inject System.loadLibrary("injector") into MDActivity.smali <clinit>
# 3. Drop com/wsAndroid/Overlay.smali into the extracted smali tree
# 4. Patch MDRenderer.smali so that onDrawFrame, after calling the game's
#    Native.onDrawFrame(), also calls Overlay.draw() (our overlay)
#
# Idempotent: re-running on an already-patched tree is a no-op.
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
EXTRACTED="${1:-$PROJECT_ROOT/apk/extracted}"
BUILD_DIR="$PROJECT_ROOT/build"

if [ ! -d "$EXTRACTED" ]; then
    echo "[!] $EXTRACTED missing — run apk/unpack.sh first"; exit 1
fi

# ─── 1. libinjector.so ────────────────────────────────────────────────────
echo "[*] Copying libinjector.so into APK lib dirs..."
for abi in arm64-v8a armeabi-v7a; do
    src="$BUILD_DIR/$abi/libinjector.so"
    dst="$EXTRACTED/lib/$abi/libinjector.so"
    if [ ! -f "$src" ]; then
        echo "[!] missing $src — build first via android-injector/"
        exit 2
    fi
    cp "$src" "$dst"
    echo "    $abi: $(stat -c %s "$dst") bytes"
done

# ─── 2. MDActivity.smali loadLibrary patch ────────────────────────────────
SMALI_ACT="$EXTRACTED/smali/com/aigrind/warspear/MDActivity.smali"
[ -f "$SMALI_ACT" ] || { echo "[!] $SMALI_ACT not found"; exit 3; }

if grep -q 'ws-android injector' "$SMALI_ACT"; then
    echo "[*] MDActivity.smali already patched — skipping load injection"
else
    echo "[*] Patching MDActivity.smali (load injector before warspear)..."
    python3 - "$SMALI_ACT" <<'PYEOF'
import sys
path = sys.argv[1]
with open(path) as f: src = f.read()
ANCHOR = 'invoke-static {v0, v1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I'
INJECT = '''invoke-static {v0, v1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    # === ws-android injector: load BEFORE libwarspear ===
    const-string v0, "injector"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
    # === end injector ===
'''
if ANCHOR not in src:
    print("[!] anchor not found in MDActivity.smali", file=sys.stderr); sys.exit(4)
src = src.replace(ANCHOR, INJECT, 1)
open(path,'w').write(src)
PYEOF
fi

# ─── 3. Drop Overlay.smali into the smali tree ────────────────────────────
OVERLAY_DST_DIR="$EXTRACTED/smali/com/wsAndroid"
mkdir -p "$OVERLAY_DST_DIR"
cp "$SCRIPT_DIR/Overlay.smali" "$OVERLAY_DST_DIR/Overlay.smali"
echo "[*] Dropped Overlay.smali into smali/com/wsAndroid/"

# ─── 4. MDRenderer.smali — call Overlay.draw() after Native.onDrawFrame ──
SMALI_RND="$EXTRACTED/smali/com/aigrind/warspear/MDRenderer.smali"
[ -f "$SMALI_RND" ] || { echo "[!] $SMALI_RND not found"; exit 5; }

if grep -q 'com/wsAndroid/Overlay' "$SMALI_RND"; then
    echo "[*] MDRenderer.smali already patched — skipping draw injection"
else
    echo "[*] Patching MDRenderer.smali to call Overlay.draw()..."
    python3 - "$SMALI_RND" <<'PYEOF'
import sys
path = sys.argv[1]
with open(path) as f: src = f.read()
# Anchor: the existing Native.onDrawFrame() call inside the onDrawFrame method
ANCHOR = '    invoke-static {}, Lcom/aigrind/mobiledragon/Native;->onDrawFrame()V'
INJECT = '''    invoke-static {}, Lcom/aigrind/mobiledragon/Native;->onDrawFrame()V

    # === ws-android overlay: draw on top of the game's framebuffer ===
    invoke-static {}, Lcom/wsAndroid/Overlay;->draw()V
    # === end overlay ===
'''
if ANCHOR not in src:
    print("[!] anchor not found in MDRenderer.smali", file=sys.stderr); sys.exit(6)
src = src.replace(ANCHOR, INJECT, 1)
open(path,'w').write(src)
PYEOF
fi

# ─── 5. MDSurfaceView.smali — route touches to Overlay.onTouch first ──────
SMALI_SV="$EXTRACTED/smali/com/aigrind/warspear/MDSurfaceView.smali"
[ -f "$SMALI_SV" ] || { echo "[!] $SMALI_SV not found"; exit 7; }

if grep -q 'Overlay;->onTouch' "$SMALI_SV"; then
    echo "[*] MDSurfaceView.smali already patched — skipping touch injection"
else
    echo "[*] Patching MDSurfaceView.smali to route touches through Overlay.onTouch..."
    python3 - "$SMALI_SV" <<'PYEOF'
import sys
path = sys.argv[1]
with open(path) as f: src = f.read()
# Anchor: the start of onTouchEvent method body, just after .locals.
ANCHOR = '.method public onTouchEvent(Landroid/view/MotionEvent;)Z\n    .locals 5'
# Use registers v4 (available — method has 5 locals) for the return value.
# If Overlay.onTouch returns true, we consume the event by returning true.
INJECT = '''.method public onTouchEvent(Landroid/view/MotionEvent;)Z
    .locals 5

    # === ws-android: let Overlay see the touch first ===
    invoke-static {p1}, Lcom/wsAndroid/Overlay;->onTouch(Landroid/view/MotionEvent;)Z
    move-result v4
    if-eqz v4, :ws_touch_pass
    const/4 v0, 0x1
    return v0
    :ws_touch_pass
    # === end overlay touch ==='''
if ANCHOR not in src:
    print("[!] anchor not found in MDSurfaceView.smali", file=sys.stderr); sys.exit(8)
src = src.replace(ANCHOR, INJECT, 1)
open(path,'w').write(src)
PYEOF
fi

echo "[*] Done. Patches applied. Run apk/repack.sh to rebuild + sign."
