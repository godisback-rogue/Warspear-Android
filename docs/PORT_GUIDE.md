# PORT_GUIDE — visão consolidada do port

Resumo executivo do estado do port + roadmap. Pra setup detalhado, ver [SETUP.md](SETUP.md). Pra arquitetura, ver [ARCHITECTURE.md](ARCHITECTURE.md).

## Diff Linux SDK → Android

| Camada | Linux (ws-hack2) | Android (ws-android) |
|---|---|---|
| **Arch** | x86 32-bit | arm64-v8a (primary) + armeabi-v7a |
| **Injeção** | `LD_PRELOAD` | APK repack + `System.loadLibrary` no smali |
| **Render hook** | `glXSwapBuffers` PLT hook | Smali patch em `MDRenderer.onDrawFrame` |
| **Input hook** | `XNextEvent` X11 | Smali patch em `MDSurfaceView.onTouchEvent` |
| **Overlay** | ImGui + OpenGL 1.x | ImGui + OpenGL ES 2.0 |
| **Logger** | stdout/stderr / `Log::info` | `__android_log_print` → adb logcat |
| **Mem reader (RPC)** | `/workspace/ws_mem.sock` | (pending M4) `/data/local/tmp/ws_mem.sock` + `adb forward` |
| **Packet hooks** | xhook em `send`/`recv` | (pending M5) smali patch na camada de rede Java |
| **Debug** | GDB local + Ghidra | gdbserver via adb + Ghidra (libwarspear arm64) |

## O que migra direto vs. precisa re-RE

| Coisa | Migra? |
|---|---|
| Lógica platform-agnostic (combat scripts, A*, ScriptRegistry, etc) | ✓ direto |
| Packet structures e opcodes | ✓ direto (mesmo wire protocol) |
| Class names internas (`Container`, `GameManager`, `Svp::*`, `EffectsManager`...) | ✓ direto (mesmo MobileDragon framework) |
| Struct field offsets (Container::HP, etc) | ✗ refazer (8-byte ptrs em ARM64 vs 4-byte x86) |
| Function RVAs em libwarspear | ✗ refazer (ABI diferente) |
| VTable addresses | ✗ refazer (mas layout estável) |

