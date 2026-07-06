#pragma once
#include <cstdint>
#include <vector>

namespace ws::game {

struct ScanCandidate {
    uintptr_t hp_addr   = 0;   // address of HP u32 in heap
    uint32_t  hp        = 0;
    uint32_t  hp_max    = 0;
    uint32_t  mp        = 0;
    uint32_t  mp_max    = 0;
    int       score     = 0;   // higher = more likely to be the player's Container
    // If this candidate looks like a real C++ object (a vtable pointer was
    // found pointing into libwarspear's .rodata at offset hp_addr-vtable_off),
    // these are set. Otherwise vtable_addr == 0.
    uintptr_t vtable_addr = 0;
    intptr_t  vtable_off  = 0;  // offset of vtable_addr relative to hp_addr (negative)
};

// Scan all anonymous rw-p regions of our own process for a plausible
// (HP, HPMax, MP, MPMax) u32 tuple — matching Container's stat block layout
// from the Linux SDK (HP/HPMax/MP/MPMax adjacent u32s with HP<=HPMax, MP<=MPMax).
// Returns up to `maxCandidates` best matches, sorted by score.
std::vector<ScanCandidate> findPlayerStatBlocks(size_t maxCandidates = 16);

// Pinpoint scan: find every 4-byte-aligned u32 in the heap whose value
// matches `hp` exactly. For each match, also reads the 3 u32s after it
// (potential HPMax, MP, MPMax) and the libwarspear-rodata vtable nearby.
// Used when the user knows the in-game HP value and wants to find the
// exact Container — much more precise than heuristic scoring.
std::vector<ScanCandidate> findByExactHp(uint32_t hp, size_t maxCandidates = 32);

// Find all heap objects whose first 8 bytes (vtable slot) equal `vtable_addr`.
// vtable_addr is typically computed as libwarspear_base + vtable_rva.
// For each match, hp_addr/hp/hp_max/mp/mp_max are populated using the
// Container offsets from Offsets_android.h (HP at +0x1A4 on ARM64).
// This is the FAST + PRECISE way to locate Container instances — much
// better than heuristic stat-tuple scanning.
std::vector<ScanCandidate> findByVtable(uintptr_t vtable_addr, size_t maxCandidates = 32);

// Read a possibly-truncated UTF-32LE wide string at a given address. Returns
// up to maxChars characters, stopping at the first nul codepoint.
// Used for player name display once we've identified its offset.
std::string readWideString(uintptr_t addr, size_t maxChars = 32);

} // namespace ws::game
