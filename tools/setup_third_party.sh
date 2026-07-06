#!/usr/bin/env bash
# Fetch third_party dependencies that aren't tracked in this repo.
# Run once after cloning. Idempotent.
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TP="$PROJECT_ROOT/third_party"
mkdir -p "$TP"

# ─── ImGui v1.91.4 — needed for the overlay (GLES2 backend) ───────────────
if [ ! -d "$TP/imgui" ]; then
    echo "[*] Cloning ImGui v1.91.4..."
    git clone --depth 1 --branch v1.91.4 https://github.com/ocornut/imgui.git "$TP/imgui"
else
    echo "[*] $TP/imgui already present"
fi

# ─── xhook — PLT hook lib (kept vendored for future real-device builds) ──
if [ ! -d "$TP/xhook" ]; then
    echo "[*] Cloning xhook..."
    git clone --depth 1 https://github.com/iqiyi/xhook.git "$TP/xhook"
else
    echo "[*] $TP/xhook already present"
fi

# ─── shadowhook — inline hook lib (idem, not used under Houdini) ──────────
if [ ! -d "$TP/shadowhook" ]; then
    echo "[*] Cloning shadowhook..."
    git clone --depth 1 https://github.com/bytedance/android-inline-hook.git "$TP/shadowhook"
else
    echo "[*] $TP/shadowhook already present"
fi

echo ""
echo "[*] Third-party deps ready:"
ls -d "$TP"/*/ 2>/dev/null | sed 's|/$||'
echo ""
echo "[*] Next: tools/setup_apk_tools.sh (downloads apktool + uber-apk-signer)"
