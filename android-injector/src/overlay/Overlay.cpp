#include "overlay/Overlay.h"
#include "game/PlayerWatch.h"
#include "logger.h"
#include "memory.h"

#define IMGUI_IMPL_OPENGL_ES2
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <unistd.h>
#include <cstdio>

namespace ws::overlay {

uint64_t g_frameCount = 0;

namespace {
bool  s_imguiReady = false;
int   s_displayW   = 1280;
int   s_displayH   = 720;
int   s_lastScanReq = -120;
float s_fontScale  = 1.0f;   // user-adjustable runtime scale (sliders below)
}

bool init(EGLDisplay dpy, EGLSurface surface, int w, int h) {
    if (s_imguiReady) return true;

    if (w > 0 && h > 0) {
        s_displayW = w;
        s_displayH = h;
    } else {
        EGLint qw = 0, qh = 0;
        eglQuerySurface(dpy, surface, EGL_WIDTH,  &qw);
        eglQuerySurface(dpy, surface, EGL_HEIGHT, &qh);
        if (qw > 0 && qh > 0) {
            s_displayW = qw;
            s_displayH = qh;
        }
    }

    ALOGI("overlay: init  display=%dx%d  pid=%d", s_displayW, s_displayH, getpid());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)s_displayW, (float)s_displayH);
    io.IniFilename = nullptr;
    io.MouseDrawCursor = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    // Load the default font at a larger pixel size for readability on
    // phone/tablet displays. The default 13px is unusable on a 1280x720
    // or higher screen.
    ImFontConfig fontCfg;
    fontCfg.SizePixels = 22.0f;     // ~1.7x the default
    fontCfg.OversampleH = 2;
    fontCfg.OversampleV = 2;
    io.Fonts->AddFontDefault(&fontCfg);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    // Scale paddings/borders to match the bigger font
    style.ScaleAllSizes(2.0f);
    style.WindowRounding = 8.0f;
    style.FrameRounding  = 5.0f;
    style.WindowBorderSize = 0.0f;
    style.Alpha = 0.92f;

    if (!ImGui_ImplOpenGL3_Init("#version 100")) {
        ALOGE("overlay: ImGui_ImplOpenGL3_Init FAILED");
        return false;
    }

    s_imguiReady = true;
    ALOGI("overlay: ImGui initialized");
    return true;
}

void updateDisplaySize(int w, int h) {
    if (w > 0) s_displayW = w;
    if (h > 0) s_displayH = h;
}

