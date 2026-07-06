#include "logger.h"
#include "memory.h"

#include <jni.h>
#include <pthread.h>
#include <unistd.h>

extern "C" {

// Called by Android linker when the .so is loaded via System.loadLibrary
// from the patched MDActivity.<clinit>. This runs BEFORE libwarspear.so
// loads, so the game's own state isn't up yet — we keep this fast.
//
// The per-frame overlay drawing is invoked from Java via the Overlay.draw()
// JNI export (see src/jni/OverlayJni.cpp) after smali patches in MDRenderer
// call it.
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* /*vm*/, void* /*reserved*/) {
    ALOGI("=== ws-android injector loaded (pid=%d) ===", getpid());

    // Background thread: wait for libwarspear.so to be in our address space,
    // then log its base for future memory work. No hooks installed — overlay
    // is driven entirely from Java/smali.
    pthread_t tid;
    pthread_create(&tid, nullptr, [](void*) -> void* {
        sleep(2);
        auto base = ws::memory::findModuleBase("libwarspear");
        if (base == 0) {
            sleep(5);
            base = ws::memory::findModuleBase("libwarspear");
        }
        if (base != 0) {
            ALOGI("init: libwarspear.so base = 0x%lx", (unsigned long)base);
        } else {
            ALOGW("init: libwarspear.so not found in /proc/self/maps after retry");
        }
        return nullptr;
    }, nullptr);
    pthread_detach(tid);

    return JNI_VERSION_1_6;
}

} // extern "C"