Estratégia pra offsets: heap scan via tupla semântica (HP/HPMax/MP/MPMax) — ver [ARCHITECTURE.md#heap-scan](ARCHITECTURE.md#heap-scan-player-container-localização).

## Milestones

| ID | Conquista | Commit |
|---|---|---|
| **M0** | Setup completo: LDPlayer rooted + cpuinfo masked + ADB conectado | (preparação) |
| **M1** | `libinjector.so` carregando dentro do processo + log em `adb logcat -s WS_SDK` | `7f45a8e` |
| **M2** | ImGui overlay renderizando no jogo (sobre o framebuffer GLES2) | `c142dd3` |
| **M3** | Live player stats (Name/Level/HP/MP) + janela arrastável/minimizável + touch routing | `70fa84f` |
| **M4** | Socket server pra read/write/scan memory + send_packet | (pending) |
| **M5** | Packet hooks (interceptação de send/recv da network) | (pending) |
| **M6** | Port das combat scripts e ESP | (pending) |

Detalhes de cada milestone em [MILESTONES.md](MILESTONES.md).

## Pipeline de dev

```
android-injector/src/                ← código C++ (overlay, JNI, heap scan)
        │
        │ cmake (NDK toolchain)
        ▼
build/<abi>/libinjector.so          ← shared lib pronta
        │
        │ apply.sh copia → apk/extracted/lib/<abi>/
        ▼
apk/extracted/                       ← APK descomprimido pelo apktool
  ├── smali/com/wsAndroid/Overlay.smali     ← classe adicionada
  ├── smali/com/aigrind/warspear/
  │      MDActivity.smali, MDRenderer.smali, MDSurfaceView.smali  ← patched
  └── lib/{arm64-v8a,armeabi-v7a}/libinjector.so
        │
        │ apktool b + uber-apk-signer
        ▼
apk/repacked/signed/*-debugSigned.apk  ← APK final
        │
        │ adb install
        ▼
device → app rodando com nosso overlay
```

## Compatibilidade

| Ambiente | Status | Notas |
|---|---|---|
| LDPlayer 9 (Android 9, x86_64+Houdini) | ✓ testado | Precisa root + cpuinfo mask. **Não suporta inline hooks** (Houdini cacheia tradução) |
| Bluestacks 5 | ? não testado | Devia funcionar igual (mesmo Houdini stack) |
| MEmu / NoxPlayer / MuMu | ? não testado | Idem |
| Device ARM físico real | ✓ esperado | Smali patches funcionam idêntico. Inline hooks adicionais possíveis. |
| AVD com imagem `arm64-v8a` nativa | ? não testado | Devia ser equivalente a device real, mas brutalmente lento sem KVM |
| AVD com `x86_64` + ARM translation | = LDPlayer | Mesmo problema do Houdini |

## Limitações conhecidas

- **Inline hooks impossíveis em emuladores x86**: detalhes em [HOOKING_SAGA.md](HOOKING_SAGA.md). Tudo nosso passa por smali patches.
- **Anti-cheat detection**: usamos APK auto-assinado (debug keystore) — não passa por servidores que validem signature. Pra produção real (não dev), precisaria de signature válida.
- **Cpuinfo mask precisa root**: em devices físicos não é problema; em emulators é toggle.
- **Performance da heap scan**: ~1-2s pra varrer 300MB de heap. Rodado em background — não bloqueia UI, mas pode causar stutter no boot.
- **Memory snapshot inconsistente**: durante scan, valores podem mudar (jogo é mutável). PlayerWatch lê de novo cada frame, então não tem TOCTOU real, mas o "lock" pode pegar valores intermediários — fica visível e o usuário pode re-scanar.

## Onde está o código

Ponto de entrada: [android-injector/src/main.cpp](../android-injector/src/main.cpp) — `JNI_OnLoad`.

A partir daí, fluxo de chamadas:
- Per-frame: `Java_com_wsAndroid_Overlay_draw` (em [OverlayJni.cpp](../android-injector/src/jni/OverlayJni.cpp)) → `ws::overlay::renderFrame` (em [Overlay.cpp](../android-injector/src/overlay/Overlay.cpp)) → `ws::game::tick` (em [PlayerWatch.cpp](../android-injector/src/game/PlayerWatch.cpp))
- Per-touch: `Java_com_wsAndroid_Overlay_onTouch` (em OverlayJni.cpp) → ImGui IO events.
- Heap scan: `ws::game::requestScan` → thread → `ws::game::findPlayerStatBlocks` (em [HeapScan.cpp](../android-injector/src/game/HeapScan.cpp)).

## Continuando o trabalho

Pra adicionar uma feature nova:
1. Editar/adicionar arquivos em `android-injector/src/`
2. `( cd build/arm64-v8a && cmake --build . -j )`
3. `apk/repack.sh` (re-aplica patches e re-builda APK)
4. `tools/install_apk.sh apk/repacked/signed/*-debugSigned.apk`
5. `adb shell am force-stop com.aigrind.warspear; adb shell am start -n com.aigrind.warspear/.LoaderActivity`
6. `adb logcat -s WS_SDK`

Ciclo: ~30s. Iteração rápida.

## Próximas leituras

- [ARCHITECTURE.md](ARCHITECTURE.md) — design de sistema completo
- [SMALI_PATCHES.md](SMALI_PATCHES.md) — cada patch byte-a-byte
- [HOOKING_SAGA.md](HOOKING_SAGA.md) — por que inline hook não funciona
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) — soluções pra problemas comuns
- [MILESTONES.md](MILESTONES.md) — log cronológico
