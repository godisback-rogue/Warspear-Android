#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <atomic>

namespace ws::game {

// A single heap-scan candidate (one location whose 4 u32s look like a stat
// block). The overlay can display the full list so the user picks the right
// one when auto-selection got it wrong.
struct ScanResult {
    uintptr_t hp_addr      = 0;
    uint32_t  hp           = 0;
    uint32_t  hp_max       = 0;
    uint32_t  mp           = 0;
    uint32_t  mp_max       = 0;
    int       score        = 0;
    uintptr_t vtable_addr  = 0;   // pointer into libwarspear .rodata (vtable)
    intptr_t  vtable_off   = 0;   // negative offset from hp_addr to vtable slot
                                  // (= offset of the Container instance start)
};

// Live snapshot of the player's stats, refreshed each tick from memory.
struct PlayerSnapshot {
    bool          locked    = false;     // true once we've identified a Container
    bool          scanning  = false;     // background scan in progress
    uintptr_t     hp_addr   = 0;
    uintptr_t     container_base = 0;
    uint32_t      hp        = 0;
    uint32_t      hp_max    = 0;
    uint32_t      mp        = 0;
    uint32_t      mp_max    = 0;
    uint32_t      level     = 0;
    std::string   name;
    // All candidates found by the last scan (may be empty if not yet run).
    std::vector<ScanResult> candidates;
    int           selected  = -1;        // index into candidates; -1 = none
};

PlayerSnapshot getSnapshot();

// Kick off an async heap scan. Always re-runs (drops current lock so the
// rescan can re-pick a candidate, useful after entering the game world).
void requestScan();

// Manually select one of the candidates (called when the user clicks an
// item in the candidate list). Locks onto that address.
void selectCandidate(int idx);

// Refresh stats from selected addresses. Cheap. Called each frame.
void tick();

} // namespace ws::game