static void drawWindow() {
    auto snap = ws::game::getSnapshot();

    // First-time placement; subsequent frames respect drag/resize
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(520, 480), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("WS_SDK", nullptr, ImGuiWindowFlags_NoSavedSettings)) {
        // Apply user font scale on top of the loaded-at-22px font
        ImGui::SetWindowFontScale(s_fontScale);

        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.6f, 1.0f),
                           "ws-android — Warspear Online");
        ImGui::Separator();

        // ─── Player stats ──────────────────────────────────────────────
        if (snap.locked) {
            ImGui::Text("Name:  %s", snap.name.empty() ? "(unknown)" : snap.name.c_str());
            ImGui::Text("Level: %u", snap.level);
            ImGui::Spacing();

            float hpRatio = snap.hp_max ? (float)snap.hp / (float)snap.hp_max : 0.f;
            char hpBuf[64]; snprintf(hpBuf, sizeof(hpBuf), "%u / %u", snap.hp, snap.hp_max);
            ImGui::Text("HP");
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.85f, 0.20f, 0.20f, 1.0f));
            ImGui::ProgressBar(hpRatio, ImVec2(-1, 0), hpBuf);
            ImGui::PopStyleColor();

            float mpRatio = snap.mp_max ? (float)snap.mp / (float)snap.mp_max : 0.f;
            char mpBuf[64]; snprintf(mpBuf, sizeof(mpBuf), "%u / %u", snap.mp, snap.mp_max);
            ImGui::Text("MP");
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.20f, 0.45f, 0.95f, 1.0f));
            ImGui::ProgressBar(mpRatio, ImVec2(-1, 0), mpBuf);
            ImGui::PopStyleColor();

            ImGui::TextDisabled("HP @ 0x%lx", (unsigned long)snap.hp_addr);
            // If the selected candidate has a vtable, show its info.
            if (snap.selected >= 0 && snap.selected < (int)snap.candidates.size()) {
                const auto& sel = snap.candidates[snap.selected];
                if (sel.vtable_addr) {
                    ImGui::TextDisabled("Container start = HP%+ld (vtable 0x%lx)",
                                       (long)sel.vtable_off,
                                       (unsigned long)sel.vtable_addr);
                }
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f),
                              "Player Container not located yet.");
            ImGui::TextWrapped("1. Logue no Warspear e entre com o personagem (no mundo, com HUD visivel).\n2. Clique 'Scan Heap' abaixo.\n3. Toque em um candidato com [vt] cujo HP/MP bata com o que voce ve.");
        }

        ImGui::Spacing();
        ImGui::Separator();

        // ─── Scan controls ─────────────────────────────────────────────
        if (snap.scanning) {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "Scanning heap...");
        } else {
            if (ImGui::Button(snap.locked ? "Rescan" : "Scan Heap", ImVec2(-1, 0))) {
                ws::game::requestScan();
                s_lastScanReq = (int)g_frameCount;
            }
        }

        // Candidate list — let the user pick the right one if auto-select
        // got it wrong (very common on login screen / early-game).
        //
        // Candidates with a nearby vtable (in libwarspear's .rodata) are
        // marked with [vt] — those are the REAL C++ instances. Without
        // [vt], it's almost certainly just a 4-tuple of integers that
        // happened to look like stats. Pick a [vt] candidate whose HP/MP
        // matches what you see on screen.
        if (!snap.candidates.empty()) {
            // Count candidates with vtable — useful indicator
            int withVt = 0;
            for (const auto& c : snap.candidates) if (c.vtable_addr) withVt++;

            ImGui::Spacing();
            ImGui::Text("Candidates: %zu (%d with vtable)",
                       snap.candidates.size(), withVt);
            if (withVt == 0) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.2f, 1.0f),
                                  "No real objects found — are you in-game?");
            } else {
                ImGui::TextDisabled("Tap one whose HP/MP matches in-game.");
            }
            ImGui::BeginChild("CandList", ImVec2(0, 220), true);
            for (size_t i = 0; i < snap.candidates.size(); ++i) {
                const auto& c = snap.candidates[i];
                char label[160];
                snprintf(label, sizeof(label),
                         "%c %s #%zu  HP %u/%u  MP %u/%u  (s=%d)  0x%lx",
                         (int)i == snap.selected ? '>' : ' ',
                         c.vtable_addr ? "[vt]" : "    ",
                         i, c.hp, c.hp_max, c.mp, c.mp_max, c.score,
                         (unsigned long)c.hp_addr);
                bool isSel = ((int)i == snap.selected);
                if (c.vtable_addr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 1.0f, 0.7f, 1.0f));
                }
                if (ImGui::Selectable(label, isSel)) {
                    ws::game::selectCandidate((int)i);
                }
                if (c.vtable_addr) {
                    ImGui::PopStyleColor();
                }
            }
            ImGui::EndChild();
        }

        ImGui::Spacing();
        ImGui::Separator();

        // ─── Settings ──────────────────────────────────────────────────
        if (ImGui::CollapsingHeader("Settings")) {
            ImGui::SliderFloat("Font scale", &s_fontScale, 0.7f, 2.5f, "%.2f");
            ImGui::SameLine();
            if (ImGui::SmallButton("1.0")) s_fontScale = 1.0f;
        }

        ImGui::TextDisabled("PID %d | display %dx%d | frame %lu",
                           getpid(), s_displayW, s_displayH,
                           (unsigned long)g_frameCount);
    }
    ImGui::End();
}

void renderFrame() {
    if (!s_imguiReady) return;

    g_frameCount++;

    // Refresh player stats from cached addresses (cheap when not locked)
    ws::game::tick();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)s_displayW, (float)s_displayH);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    drawWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace ws::overlay
