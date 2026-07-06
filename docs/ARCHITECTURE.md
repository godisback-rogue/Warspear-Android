# ARCHITECTURE

Visão de sistema do ws-android. Foca em **como** os componentes conversam (Java ↔ JNI ↔ libinjector ↔ libwarspear) e **por que** as decisões foram tomadas (especialmente a escolha de smali patches sobre inline hooks).

## Vista de 10000 pés

```
                  ┌──────────────────────────────────────────────────┐
                  │              com.aigrind.warspear (APK)          │
                  │              (modificado por nós)                │
                  │                                                  │
   adb logcat ◄───┤  ┌────────────────┐                              │
   "WS_SDK"       │  │ MDActivity     │  static {                    │
                  │  │   <clinit>     │    System.loadLibrary(       │
                  │  └────────┬───────┘      "injector"  ◄── injected│
                  │           │              );                      │
                  │           │              System.loadLibrary(     │
                  │           │                "warspear");          │
                  │           │            }                         │
                  │           ▼                                      │
                  │  ┌────────────────┐    ┌─────────────────────┐   │
                  │  │ MDRenderer     │───►│ Native.onDrawFrame  │   │
                  │  │  onDrawFrame() │    │  → libwarspear      │   │
                  │  │  - Native.onDrawFrame                     │   │
                  │  │  - Overlay.draw  ◄── injected             │   │
                  │  └────────────────┘    └─────────────────────┘   │
                  │           │                                      │
                  │  ┌────────────────┐    ┌─────────────────────┐   │
                  │  │ MDSurfaceView  │───►│ Overlay.onTouch     │   │
                  │  │  onTouchEvent  │    │  ◄── injected       │   │
                  │  │   1. Overlay.onTouch(MotionEvent)            │
                  │  │   2. if !consumed → Native.mdTouch(x,y,a)    │
                  │  └────────────────┘    └─────────────────────┘   │
                  │                                                  │
                  │  ┌───────────────────────────────────────────┐   │
                  │  │ libinjector.so  (nossa)                   │   │
                  │  │                                           │   │
                  │  │  JNI_OnLoad()    → background thread:     │   │
                  │  │                    findModuleBase(libws.so)
                  │  │                                           │   │
                  │  │  Java_com_wsAndroid_Overlay_draw          │   │
                  │  │    └─► overlay::renderFrame() (ImGui+GLES2)   │
                  │  │        └─► PlayerWatch::tick() — refresh HP/MP│
                  │  │                                           │   │
                  │  │  Java_com_wsAndroid_Overlay_onTouch       │   │
                  │  │    └─► ImGui::AddMouse{Pos,Button}Event   │   │
                  │  │    return io.WantCaptureMouse             │   │
                  │  └───────────────────────────────────────────┘   │
                  │                                                  │
                  │  ┌───────────────────────────────────────────┐   │
                  │  │ libwarspear.so  (jogo, intacta)           │   │
                  │  │  - ~14 MB de C++ (MobileDragon framework) │   │
                  │  │  - todos os Java_com_aigrind_..._md*      │   │
                  │  └───────────────────────────────────────────┘   │
                  └──────────────────────────────────────────────────┘
```

## Componentes

### libinjector.so — nossa shared library

| Arquivo | Função |
|---|---|
| `src/main.cpp` | `JNI_OnLoad` — entry point. Cria thread em background que detecta `libwarspear.so` no `/proc/self/maps`. Não instala hooks (smali já encaminha tudo). |
| `src/overlay/Overlay.cpp` | Init do ImGui com backend GLES2 (`imgui_impl_opengl3` com `IMGUI_IMPL_OPENGL_ES2`). Função `renderFrame()` chama `ImGui::NewFrame` → desenha a janela → `ImGui::Render` → `RenderDrawData`. |
| `src/jni/OverlayJni.cpp` | `Java_com_wsAndroid_Overlay_draw` e `Java_com_wsAndroid_Overlay_onTouch` — exports JNI chamados pelo smali. Visibilidade `default` (override do `-fvisibility=hidden` global). |
| `src/game/HeapScan.cpp` | Scan de heap por `(HP, HPMax, MP, MPMax)` u32-tuple. Lê via `process_vm_readv` pra ser seguro em pages com guard. |
| `src/game/PlayerWatch.cpp` | Orquestra scan async, lock-on-best-candidate, refresh per-tick dos stats. |
| `src/memory.{h,cpp}` | Parsing de `/proc/self/maps` e `findModuleBase("libwarspear.so")`. |
| `src/logger.{h,cpp}` | `ALOG*` macros (V/D/I/W/E) → `__android_log_print(ANDROID_LOG_*, "WS_SDK", ...)`. |
| `src/probe_reader.cpp` | Binário standalone (não parte da .so). Lê `/proc/PID/mem` via root pra reconhecimento externo durante RE. |

