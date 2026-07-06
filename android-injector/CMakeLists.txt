cmake_minimum_required(VERSION 3.22)
project(ws_injector_android C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/../third_party")

# ─── ImGui (core + GLES2 backend) ────────────────────────────────────────
set(IMGUI_DIR "${THIRD_PARTY}/imgui")
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_OPENGL_ES2
    IMGUI_DISABLE_DEBUG_TOOLS
    IMGUI_DISABLE_DEMO_WINDOWS
)
target_link_libraries(imgui PUBLIC EGL GLESv2)

# ─── injector ──────────────────────────────────────────────────────────
set(INJECTOR_SOURCES
    src/main.cpp
    src/logger.cpp
    src/memory.cpp
    src/overlay/Overlay.cpp
    src/jni/OverlayJni.cpp
    src/game/HeapScan.cpp
    src/game/PlayerWatch.cpp
)

add_library(injector SHARED ${INJECTOR_SOURCES})
target_include_directories(injector PRIVATE src)
target_link_libraries(injector PRIVATE imgui log android EGL GLESv2 dl)
target_compile_options(injector PRIVATE
    -Wall -Wextra -Wno-unused-parameter
    -fvisibility=hidden
    $<$<CONFIG:Release>:-O2>
)
# IMPORTANT: JNI exports must NOT have hidden visibility — explicitly
# set visibility to default for the JNI export TU.
set_source_files_properties(src/jni/OverlayJni.cpp PROPERTIES
    COMPILE_FLAGS "-fvisibility=default")

set_target_properties(injector PROPERTIES
    LINK_FLAGS_RELEASE "-Wl,--strip-all"
)

# ─── probe_reader (standalone tool) ──────────────────────────────────────
add_executable(probe_reader src/probe_reader.cpp)
target_compile_options(probe_reader PRIVATE -Wall -Wextra -O2)
