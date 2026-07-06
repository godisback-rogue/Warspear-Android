// probe_reader — standalone tool to read process memory via /proc/PID/mem.
// Used (with root) to validate offsets against a running Warspear process
// before we commit to repackaging the APK.
//
// Build: included as a separate target in CMakeLists.txt
// Usage on device: ./probe_reader <pid> <hex_addr> <hex_length>
//                  ./probe_reader <pid> chain        # walk GSI→GM→Player

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>

// process_vm_readv is portable but requires CAP_SYS_PTRACE or matching UID.
// As a fallback we open /proc/PID/mem (also needs root or same UID).

static int g_mem_fd = -1;

static bool read_mem(uintptr_t addr, void* buf, size_t len) {
    if (g_mem_fd < 0) return false;
    // pread64 takes off64_t — handles full 64-bit
    ssize_t n = pread64(g_mem_fd, buf, len, (off64_t)addr);
    return n == (ssize_t)len;
}

static uint32_t u32(uintptr_t addr) {
    uint32_t v = 0;
    return read_mem(addr, &v, 4) ? v : 0;
}

static uint64_t u64(uintptr_t addr) {
    uint64_t v = 0;
    return read_mem(addr, &v, 8) ? v : 0;
}

static void dump_hex(uintptr_t addr, size_t len) {
    uint8_t buf[4096];
    if (len > sizeof(buf)) len = sizeof(buf);
    if (!read_mem(addr, buf, len)) {
        fprintf(stderr, "read failed at 0x%lx (errno=%d %s)\n",
                (unsigned long)addr, errno, strerror(errno));
        return;
    }
    for (size_t i = 0; i < len; i += 16) {
        printf("%016lx  ", (unsigned long)(addr + i));
        for (size_t j = 0; j < 16 && i + j < len; ++j) {
            printf("%02x ", buf[i + j]);
            if (j == 7) printf(" ");
        }
        printf(" |");
        for (size_t j = 0; j < 16 && i + j < len; ++j) {
            uint8_t c = buf[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("|\n");
    }
}

// Try to find the libwarspear.so base in /proc/PID/maps
static uintptr_t find_lib_base(int pid, const char* needle) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* f = fopen(path, "r");
    if (!f) return 0;
    char line[1024];
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), f)) {
        if (!strstr(line, needle)) continue;
        // First match — typically the r--p of the library
        unsigned long s = 0;
        if (sscanf(line, "%lx-", &s) == 1) {
            base = s;
            break;
        }
    }
    fclose(f);
    return base;
}

// Walk chain following the Linux SDK layout, just to see if pointers look sane
// even though the offsets will differ for ARM64. We probe a few candidate
// offsets near the Linux ones.
static void try_chain(int pid) {
    uintptr_t base = find_lib_base(pid, "libwarspear.so");
    printf("libwarspear.so base = 0x%lx\n\n", (unsigned long)base);
    if (!base) return;

    // The Linux GSI is at base + 0xDED060. Highly unlikely to be the same
    // address on ARM64 — but the BSS singletons live in the writable
    // anonymous-or-data region near the end of the module. Let's just dump
    // a few candidate offsets and let the human read.

    // We probe by scanning a small range of BSS-like addresses for pointer
    // values that themselves look like valid heap addresses.
    printf("=== quick scan: looking for pointer-like values near base+0xDED060 ===\n");
    uintptr_t test_base = base + 0xDED060;
    for (intptr_t off = -0x2000; off <= 0x2000; off += 8) {
        uintptr_t a = test_base + off;
        uint64_t v = u64(a);
        // Heap addresses on this device are roughly 0x6xxxxxxxxxxx-0x7xxxxxxxxxxx
        if ((v >> 32) > 0x50 && (v >> 32) < 0x80) {
            printf("  0x%lx (=base+0x%lx): -> 0x%lx\n",
                   (unsigned long)a, (unsigned long)(a - base), (unsigned long)v);
        }
    }
}

// Find libwarspear's r--p ranges so we can flag vtable hits.
struct LibRange { uintptr_t lo, hi; };
static LibRange getLibRange(int pid) {
    LibRange r{0,0};
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* f = fopen(path, "r");
    if (!f) return r;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (!strstr(line, "libwarspear")) continue;
        // Match only r--p (.text/.rodata)
        char perms[8] = {};
        unsigned long s=0, e=0;
        sscanf(line, "%lx-%lx %7s", &s, &e, perms);
        if (perms[0]=='r' && perms[1]=='-') {
            if (!r.lo || (uintptr_t)s < r.lo) r.lo = s;
            if ((uintptr_t)e > r.hi) r.hi = e;
        }
    }
    fclose(f);
    return r;
}