### Smali patches (apk/patches/apply.sh)

Três patches são aplicados no APK descomprimido, todos idempotentes:

**1. MDActivity.smali `<clinit>` — carregar libinjector ANTES de libwarspear**

```diff
  .line 63
  :try_start_0
  const-string v0, "Warspear"
  const-string v1, "Loading warspear"
  invoke-static {v0, v1}, Landroid/util/Log;->i(...)I

+ # === ws-android injector: load BEFORE libwarspear ===
+ const-string v0, "injector"
+ invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
+ # === end injector ===

  .line 64
  const-string v0, "warspear"
  invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
```

**2. MDRenderer.smali `onDrawFrame` — render overlay APÓS render do jogo**

```diff
  .line 82
  invoke-static {}, Lcom/aigrind/mobiledragon/Native;->onDrawFrame()V

+ # === ws-android overlay: draw on top of the game's framebuffer ===
+ invoke-static {}, Lcom/wsAndroid/Overlay;->draw()V
+ # === end overlay ===

  .line 84
  :cond_2
```

**3. MDSurfaceView.smali `onTouchEvent` — rotear touches pro overlay primeiro**

```diff
  .method public onTouchEvent(Landroid/view/MotionEvent;)Z
      .locals 5

+     # === ws-android: let Overlay see the touch first ===
+     invoke-static {p1}, Lcom/wsAndroid/Overlay;->onTouch(Landroid/view/MotionEvent;)Z
+     move-result v4
+     if-eqz v4, :ws_touch_pass
+     const/4 v0, 0x1
+     return v0
+     :ws_touch_pass
+     # === end overlay touch ===

      .line 102
      invoke-static {}, Lcom/aigrind/warspear/WeakActivityHolder;->get()L...
```

**4. Skeleton class:** `com/wsAndroid/Overlay.smali` é adicionado ao smali tree com:

```smali
.class public Lcom/wsAndroid/Overlay;
.super Ljava/lang/Object;

.method public static native draw()V
.end method

.method public static native onTouch(Landroid/view/MotionEvent;)Z
.end method
```

Ambos os native methods são resolvidos via `dlsym` para `Java_com_wsAndroid_Overlay_draw` e `Java_com_wsAndroid_Overlay_onTouch` em `libinjector.so`.

Detalhes byte-a-byte e justificativas em [SMALI_PATCHES.md](SMALI_PATCHES.md).

## Fluxo por frame

```
GLSurfaceView render thread:
  ├── Android framework calls MDRenderer.onDrawFrame(GL10)
  │     └── original: Native.onDrawFrame()
  │           └── libwarspear renders game scene into current EGL surface
  │     └── PATCHED: Overlay.draw()
  │           └── JNI → Java_com_wsAndroid_Overlay_draw
  │                 ├── on first call: ImGui::CreateContext + ImGui_ImplOpenGL3_Init
  │                 │   AND ws::game::requestScan() → background thread
  │                 ├── ws::game::tick() — refresh HP/HPMax/MP/MPMax (cheap)
  │                 ├── ImGui::NewFrame → drawWindow → ImGui::Render
  │                 └── ImGui_ImplOpenGL3_RenderDrawData (uses current bound FBO)
  └── eglSwapBuffers() — flushes everything to display
```

```
UI thread (touch dispatch):
  ├── MDSurfaceView.onTouchEvent(MotionEvent)
  │     └── PATCHED: Overlay.onTouch(MotionEvent)
  │           └── JNI → Java_com_wsAndroid_Overlay_onTouch
  │                 ├── getAction / getX / getY via JNI method calls
  │                 ├── ImGui::AddMousePosEvent(x, y)
  │                 ├── ImGui::AddMouseButtonEvent(0, action == DOWN)
  │                 └── return io.WantCaptureMouse
  │     └── if consumed: return true (game doesn't see touch)
  │     └── if NOT consumed: fall through to game's handling
  │           ├── ... eventually: Native.mdTouch(x, y, action)
  │           └── libwarspear processes click
```

## Por que NÃO usamos inline hooks

A primeira tentativa foi inline hook em `Java_com_aigrind_mobiledragon_Native_onDrawFrame` (offset 0xb227c4 em libwarspear.so). **Quebrou catastroficamente** em LDPlayer/Bluestacks.

Causa: LDPlayer roda libwarspear.so (arm64) em CPU x86 via **libhoudini.so** (binary translator ARM→x86). Houdini **cacheia traduções de chunks**. Quando patcheamos os bytes ARM64 do início da função:

1. Houdini ainda tem a tradução x86 dos bytes antigos no cache
2. Próxima chamada bate no cache → executa código x86 baseado nos bytes antigos
3. Quando o cache eventualmente é invalidado, Houdini relê os bytes → vê nossos `LDR x16, [pc, #8]; BR x16; <addr>` → não sabe traduzir → SIGSEGV/SIGILL

