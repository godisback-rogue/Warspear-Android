#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace ws::memory {

struct MapRegion {
    uintptr_t start = 0;
    uintptr_t end   = 0;
    char      perms[5] = {};   // e.g. "r-xp", "rw-p"
    std::string pathname;      // backing file, "[anon:libc_malloc]", etc.
};

// Parse /proc/self/maps. Returns all readable regions of the calling process.
std::vector<MapRegion> readMaps();

// Find the lowest-vaddr executable region of a module matching `needle`
// in its pathname (substring match). Returns 0 if not found.
//
// `needle` examples: "libwarspear.so", "warspear-online" (Linux). For
// Android, pass the .so name; e.g. "libwarspear.so".
uintptr_t findModuleBase(const std::string& needle);

// Helpers — reading memory is always direct, this is a single process.
// All reads assume the address is mapped + readable; caller checks.

template <typename T>
inline T readT(uintptr_t addr) {
    return *reinterpret_cast<const T*>(addr);
}

inline uint32_t u32(uintptr_t addr) { return readT<uint32_t>(addr); }
inline uint16_t u16(uintptr_t addr) { return readT<uint16_t>(addr); }
inline uint8_t  u8 (uintptr_t addr) { return readT<uint8_t >(addr); }
inline float    f32(uintptr_t addr) { return readT<float   >(addr); }

} // namespace ws::memory
