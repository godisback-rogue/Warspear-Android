#pragma once

#include <EGL/egl.h>
#include <cstdint>

namespace ws::overlay {

// Initialize ImGui + GLES2 backend. Must be called from within a thread that
// has a current EGL context (i.e. from inside onDrawFrame's first run).
// Idempotent — repeated calls return true without re-initializing.
//
// w/h are the surface size from onSurfaceChanged. Pass 0/0 to auto-query
// via eglQuerySurface.
bool init(EGLDisplay dpy, EGLSurface surface, int w = 0, int h = 0);

// Push a new surface size (called from onSurfaceChanged hook).
void updateDisplaySize(int w, int h);

// Called once per frame, after the game has drawn into the current surface.
// Renders ImGui directly into the framebuffer. The subsequent eglSwapBuffers
// (driven from Java GLSurfaceView) flushes everything to screen.
void renderFrame();

// Counters / state available for the window contents
extern uint64_t g_frameCount;

} // namespace ws::overlay
