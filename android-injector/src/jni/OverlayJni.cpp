// JNI entry points called from Java side after smali patches:
//   Java_com_wsAndroid_Overlay_draw      — invoked from MDRenderer.onDrawFrame
//   Java_com_wsAndroid_Overlay_onTouch   — invoked from MDSurfaceView.onTouchEvent
//
// onTouch returns true to "consume" the event (i.e. our overlay grabbed it
// because the touch fell inside an ImGui window); returns false otherwise so
// the game receives the touch normally.

#include "overlay/Overlay.h"
#include "game/PlayerWatch.h"
#include "logger.h"

#include "imgui.h"

#include <jni.h>
#include <EGL/egl.h>
#include <atomic>
#include <android/input.h>

namespace {

std::atomic<bool> s_inited{false};
std::atomic<int>  s_frameCount{0};

// Android MotionEvent action constants (from android/input.h)
constexpr int MASK_ACTION   = 0xFF;
constexpr int ACTION_DOWN   = 0;
constexpr int ACTION_UP     = 1;
constexpr int ACTION_MOVE   = 2;
constexpr int ACTION_CANCEL = 3;

// Cached JNI method IDs for MotionEvent
struct MotionEventIds {
    jclass     cls          = nullptr;
    jmethodID  getAction    = nullptr;
    jmethodID  getX         = nullptr;
    jmethodID  getY         = nullptr;
    bool       ready        = false;
};
MotionEventIds s_me;

bool initMotionEventIds(JNIEnv* env) {
    if (s_me.ready) return true;
    jclass local = env->FindClass("android/view/MotionEvent");
    if (!local) { env->ExceptionClear(); return false; }
    s_me.cls = static_cast<jclass>(env->NewGlobalRef(local));
    env->DeleteLocalRef(local);
    s_me.getAction = env->GetMethodID(s_me.cls, "getAction", "()I");
    s_me.getX      = env->GetMethodID(s_me.cls, "getX", "()F");
    s_me.getY      = env->GetMethodID(s_me.cls, "getY", "()F");
    if (!s_me.getAction || !s_me.getX || !s_me.getY) {
        env->ExceptionClear();
        return false;
    }
    s_me.ready = true;
    ALOGI("OverlayJni: MotionEvent JNI ids cached");
    return true;
}

bool ensureInited() {
    if (s_inited.load(std::memory_order_acquire)) return true;
    EGLDisplay dpy = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    if (dpy == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) return false;
    bool ok = ws::overlay::init(dpy, surface, 0, 0);
    if (ok) {
        s_inited.store(true, std::memory_order_release);
        // No auto-scan — user clicks "Scan" once in-game so we don't
        // lock onto login-screen false positives.
    }
    return ok;
}

} // namespace

extern "C" {

JNIEXPORT void JNICALL
Java_com_wsAndroid_Overlay_draw(JNIEnv* /*env*/, jclass /*cls*/) {
    if (!ensureInited()) return;
    ws::overlay::renderFrame();

    int n = s_frameCount.fetch_add(1, std::memory_order_relaxed) + 1;
    if (n == 1 || n == 60 || n % 600 == 0) {
        ALOGI("overlay: draw() call #%d", n);
    }
}

JNIEXPORT jboolean JNICALL
Java_com_wsAndroid_Overlay_onTouch(JNIEnv* env, jclass /*cls*/, jobject motionEvent) {
    if (!s_inited.load(std::memory_order_acquire)) return JNI_FALSE;
    if (!motionEvent) return JNI_FALSE;
    if (!initMotionEventIds(env)) return JNI_FALSE;

    jint   action = env->CallIntMethod(motionEvent, s_me.getAction);
    jfloat x      = env->CallFloatMethod(motionEvent, s_me.getX);
    jfloat y      = env->CallFloatMethod(motionEvent, s_me.getY);
    if (env->ExceptionCheck()) { env->ExceptionClear(); return JNI_FALSE; }

    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);

    switch (action & MASK_ACTION) {
        case ACTION_DOWN:
            io.AddMouseButtonEvent(0, true);
            break;
        case ACTION_UP:
        case ACTION_CANCEL:
            io.AddMouseButtonEvent(0, false);
            break;
        case ACTION_MOVE:
            // position update only
            break;
        default:
            break;
    }

    // If ImGui wants the mouse (cursor over our window etc.), consume the
    // touch so the game doesn't react to it. Otherwise let the game see it.
    return io.WantCaptureMouse ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"