// Search the heap (anonymous rw-p) for a u32 value. For each match, dump
// the u32 plus 3 following ones (potential HPMax/MP/MPMax) and check if
// a libwarspear vtable pointer sits within 0x600 bytes before.
static void searchU32(int pid, uint32_t needle) {
    LibRange lib = getLibRange(pid);
    fprintf(stderr, "libwarspear r--p range: 0x%lx..0x%lx\n",
            (unsigned long)lib.lo, (unsigned long)lib.hi);

    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* f = fopen(path, "r");
    if (!f) { perror("maps"); return; }

    int hits = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        unsigned long s=0, e=0;
        char perms[8]={}, path2[768]={};
        int n = sscanf(line, "%lx-%lx %7s %*x %*s %*d %767s", &s, &e, perms, path2);
        if (n < 3) continue;
        if (perms[0]!='r' || perms[1]!='w') continue;
        if (n >= 4 && path2[0]) {
            // Only anonymous regions (heap)
            if (path2[0] != '[') continue;
            if (strstr(path2, "stack") || strstr(path2, "signal") || strstr(path2, "dalvik")) continue;
        }
        size_t size = e - s;
        if (size > 64*1024*1024) continue;

        const size_t CHUNK = 65536;
        uint8_t buf[CHUNK];
        for (size_t off = 0; off < size; off += CHUNK) {
            size_t toRead = (off + CHUNK <= size) ? CHUNK : (size - off);
            if (pread64(g_mem_fd, buf, toRead, (off64_t)(s + off)) != (ssize_t)toRead) continue;
            const uint32_t* u = (const uint32_t*)buf;
            size_t qwords = toRead / 4;
            for (size_t i = 0; i + 3 < qwords; ++i) {
                if (u[i] != needle) continue;
                uintptr_t hp_addr = s + off + i * 4;
                uint32_t a = u[i], b = u[i+1], c = u[i+2], d = u[i+3];

                // Look for a vtable pointer in the 0x600 bytes before hp_addr
                uintptr_t vt = 0;
                intptr_t  vt_off = 0;
                if (lib.lo) {
                    constexpr size_t MAX_BACK = 0x600;
                    uintptr_t back_buf[MAX_BACK/8 + 2];
                    if (pread64(g_mem_fd, back_buf, sizeof(back_buf),
                                (off64_t)(hp_addr - MAX_BACK)) == (ssize_t)sizeof(back_buf)) {
                        for (size_t j = 0; j < sizeof(back_buf)/8; ++j) {
                            uintptr_t v = back_buf[j];
                            if (v >= lib.lo && v < lib.hi) {
                                vt = v;
                                vt_off = (intptr_t)((hp_addr - MAX_BACK + j*8) - hp_addr);
                                break;
                            }
                        }
                    }
                }

                printf("%-3d HP=%u  HPMax=%u  MP=%u  MPMax=%u  @0x%lx",
                       hits, a, b, c, d, (unsigned long)hp_addr);
                if (vt) {
                    printf("  [vt] vt=0x%lx  cnt_start=HP%+ld",
                           (unsigned long)vt, (long)vt_off);
                }
                printf("\n");
                hits++;
                if (hits >= 64) { fclose(f); return; }
            }
        }
    }
    fclose(f);
    fprintf(stderr, "total hits: %d\n", hits);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "  %s <pid> <hex_addr> <hex_length>\n", argv[0]);
        fprintf(stderr, "  %s <pid> chain\n", argv[0]);
        fprintf(stderr, "  %s <pid> base                # print libwarspear.so base\n", argv[0]);
        fprintf(stderr, "  %s <pid> search <decimal>    # find u32 value in heap (e.g. visible HP)\n", argv[0]);
        return 1;
    }
    int pid = atoi(argv[1]);
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    g_mem_fd = open(path, O_RDONLY);
    if (g_mem_fd < 0) {
        fprintf(stderr, "open %s failed: %s (need root)\n", path, strerror(errno));
        return 2;
    }

    if (strcmp(argv[2], "base") == 0) {
        uintptr_t b = find_lib_base(pid, "libwarspear.so");
        printf("0x%lx\n", (unsigned long)b);
    } else if (strcmp(argv[2], "chain") == 0) {
        try_chain(pid);
    } else if (strcmp(argv[2], "search") == 0) {
        if (argc < 4) { fprintf(stderr, "missing value to search\n"); return 1; }
        uint32_t needle = (uint32_t)strtoul(argv[3], nullptr, 0);
        searchU32(pid, needle);
    } else {
        if (argc < 4) {
            fprintf(stderr, "missing <hex_length>\n");
            return 1;
        }
        uintptr_t addr = strtoull(argv[2], nullptr, 16);
        size_t len = strtoull(argv[3], nullptr, 16);
        dump_hex(addr, len);
    }

    close(g_mem_fd);
    return 0;
}