Reproduzido com 3 libs distintas: `xhook` (PLT-only, libwarspear não importa `eglSwapBuffers` então não funcionou), `Dobby` (master quebrado em NDK r26), `shadowhook` (`Init linker mod failed` sob Houdini), e nosso próprio inline hook handwritten (process morria com signal 4/11 no GLThread após o primeiro frame).

**Smali patches são imunes a isso** porque:
1. Modificação é Java bytecode (DEX), não ARM
2. Carregada pela ART/Dalvik VM normalmente
3. Houdini só vê CHAMADAS pra nossas funções JNI — não vê patching, não tem cache pra invalidar
4. Mesma técnica funciona idêntica em devices ARM reais (sem Houdini)

História completa em [HOOKING_SAGA.md](HOOKING_SAGA.md). As libs xhook/shadowhook ficam vendoradas em `third_party/` (no `setup_third_party.sh`) pra uso futuro em devices reais.

## Bypass do anti-emulator

`libwarspear.so` chama o método Java `MDActivity.isEmulator()` no startup. Esse método retorna `false` (não detectou) — mas o C++ ainda **lê `/proc/cpuinfo`** e bate contra strings: "bluestack", "AuthenticAMD", "GenuineIntel" etc. Se achar qualquer uma, dispara `MDActivity.finish()` e o app fecha em ~1s.

Bypass usado: **bind-mount `/proc/cpuinfo`** com um cpuinfo falso de Snapdragon 888 (8 cores Cortex-A78). Implementado em `tools/mask_cpuinfo.sh`:

```bash
adb shell 'su -c "mount --bind /data/local/tmp/cpuinfo /proc/cpuinfo"'
```

Precisa root no device. Em LDPlayer é toggle nas settings; em devices reais não é necessário (cpuinfo já é ARM real).

Alternativa que **NÃO funciona**: editar `MDActivity.isEmulator` no smali pra retornar `false`. O C++ tem checagem independente via `/proc/cpuinfo` que continuaria detectando.

Alternativa robusta sem root: **hook em `fopen`/`open`** dentro de libinjector pra interceptar reads de `/proc/cpuinfo` e devolver bytes falsos. Não implementado ainda (precisa de PLT hook em libc — que xhook resolve em devices reais; em LDPlayer com Houdini, mesma situação dos inline hooks).

## Heap scan (Player Container localização)

O Linux SDK conhece a chain `BSS::GSI → GSI+0x28 → GM → GM+0x20 → Player Container`. No ARM64 build do `libwarspear.so` os offsets exatos diferem (ponteiros 8 bytes em vez de 4, alinhamentos diferentes). Em vez de re-derivar via Ghidra (caro), localizamos o Container por **assinatura semântica**:

```cpp
// HeapScan::findPlayerStatBlocks()
for each anonymous rw-p region in /proc/self/maps:
  for each 4-byte aligned offset:
    read (u32 a, u32 b, u32 c, u32 d)
    if (a in [10..100k] && a <= b && c <= d && plausible):
      record candidate, score, dedup nearby matches
return top 16
```

A leitura usa `process_vm_readv` (não `*ptr`) porque anonymous rw-p mappings **têm guard pages no meio**. Dereferenciar direto causa SIGSEGV (especialmente sob Houdini, que mata o processo inteiro). `process_vm_readv` retorna -1/EFAULT em endereços inválidos.

Uma vez locked num candidato, `PlayerWatch::tick()` lê os 4 u32s do mesmo endereço a cada frame:

```cpp
uint32_t stats[4];
process_vm_readv(getpid(), local_iov, 1, remote_iov, 1, 0);
// stats[0] = HP, stats[1] = HPMax, stats[2] = MP, stats[3] = MPMax
```

Para Name e Level, sondas heurísticas (sweep de offsets ao redor) — descritas em [game/PlayerWatch.cpp](../android-injector/src/game/PlayerWatch.cpp).

## Próximas camadas

**M4 — Socket server** (porta direta do SDK Linux):

```
adb forward localabstract:ws_mem tcp:9999
    │
    └──► /data/local/tmp/ws_mem.sock  (Unix socket dentro do app)
            │
            └──► libinjector thread: aceita conexões, executa
                  CMD_READ / CMD_WRITE / CMD_SCAN / CMD_SEND_PACKET
                  igual ao Linux SDK
```

Reuso direto do código de `ws-hack2/src/debug/SocketServer.cpp`.

**M5 — Packet hooks**:

`libwarspear.so` importa `send`/`recv` da libc. Isso é **PLT** (importa) → xhook funciona pra patchear em devices ARM reais. Em LDPlayer/Houdini, mesma situação de antes: precisamos rotear pela camada Java (`Java_com_aigrind_mobiledragon_Native_*` da rede) ou aceitar que packet hooks só funcionam em hardware real.
