#include "game/PlayerWatch.h"
#include "game/HeapScan.h"
#include "game/Offsets_android.h"
#include "memory.h"
#include "logger.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <cstring>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

namespace ws::game {

namespace {

// Player Container vtable RVA — discovered 2026-05-13 by searching the
// in-game HP value (751) and finding the vtable pointer in the matching
// heap object's first 8 bytes. This is the canonical "this is the player's
// Container" signature for Warspear 13.3.0 on ARM64.
//
// vtable_addr at runtime = libwarspear_base + VTABLE_PLAYER_RVA
constexpr uintptr_t VTABLE_PLAYER_RVA = 0xC903A0;

std::mutex         s_mutex;
PlayerSnapshot     s_snap;
std::atomic<bool>  s_scanning{false};

// Safe memory read via process_vm_readv — returns false on bad addr.
bool safeRead(uintptr_t addr, void* buf, size_t len) {
    struct iovec local  { buf, len };
    struct iovec remote { reinterpret_cast<void*>(addr), len };
    return process_vm_readv(getpid(), &local, 1, &remote, 1, 0) == (ssize_t)len;
}

uint8_t safeU8(uintptr_t addr) {
    uint8_t v = 0; safeRead(addr, &v, 1); return v;
}
uint32_t safeU32(uintptr_t addr) {
    uint32_t v = 0; safeRead(addr, &v, 4); return v;
}

// Read the player's name from Container::NameData (UTF-32LE wstring data,
// preceded by length at NameData-4). Linux had +0x064; ARM64 has +0x090.
std::string readPlayerName(uintptr_t container) {
    namespace C = OffsetsAndroid::Container;
    uint32_t len = safeU32(container + C::WStringLen);
    if (len == 0 || len > 32) return "";
    return readWideString(container + C::NameData, len);
}

void doScan() {
    bool expected = false;
    if (!s_scanning.compare_exchange_strong(expected, true)) return;

    {
        std::lock_guard<std::mutex> lk(s_mutex);
        s_snap.scanning = true;
    }

    uintptr_t base = ws::memory::findModuleBase("libwarspear.so");
    if (!base) {
        ALOGE("PlayerWatch: libwarspear.so not mapped, abort scan");
        s_scanning.store(false);
        std::lock_guard<std::mutex> lk(s_mutex);
        s_snap.scanning = false;
        return;
    }

    uintptr_t vtable_addr = base + VTABLE_PLAYER_RVA;
    ALOGI("PlayerWatch: scan via vtable 0x%lx (base 0x%lx + rva 0x%lx)",
          (unsigned long)vtable_addr, (unsigned long)base, (unsigned long)VTABLE_PLAYER_RVA);

    auto candidates = findByVtable(vtable_addr, 16);

    PlayerSnapshot newSnap;
    newSnap.scanning = false;
    newSnap.candidates.reserve(candidates.size());

    for (const auto& c : candidates) {
        ScanResult r;
        r.hp_addr     = c.hp_addr;
        r.hp          = c.hp;
        r.hp_max      = c.hp_max;
        r.mp          = c.mp;
        r.mp_max      = c.mp_max;
        r.score       = c.score;
        r.vtable_addr = c.vtable_addr;
        r.vtable_off  = c.vtable_off;
        newSnap.candidates.push_back(r);
        // hp_addr - HP_offset = Container base
        uintptr_t container = c.hp_addr - OffsetsAndroid::Container::HP;
        std::string name = readPlayerName(container);
        ALOGI("  cand container=0x%lx  name=\"%s\"  HP %u/%u  MP %u/%u",
              (unsigned long)container, name.c_str(),
              c.hp, c.hp_max, c.mp, c.mp_max);
    }

    if (!candidates.empty()) {
        const auto& best = candidates.front();
        newSnap.hp_addr        = best.hp_addr;
        newSnap.container_base = best.hp_addr - OffsetsAndroid::Container::HP;
        newSnap.hp             = best.hp;
        newSnap.hp_max         = best.hp_max;
        newSnap.mp             = best.mp;
        newSnap.mp_max         = best.mp_max;
        newSnap.name           = readPlayerName(newSnap.container_base);
        newSnap.level          = 0;  // TODO: probe once we have ARM64 Level offset
        newSnap.selected       = 0;
        newSnap.locked         = true;
    }

    {
        std::lock_guard<std::mutex> lk(s_mutex);
        s_snap = newSnap;
    }
    s_scanning.store(false);
    ALOGI("PlayerWatch: scan complete. locked=%d candidates=%zu",
          newSnap.locked ? 1 : 0, newSnap.candidates.size());
}

} // namespace

void requestScan() {
    if (s_scanning.load()) return;
    {
        std::lock_guard<std::mutex> lk(s_mutex);
        s_snap.locked = false;
        s_snap.scanning = true;
    }
    std::thread([] { doScan(); }).detach();
}

void selectCandidate(int idx) {
    std::lock_guard<std::mutex> lk(s_mutex);
    if (idx < 0 || idx >= (int)s_snap.candidates.size()) return;
    const auto& c = s_snap.candidates[idx];
    s_snap.hp_addr        = c.hp_addr;
    s_snap.container_base = c.hp_addr - OffsetsAndroid::Container::HP;
    s_snap.hp             = c.hp;
    s_snap.hp_max         = c.hp_max;
    s_snap.mp             = c.mp;
    s_snap.mp_max         = c.mp_max;
    s_snap.name           = readPlayerName(s_snap.container_base);
    s_snap.selected       = idx;
    s_snap.locked         = true;
    ALOGI("PlayerWatch: selected #%d  container=0x%lx  name=\"%s\"",
          idx, (unsigned long)s_snap.container_base, s_snap.name.c_str());
}

PlayerSnapshot getSnapshot() {
    std::lock_guard<std::mutex> lk(s_mutex);
    return s_snap;
}

void tick() {
    std::lock_guard<std::mutex> lk(s_mutex);
    if (!s_snap.locked || !s_snap.container_base) return;
    namespace C = OffsetsAndroid::Container;
    uint32_t stats[4] = {0};
    if (safeRead(s_snap.container_base + C::HP, stats, sizeof(stats))) {
        s_snap.hp     = stats[0];
        s_snap.hp_max = stats[1];
        s_snap.mp     = stats[2];
        s_snap.mp_max = stats[3];
    }
    // Refresh name in case it changed (relog / hot-swap)
    auto fresh = readPlayerName(s_snap.container_base);
    if (!fresh.empty()) s_snap.name = fresh;
}

} // namespace ws::game
