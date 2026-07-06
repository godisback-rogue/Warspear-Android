#!/usr/bin/env bash
# Discover and connect to Bluestacks ADB on the Windows host.
# Scans the current LAN /28 subnet for port 5555, picks the first hit.
# Usage:
#   tools/bs_connect.sh                 # auto-discover
#   tools/bs_connect.sh 192.168.1.100     # use explicit IP
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh" > /dev/null

PORT=5555
HOST_IP="$1"

if [ -z "$HOST_IP" ]; then
    # Pull the subnet base from our own IP and gateway
    MY_IP=$(ip -4 -o addr show enp0s3 | awk '{print $4}' | cut -d/ -f1)
    SUBNET=$(echo "$MY_IP" | awk -F. '{print $1"."$2"."$3}')
    echo "[*] Scanning ${SUBNET}.1-14 for port $PORT ..."
    for last in {1..14}; do
        ip="${SUBNET}.${last}"
        # Skip our own IP
        [ "$ip" = "$MY_IP" ] && continue
        if timeout 1 bash -c "</dev/tcp/$ip/$PORT" 2>/dev/null; then
            echo "[*] Found Bluestacks ADB at $ip:$PORT"
            HOST_IP="$ip"
            break
        fi
    done
    if [ -z "$HOST_IP" ]; then
        echo "[!] No host found with port $PORT open. Check Bluestacks ADB is on,"
        echo "    netsh portproxy is configured, and firewall rule is in place."
        exit 1
    fi
fi

# Disconnect any stale entries, then reconnect
adb disconnect "$HOST_IP:$PORT" 2>&1 | head -1
adb connect "$HOST_IP:$PORT" 2>&1 | head -1

echo ""
echo "=== adb devices ==="
adb devices -l

# Quick smoke test
if adb -s "$HOST_IP:$PORT" get-state > /dev/null 2>&1; then
    echo ""
    echo "=== device info ==="
    echo "Model:     $(adb -s "$HOST_IP:$PORT" shell getprop ro.product.model | tr -d '\r')"
    echo "Android:   $(adb -s "$HOST_IP:$PORT" shell getprop ro.build.version.release | tr -d '\r')"
    echo "API:       $(adb -s "$HOST_IP:$PORT" shell getprop ro.build.version.sdk | tr -d '\r')"
    echo "ABI:       $(adb -s "$HOST_IP:$PORT" shell getprop ro.product.cpu.abi | tr -d '\r')"
    echo "ABIList:   $(adb -s "$HOST_IP:$PORT" shell getprop ro.product.cpu.abilist | tr -d '\r')"

    # Export for current shell
    export WS_ADB_TARGET="$HOST_IP:$PORT"
    echo ""
    echo "[*] Connected. WS_ADB_TARGET=$WS_ADB_TARGET"
    echo "[*] Use: adb -s \$WS_ADB_TARGET <cmd>"
fi
