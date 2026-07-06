#include "memory.h"
#include "logger.h"

#include <cstdio>
#include <cstring>

namespace ws::memory {

std::vector<MapRegion> readMaps() {
    std::vector<MapRegion> out;
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) {
        ALOGE("readMaps: open /proc/self/maps failed");
        return out;
    }
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        MapRegion r;
        unsigned long start = 0, end = 0;
        char perms[8] = {};
        char path[768] = {};
        // Typical line:
        // 70a1b000-70a1c000 r-xp 00000000 fe:01 1234567   /data/app/.../lib/libwarspear.so
        int n = sscanf(line, "%lx-%lx %7s %*x %*s %*d %767s",
                       &start, &end, perms, path);
        if (n < 3) continue;
        r.start = static_cast<uintptr_t>(start);
        r.end   = static_cast<uintptr_t>(end);
        std::strncpy(r.perms, perms, 4);
        r.perms[4] = 0;
        if (n >= 4) r.pathname = path;
        out.push_back(std::move(r));
    }
    fclose(f);
    return out;
}

uintptr_t findModuleBase(const std::string& needle) {
    auto regions = readMaps();
    for (const auto& r : regions) {
        if (r.pathname.find(needle) == std::string::npos) continue;
        // First executable mapping for the module is the canonical base
        if (r.perms[0] == 'r' && r.perms[2] == 'x') {
            return r.start;
        }
    }
    // Fallback: any mapping that matches
    for (const auto& r : regions) {
        if (r.pathname.find(needle) != std::string::npos) {
            return r.start;
        }
    }
    return 0;
}

} // namespace ws::memory
