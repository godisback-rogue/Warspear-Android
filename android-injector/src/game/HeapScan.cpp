#include "game/HeapScan.h"
#include "game/Offsets_android.h"
#include "memory.h"
#include "logger.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <errno.h>

namespace ws::game {

namespace {

// Plausibility filter for (hp, hp_max, mp, mp_max) — based on observed
// Warspear stat ranges (level 1 ~50 HP, level 34 ~6000 HP, max-level cap ~15k).
// MP/energy similar magnitudes (can be 0 for some classes).
constexpr uint32_t HP_MIN     = 10;
constexpr uint32_t HP_MAX_CAP = 100000;
constexpr uint32_t MP_MAX_CAP = 100000;

// Common values that turn out to be false positives:
//   * display dimensions (1280, 720, 1600, 900, 1080, 2340, 2400 etc)
//   * round powers of 2 (256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536)
//   * default int-max-ish (0xFFFF=65535, etc)
bool isLikelyFalsePositive(uint32_t v) {
    static const uint32_t SUSPICIOUS[] = {
        // Display dims
        720, 768, 900, 1080, 1280, 1440, 1600, 1920, 2160, 2340, 2400, 2880, 3200,
        // Round powers
        256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536,
        // Common defaults
        65535, 0xFFFF, 65280, 100, 1000, 10000, 100000,
    };
    for (uint32_t x : SUSPICIOUS) {
        if (v == x) return true;
    }
    return false;
}

bool plausibleStats(uint32_t hp, uint32_t hpMax, uint32_t mp, uint32_t mpMax) {
    if (hp < HP_MIN || hp > HP_MAX_CAP) return false;
    if (hpMax < HP_MIN || hpMax > HP_MAX_CAP) return false;
    if (hp > hpMax) return false;
    if (mp > MP_MAX_CAP) return false;
    if (mpMax > MP_MAX_CAP) return false;
    if (mp > mpMax) return false;
    return true;
}

int scoreCandidate(uint32_t hp, uint32_t hpMax, uint32_t mp, uint32_t mpMax) {
    int s = 0;
    // Reasonable range for a real player
    if (hp >= 100 && hpMax >= 100) s += 3;
    if (hpMax > hp) s += 2;   // not at max health is a good sign
    if (hpMax >= 1000) s += 1;
    if (mpMax > 0)     s += 1;
    if (hp == hpMax) s -= 1;
    // Heavy penalty for values that look like display dimensions or
    // power-of-2 defaults — those are almost certainly false positives.
    if (isLikelyFalsePositive(hp))     s -= 5;
    if (isLikelyFalsePositive(hpMax))  s -= 5;
    if (isLikelyFalsePositive(mp))     s -= 2;
    if (isLikelyFalsePositive(mpMax))  s -= 3;
    return s;
}

// Safely read up to len bytes from `addr` in our own address space into `buf`.
// Uses process_vm_readv so unmapped pages give -1 instead of SIGSEGV (anonymous
// rw-p regions in this process have guard pages mid-region).
static ssize_t safeRead(uintptr_t addr, void* buf, size_t len) {
    struct iovec local  { buf, len };
    struct iovec remote { reinterpret_cast<void*>(addr), len };
    return process_vm_readv(getpid(), &local, 1, &remote, 1, 0);
}

// Scan an arbitrarily-large region by reading it in 4 KB chunks via
// process_vm_readv. Skips chunks that fail to read (guard pages, etc).
// Returns when out has reached maxOut*8 candidates.
void scanRegion(uintptr_t baseAddr, size_t regionSize,
                std::vector<ScanCandidate>& out, size_t maxOut) {
    constexpr size_t CHUNK = 4096;
    uint8_t buf[CHUNK];
    // We allow the last quadruple to span across a chunk boundary by reading
    // CHUNK+16 bytes worth via a sliding overlap. Simpler approach: just read
    // CHUNK and process up to len-16 within each chunk (skip the last 4 u32s).
    for (size_t off = 0; off + 16 <= regionSize; off += CHUNK - 16) {
        size_t toRead = (off + CHUNK <= regionSize) ? CHUNK : (regionSize - off);
        if (toRead < 16) break;
        ssize_t n = safeRead(baseAddr + off, buf, toRead);
        if (n <= 0) continue; // guard page or otherwise unreadable — skip
        const uint32_t* u = reinterpret_cast<const uint32_t*>(buf);
        size_t qwords = static_cast<size_t>(n) / 4;
        if (qwords < 4) continue;
        for (size_t i = 0; i + 3 < qwords; ++i) {
            uint32_t a = u[i];
            uint32_t b = u[i + 1];
            uint32_t c = u[i + 2];
            uint32_t d = u[i + 3];
            if (!plausibleStats(a, b, c, d)) continue;
            ScanCandidate cand;
            cand.hp_addr = baseAddr + off + i * 4;
            cand.hp = a; cand.hp_max = b; cand.mp = c; cand.mp_max = d;
            cand.score = scoreCandidate(a, b, c, d);
            out.push_back(cand);
            if (out.size() >= maxOut * 8) {
                std::partial_sort(out.begin(),
                                  out.begin() + maxOut,
                                  out.end(),
                                  [](const ScanCandidate& x, const ScanCandidate& y) {
                                      return x.score > y.score;
                                  });
                out.resize(maxOut);
            }
        }
    }
}

} // namespace

// Locate libwarspear.so's read-only ranges (.text + .rodata). C++ vtables
// for instances allocated on the heap live here; the first u64 of every
// real object instance is a pointer into this range.
struct LibRanges {
    uintptr_t lo = 0;
    uintptr_t hi = 0;
};
static LibRanges getLibwarspearRanges() {
    LibRanges r;
    auto regions = ws::memory::readMaps();
    for (const auto& mr : regions) {
        if (mr.pathname.find("libwarspear") == std::string::npos) continue;
        // Read-only segments only (.text/.rodata, NOT .data/.bss which are rw-p)
        if (mr.perms[0] != 'r' || mr.perms[1] != '-') continue;
        if (r.lo == 0 || mr.start < r.lo) r.lo = mr.start;
        if (mr.end > r.hi) r.hi = mr.end;
    }
    return r;
}

// For a candidate at hp_addr, sweep backwards looking for a pointer into
// the libwarspear .rodata range — that's the classic C++ vtable signature
// of "the start of an instance" of some class. Returns (offset, vtable_addr)
// on success, (0, 0) if no vtable found in the search window.
//
// Why 0x600 backward window?  Linux Container::HP = 0x134. ARM64 with 8-byte
// pointers and bigger string objects roughly doubles that → cap at 0x600.
static std::pair<intptr_t, uintptr_t>
findVtableNear(uintptr_t hp_addr, const LibRanges& lib) {
    if (lib.lo == 0) return {0, 0};
    constexpr intptr_t MAX_BACK = 0x600;
    // Scan 8-byte aligned backwards. Read in one big chunk for speed.
    uintptr_t start = hp_addr - MAX_BACK;
    uintptr_t buf[(MAX_BACK + 16) / 8];
    if (safeRead(start, buf, sizeof(buf)) <= 0) return {0, 0};
    // Walk from earliest to latest — the start of the object is the FIRST
    // vtable we find (lower addresses come first).
    size_t nWords = sizeof(buf) / 8;
    for (size_t i = 0; i < nWords; ++i) {
        uintptr_t v = buf[i];
        if (v >= lib.lo && v < lib.hi) {
            uintptr_t slot = start + i * 8;
            intptr_t off = (intptr_t)(slot - hp_addr);  // negative
            return {off, v};
        }
    }
    return {0, 0};
}

std::vector<ScanCandidate> findPlayerStatBlocks(size_t maxCandidates) {
    auto regions = ws::memory::readMaps();
    std::vector<ScanCandidate> hits;
    hits.reserve(maxCandidates * 8);
    LibRanges lib = getLibwarspearRanges();
    if (lib.lo == 0) {
        ALOGW("heap-scan: libwarspear.so .rodata range not found — vtable check disabled");
    } else {
        ALOGI("heap-scan: libwarspear ro range = 0x%lx..0x%lx",
              (unsigned long)lib.lo, (unsigned long)lib.hi);
    }

    size_t scannedBytes = 0;
    for (const auto& r : regions) {
        // Only writable, anonymous, in-process heap. Skip mapped files, libs,
        // /dev/, etc. Anonymous rw-p is where game allocates its heap objects.
        if (r.perms[0] != 'r' || r.perms[1] != 'w') continue;
        if (!r.pathname.empty()) {
            // Allow regions named "[anon:...]" but skip file-backed
            if (r.pathname[0] != '[') continue;
            // Skip stack / signal regions
            if (r.pathname.find("stack") != std::string::npos) continue;
            if (r.pathname.find("signal") != std::string::npos) continue;
            // Skip dalvik (it's huge but unlikely to hold our struct)
            if (r.pathname.find("dalvik") != std::string::npos) continue;
        }
        size_t size = r.end - r.start;
        if (size > 64 * 1024 * 1024) continue;  // skip absurdly large regions
        scanRegion(r.start, size, hits, maxCandidates);
        scannedBytes += size;
    }

    // ─── Vtable verification pass ─────────────────────────────────────
    // Every real C++ instance allocated on the heap has a vtable pointer
    // as its first 8 bytes. We sweep backwards from each candidate's hp_addr
    // looking for any 8-byte slot whose value points into libwarspear's
    // .rodata. Candidates WITHOUT a nearby vtable are almost certainly
    // just a 4-tuple of integers that happened to look like stats — they
    // get a massive score penalty. Candidates WITH a vtable get a big bonus.
    if (lib.lo != 0) {
        for (auto& c : hits) {
            auto [vt_off, vt_addr] = findVtableNear(c.hp_addr, lib);
            c.vtable_off  = vt_off;
            c.vtable_addr = vt_addr;
            if (vt_addr != 0) {
                c.score += 10;  // strong positive signal
            } else {
                c.score -= 8;   // probably a false positive
            }
        }
    }

    std::sort(hits.begin(), hits.end(),
              [](const ScanCandidate& x, const ScanCandidate& y) {
                  return x.score > y.score;
              });
    // Deduplicate adjacent matches (within 16 bytes)
    if (hits.size() > 1) {
        std::vector<ScanCandidate> dedup;
        dedup.push_back(hits[0]);
        for (size_t i = 1; i < hits.size(); ++i) {
            bool isDup = false;
            for (const auto& d : dedup) {
                if (hits[i].hp_addr > d.hp_addr - 16 &&
                    hits[i].hp_addr < d.hp_addr + 16) {
                    isDup = true; break;
                }
            }
            if (!isDup) dedup.push_back(hits[i]);
            if (dedup.size() >= maxCandidates) break;
        }
        hits = std::move(dedup);
    }
    if (hits.size() > maxCandidates) hits.resize(maxCandidates);
    ALOGI("heap-scan: scanned %zu MB, %zu candidates found",
          scannedBytes / (1024 * 1024), hits.size());
    return hits;
}

std::vector<ScanCandidate> findByExactHp(uint32_t needle, size_t maxCandidates) {
    auto regions = ws::memory::readMaps();
    std::vector<ScanCandidate> hits;
    LibRanges lib = getLibwarspearRanges();

    size_t scannedBytes = 0;
    for (const auto& r : regions) {
        if (r.perms[0] != 'r' || r.perms[1] != 'w') continue;
        if (!r.pathname.empty()) {
            if (r.pathname[0] != '[') continue;
            if (r.pathname.find("stack") != std::string::npos) continue;
            if (r.pathname.find("signal") != std::string::npos) continue;
            if (r.pathname.find("dalvik") != std::string::npos) continue;
        }
        size_t size = r.end - r.start;
        if (size > 64 * 1024 * 1024) continue;
        scannedBytes += size;

        // Read in 4 KB chunks, scan u32-aligned for exact value
        constexpr size_t CHUNK = 4096;
        uint8_t buf[CHUNK];
        for (size_t off = 0; off + 16 <= size; off += CHUNK - 16) {
            size_t toRead = (off + CHUNK <= size) ? CHUNK : (size - off);
            if (toRead < 16) break;
            ssize_t n = safeRead(r.start + off, buf, toRead);
            if (n <= 0) continue;
            const uint32_t* u = reinterpret_cast<const uint32_t*>(buf);
            size_t qwords = static_cast<size_t>(n) / 4;
            for (size_t i = 0; i + 3 < qwords; ++i) {
                if (u[i] != needle) continue;
                ScanCandidate c;
                c.hp_addr = r.start + off + i * 4;
                c.hp     = u[i];
                c.hp_max = u[i + 1];
                c.mp     = u[i + 2];
                c.mp_max = u[i + 3];
                c.score  = 0;
                // Vtable check
                if (lib.lo != 0) {
                    auto [vt_off, vt_addr] = findVtableNear(c.hp_addr, lib);
                    c.vtable_off  = vt_off;
                    c.vtable_addr = vt_addr;
                    if (vt_addr) c.score += 10;
                }
                // Bonus if the 3 following u32s also pass plausibility
                if (plausibleStats(c.hp, c.hp_max, c.mp, c.mp_max)) c.score += 5;
                hits.push_back(c);
                if (hits.size() >= maxCandidates * 4) goto sort_done;
            }
        }
    }
sort_done:
    std::sort(hits.begin(), hits.end(),
              [](const ScanCandidate& a, const ScanCandidate& b) { return a.score > b.score; });
    if (hits.size() > maxCandidates) hits.resize(maxCandidates);
    ALOGI("exact-hp-scan: needle=%u  scanned %zu MB  found %zu hits",
          needle, scannedBytes / (1024 * 1024), hits.size());
    return hits;
}

std::vector<ScanCandidate> findByVtable(uintptr_t vtable_addr, size_t maxCandidates) {
    auto regions = ws::memory::readMaps();
    std::vector<ScanCandidate> hits;
    if (vtable_addr == 0) return hits;

    namespace C = OffsetsAndroid::Container;
    size_t scanned_mb = 0;

    for (const auto& r : regions) {
        if (r.perms[0] != 'r' || r.perms[1] != 'w') continue;
        if (!r.pathname.empty()) {
            if (r.pathname[0] != '[') continue;
            if (r.pathname.find("stack") != std::string::npos) continue;
            if (r.pathname.find("signal") != std::string::npos) continue;
            if (r.pathname.find("dalvik") != std::string::npos) continue;
        }
        size_t size = r.end - r.start;
        if (size > 64 * 1024 * 1024) continue;
        scanned_mb += size / (1024 * 1024);

        // Read 8-byte aligned chunks (Container's vtable is at +0, naturally
        // aligned). Compare each u64 to the target vtable address.
        constexpr size_t CHUNK = 65536;
        uint8_t buf[CHUNK];
        for (size_t off = 0; off < size; off += CHUNK) {
            size_t toRead = (off + CHUNK <= size) ? CHUNK : (size - off);
            if (toRead < sizeof(uintptr_t)) break;
            ssize_t n = safeRead(r.start + off, buf, toRead);
            if (n <= 0) continue;
            const uintptr_t* u = reinterpret_cast<const uintptr_t*>(buf);
            size_t qwords = static_cast<size_t>(n) / sizeof(uintptr_t);
            for (size_t i = 0; i < qwords; ++i) {
                if (u[i] != vtable_addr) continue;
                uintptr_t container = r.start + off + i * sizeof(uintptr_t);

                // Read stats at known offsets
                uint32_t hp[4] = {0};
                if (!safeRead(container + C::HP, hp, sizeof(hp))) continue;
                // Plausibility check — drop obviously empty/invalid matches
                if (hp[0] == 0 && hp[1] == 0 && hp[2] == 0 && hp[3] == 0) continue;
                if (hp[1] > 200000) continue;        // HPMax too large

                ScanCandidate c;
                c.hp_addr     = container + C::HP;
                c.hp          = hp[0];
                c.hp_max      = hp[1];
                c.mp          = hp[2];
                c.mp_max      = hp[3];
                c.vtable_addr = vtable_addr;
                c.vtable_off  = -(intptr_t)C::HP;
                // Score: prefer non-empty health pools
                c.score = 20 + (hp[1] > 0 ? 5 : 0) + (hp[3] > 0 ? 3 : 0);
                hits.push_back(c);
                if (hits.size() >= maxCandidates * 2) goto done;
            }
        }
    }
done:
    std::sort(hits.begin(), hits.end(),
              [](const ScanCandidate& a, const ScanCandidate& b) { return a.score > b.score; });
    if (hits.size() > maxCandidates) hits.resize(maxCandidates);
    ALOGI("vtable-scan: vt=0x%lx  scanned %zu MB  found %zu Container instances",
          (unsigned long)vtable_addr, scanned_mb, hits.size());
    return hits;
}

std::string readWideString(uintptr_t addr, size_t maxChars) {
    std::string out;
    out.reserve(maxChars);
    // Read up to maxChars codepoints safely
    constexpr size_t MAX_CHARS_CAP = 64;
    if (maxChars > MAX_CHARS_CAP) maxChars = MAX_CHARS_CAP;
    uint32_t wp[MAX_CHARS_CAP] = {0};
    ssize_t got = safeRead(addr, wp, maxChars * sizeof(uint32_t));
    if (got <= 0) return out;
    size_t haveChars = static_cast<size_t>(got) / sizeof(uint32_t);
    if (haveChars < maxChars) maxChars = haveChars;
    for (size_t i = 0; i < maxChars; ++i) {
        uint32_t cp = wp[i];
        if (cp == 0) break;
        // Basic UTF-8 encoding for BMP chars (< 0x10000)
        if (cp < 0x80) {
            out.push_back(static_cast<char>(cp));
        } else if (cp < 0x800) {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        // Cap output length to avoid runaway
        if (out.size() >= maxChars * 3) break;
    }
    return out;
}

} // namespace ws::game
