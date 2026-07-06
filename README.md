# ws-android — Warspear Online SDK (Android port)

Port do SDK Linux [`ws-hack2`](https://github.com/lucas-freitas00/ws-hack2) para Android, focado em **devices não-rooteados em produção** (root só é usado em dev pra mascarar detecções de emulador). Roda em emuladores ARM-on-x86 (LDPlayer, Bluestacks, etc.) e diretamente em hardware ARM real.
---

<img width="322" height="616" alt="image" src="https://github.com/user-attachments/assets/b5a5a051-c595-492f-af5d-d5165b7bdfb1" />

---

> **Versão alvo do jogo:** Warspear Online **13.3.0** (`com.aigrind.warspear`)
> **ABIs suportadas:** `arm64-v8a` (primária) e `armeabi-v7a`

---


> ### 🕯️ Por que este projeto é público
>
> Este SDK foi desenvolvido em privado por **GodIsBack — Rogue do servidor Tourmaline**.
>
> Após um **banimento injusto aplicado pela AIGRIND** contra a conta GodIsBack, decidi **tornar o projeto público** e **encerrar meu desenvolvimento nele**. O código fica aqui, documentado até o último milestone alcançado, pra quem quiser estudar, continuar ou aprender engenharia reversa de jogos mobile.
>
> Não vou continuar. Mas deixei o caminho todo mapeado — inclusive como plugar um agente de IA pra seguir de onde parei. Boa sorte a quem pegar. **— GodIsBack**

---

## Índice

- [O que é](#o-que-é)
- [O que está pronto hoje](#o-que-está-pronto-hoje)
- [Como funciona (arquitetura em 30s)](#como-funciona-arquitetura-em-30s)
- [Pré-requisitos](#pré-requisitos)
- [Build do zero](#build-do-zero)
- [Ciclo de desenvolvimento (atualizar)](#ciclo-de-desenvolvimento-atualizar)
- [Como progredir — roadmap com caminho](#como-progredir--roadmap-com-caminho)
- [Bug atual (bloqueador do M3.5)](#bug-atual-bloqueador-do-m35)
- [Conectar um agente de IA pra ajudar no desenvolvimento](#conectar-um-agente-de-ia-pra-ajudar-no-desenvolvimento)
- [Estrutura do repositório](#estrutura-do-repositório)
- [Relação com o SDK Linux original](#relação-com-o-sdk-linux-original)
- [Aviso legal](#aviso-legal)

---

## O que é

Um SDK de engenharia reversa que injeta um **overlay ImGui** e um **leitor de memória** dentro do processo do jogo Warspear Online. Serve pra ler/escrever estado do jogo (HP/MP/nome/level do player, e futuramente packets de rede) a partir de dentro do próprio app.

O grande desafio técnico resolvido aqui: **fazer isso funcionar sob emuladores ARM-on-x86 (Houdini)**, onde inline hooks tradicionais quebram. A solução é injetar via **APK repack + smali patches** em vez de hooks nativos — imune ao binary translator e idêntico em hardware ARM real.

Este port compartilha ~100% do trabalho de RE das classes do jogo com o SDK Linux (mesmo framework MobileDragon: `Container`, `GameManager`, `Svp::*`, `EffectsManager`, `Chat`…) — só muda ABI/RVAs/offsets.

## O que está pronto hoje

| Milestone | Entrega | Status |
|---|---|---|
| **M0** — Setup | LDPlayer rooted + cpuinfo mascarado + ADB conectado + jogo rodando | ✅ |
| **M1** — Hello World | `libinjector.so` carrega **dentro** do processo via APK repack. Visível em `adb logcat -s WS_SDK`. | ✅ |
| **M2** — Overlay ImGui | Janela ImGui renderizando no GLES2 context do jogo (sobre o framebuffer). Driver via smali patch em `MDRenderer.onDrawFrame`. | ✅ |
| **M3** — Player stats + touch | Janela mostra Name/Level/HP/MP. Heap scan localiza o Container do player. Touch roteado pro ImGui via smali patch em `MDSurfaceView.onTouchEvent` — janela arrastável e minimizável. | ✅ |
| **M3.5** — Container offsets + tradutor | Offsets ARM64 do Container descobertos (HP=`0x1A4`, Name=`0x090`, vtable RVA=`0xC903A0`). Tradutor `helpers/offsets_linux_to_arm64.py` gera 532 offsets. PlayerWatch reescrito pra scan por vtable. | ⚠️ **scan quebrando in-game** |
| **M4** — Socket server | Unix socket `/data/local/tmp/ws_mem.sock` + `adb forward`. Comandos `CMD_READ/WRITE/SCAN/SEND_PACKET`. | ⬜ pendente |
| **M5** — Packet hooks | Interceptação de `send`/`recv` via PLT hook (ARM real) ou smali na camada de rede Java. | ⬜ pendente |
| **M6** — Combat scripts + ESP | Port das combat scripts e ESP do SDK Linux. | ⬜ pendente |

Documentação técnica completa em [docs/](docs/):

- [SETUP.md](docs/SETUP.md) — do zero ao primeiro `WS_SDK` no logcat
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) — design do sistema, fluxo de chamadas
- [SMALI_PATCHES.md](docs/SMALI_PATCHES.md) — cada patch explicado linha a linha
- [HOOKING_SAGA.md](docs/HOOKING_SAGA.md) — por que NÃO usamos inline hooks (a história do Houdini)
- [MILESTONES.md](docs/MILESTONES.md) — M1–M3.5 com screenshots e logs
- [PORT_GUIDE.md](docs/PORT_GUIDE.md) — diff Linux→Android + roadmap
- [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) — problemas comuns (VBS, anti-emu, cpuinfo)
- [SESSION_2026-05-13_M3.5_OFFSETS.md](docs/SESSION_2026-05-13_M3.5_OFFSETS.md) — sessão da descoberta de offsets

## Como funciona (arquitetura em 30s)

Sem inline hooks — sob Houdini eles crasham com SIGILL (o binary translator cacheia traduções ARM e patchear bytes invalida o cache). Estratégia: **puro Java/smali**.

```
System.loadLibrary("injector")     ← smali patch em MDActivity.<clinit>, ANTES de libwarspear
MDRenderer.onDrawFrame()           ← smali patch: chama Overlay.draw() após o render do jogo
MDSurfaceView.onTouchEvent()       ← smali patch: roteia touch pro Overlay.onTouch() primeiro
        │
        ▼ JNI
libinjector.so (nossa)
  ├─ Java_com_wsAndroid_Overlay_draw     → ImGui NewFrame → drawWindow → Render (GLES2)
  ├─ Java_com_wsAndroid_Overlay_onTouch  → ImGui AddMouse{Pos,Button}Event; return WantCaptureMouse
  ├─ PlayerWatch::tick()                 → lê HP/MP/Name do Container por offset, cada frame
  └─ HeapScan / findByVtable             → localiza o Container via vtable RVA (process_vm_readv)
```

Detalhe byte-a-byte em [ARCHITECTURE.md](docs/ARCHITECTURE.md) e [SMALI_PATCHES.md](docs/SMALI_PATCHES.md).

## Pré-requisitos

- **Java 21** (pro apktool/uber-apk-signer)
- **Android SDK** em `~/Android/Sdk` (platform-tools + build-tools 33.0.2)
- **NDK r26b** em `~/Android/Sdk/ndk/...`
- **CMake 3.22+** e **Python 3**
- Um device/emulador alvo:
  - **Emulador x86 (LDPlayer/Bluestacks):** precisa **root** (pra mascarar cpuinfo) + ARM translation
  - **Device ARM físico:** funciona sem root (cpuinfo já é ARM real)

Setup detalhado do ambiente (host Windows + VM + netsh portproxy) em [SETUP.md](docs/SETUP.md).

## Build do zero

```bash
# 1. Ambiente (exporta ANDROID_HOME / NDK / PATH)
source tools/env.sh

# 2. Clona dependências vendoradas (imgui GLES2, xhook, shadowhook)
tools/setup_third_party.sh

# 3. Conecta ao device (auto-descobre LDPlayer/Bluestacks na LAN)
tools/bs_connect.sh
adb devices        # confirme que apareceu

# 4. Compila libinjector.so + probe_reader pras duas ABIs (~30s)
( cd build && for abi in arm64-v8a armeabi-v7a; do
    mkdir -p $abi && cd $abi
    cmake ../../android-injector \
      -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI=$abi -DANDROID_PLATFORM=android-23 -DCMAKE_BUILD_TYPE=Release
    cmake --build . -j && cd ..
  done )

# 5. Descompila + repack + assina o APK (aplica os smali patches + copia a .so)
apk/unpack.sh apk/warspear-13.3.0.apk
apk/repack.sh

# 6. Instala no device
tools/install_apk.sh apk/repacked/signed/warspear-modded-aligned-debugSigned.apk

# 7. Em emulador x86: mascara cpuinfo (bypass anti-emulator; precisa root)
tools/mask_cpuinfo.sh

# 8. Lança e observa
adb shell am start -n com.aigrind.warspear/.LoaderActivity
adb logcat -s WS_SDK
```

Você deve ver:

```
WS_SDK : === ws-android injector loaded (pid=...) ===
WS_SDK : init: libwarspear.so base = 0x...
WS_SDK : overlay: ImGui initialized
```

## Ciclo de desenvolvimento (atualizar)

Loop de iteração rápido (~30s) depois do primeiro build:

```bash
# 1. Edita código C++ em android-injector/src/
# 2. Recompila só a ABI que você testa
( cd build/arm64-v8a && cmake --build . -j )

# 3. Repack (re-aplica patches, copia a .so nova, re-assina)
apk/repack.sh

# 4. Reinstala + reinicia + observa
tools/install_apk.sh apk/repacked/signed/*-debugSigned.apk
adb shell am force-stop com.aigrind.warspear
adb shell am start -n com.aigrind.warspear/.LoaderActivity
adb logcat -s WS_SDK
```

Pra RE em runtime sem rebuildar o injector, use o **`probe_reader`** (binário standalone via root):

```bash
adb push build/arm64-v8a/probe_reader /data/local/tmp/
adb shell "su -c 'chmod 755 /data/local/tmp/probe_reader'"

# Buscar um valor u32 visível no jogo (ex.: HP=751) na heap inteira do jogo
adb shell "su -c '/data/local/tmp/probe_reader <PID> search 751'"
# → dump de candidatos (HP/HPMax/MP/MPMax) + verificação de vtable nas 0x600 bytes anteriores

# Dump raw de N bytes de um endereço (inspecionar struct)
adb shell "su -c '/data/local/tmp/probe_reader <PID> <addr_hex> <n_bytes>'"
```

`<PID>` do jogo: `adb shell pidof com.aigrind.warspear`.

## Como progredir — roadmap com caminho

O bloqueador imediato é o **M3.5** (scan por vtable não acha o Container in-game). Resolva isso primeiro, depois siga a ordem.

### Passo 0 — Fechar o M3.5 (desbloquear stats confiáveis)

Ver [seção do bug abaixo](#bug-atual-bloqueador-do-m35). Sem isso, o overlay não mostra dados reais consistentemente.

### Passo 1 — M4: Socket server (RPC de memória)

Portar `ws-hack2/src/debug/SocketServer.cpp` pra dentro de `libinjector`.

**Caminho:**
1. Nova thread em `libinjector` (spawn no `JNI_OnLoad` ou no primeiro frame) que cria um Unix socket em `/data/local/tmp/ws_mem.sock`, `chmod 666`.
2. Loop de accept + dispatch dos comandos: `CMD_READ` (lê N bytes de addr), `CMD_WRITE` (escreve), `CMD_SCAN` (pattern scan em range), `CMD_GET_TOKEN` (session token).
3. Reusar o wire format do SDK Linux → clientes Python existentes funcionam quase direto.
4. Do host: `adb forward localabstract:ws_mem tcp:9999` → conecta com script Python/qualquer socket TCP.

**Verificação:** `CMD_READ` no endereço do Container (achado pelo probe_reader) deve devolver os mesmos bytes de HP/MP.

### Passo 2 — Descobrir o offset do Level + mais vtables

Com o Container localizado em runtime:
1. `probe_reader <PID> <container_addr> 0x400` → varre offsets candidatos procurando a u8 com valor 1–50 (o level visível no jogo).
2. Confirmar contra o level do personagem na HUD.
3. Repetir pra achar vtables de NPC/Monster/outras classes (no Linux: `VTABLE_container_player/enemy/npc/...`).
4. Alimentar os offsets confirmados de volta no `helpers/offsets_linux_to_arm64.py` (lista VERIFIED) → recalibra as 500+ predições.

### Passo 3 — M5: Packet hooks

`libwarspear.so` importa `send`/`recv` da libc.
- **Device ARM real:** PLT-hookável com `xhook` (já vendorado em `third_party/`). Instalar hook em `send`/`recv`, dump/parse dos GostPackets.
- **Emulador Houdini:** PLT hook em libc pode sofrer do mesmo problema de cache — plano B é achar a função Java/JNI de rede (`nm -D libwarspear.so | grep -i net`) e aplicar smali patch, igual M2/M3.

### Passo 4 — M6: Combat scripts + ESP

Lógica platform-agnostic (combat, A*, ScriptRegistry) migra **direto** do SDK Linux. ESP precisa dos offsets de posição/entidade (refazer via probe_reader, mesma técnica do Container).

### Regra de ouro pra offsets

Nada de re-derivar tudo no Ghidra. **Ache um valor visível no jogo → `probe_reader search` → localize a struct → confirme o offset → calibre o tradutor.** Barato e rápido.

## Bug atual (bloqueador do M3.5)

Sintoma: `findByVtable(base + 0xC903A0)` não retorna o Container correto in-game (Name fica `(unknown)`, stats não batem). Offsets estão certos (confirmados via probe_reader com o Necromancer "Tailegork"), mas o scan em runtime falha.

**Hipóteses a investigar (em ordem):**

1. **Vtable difere por classe de personagem** — Tailegork é Necromancer. Mage/Ranger/etc. podem ter vtable diferente (polimorfismo/herança). → Rodar `probe_reader search <HP_atual>` in-game com outra classe, ver qual vtable aparece, comparar com `0xC903A0`.
2. **Base da libwarspear sob Houdini** — a base muda entre sessões (`0x4380000` → `0x4018000`). O RVA é relativo à base, então deveria funcionar; confirmar que `findModuleBase("libwarspear.so")` retorna a base **ARM64** (não o mapping de tradução do Houdini).
3. **Filtro rejeitando match válido** — `findByVtable` dropa `hp[1] > 200000` e all-zeros. Pode matar o Container quando HP=0 (morte/logout/transição). Relaxar/logar.
4. **vtable não está nos primeiros 8 bytes** — herança virtual pode deslocar. Adicionar log dos primeiros 8 bytes de cada região `rw-p` anonymous quando o scan der 0 hits.

Detalhes completos em [SESSION_2026-05-13_M3.5_OFFSETS.md](docs/SESSION_2026-05-13_M3.5_OFFSETS.md).

## Conectar um agente de IA pra ajudar no desenvolvimento

Este repo foi construído com assistência de IA (Claude Code) e **a documentação foi escrita pra ser consumível por um agente**. Qualquer um pode plugar um agente de codificação e continuar de onde o M3.5 parou.

### 1. Instalar o agente

[Claude Code](https://claude.com/claude-code) (recomendado — foi o usado aqui):

```bash
npm install -g @anthropic-ai/claude-code
cd ws-android
claude            # abre o agente no diretório do repo
```

Também existe como extensão de IDE (VS Code / JetBrains) e app desktop. Qualquer agente com acesso a shell + edição de arquivos serve (Cursor, Aider, etc.) — as instruções abaixo valem pra qualquer um.

### 2. Dar contexto ao agente (arquivo `CLAUDE.md`)

Crie um `CLAUDE.md` na raiz do repo. O Claude Code lê ele automaticamente a cada sessão. Template inicial:

```markdown
# ws-android — contexto pro agente

Port Android do SDK Linux ws-hack2 pra Warspear Online 13.3.0 (arm64-v8a).
Injeta overlay ImGui + leitor de memória via APK repack + smali patches
(NÃO inline hooks — quebram sob Houdini/LDPlayer).

## Onde está o quê
- Código C++ do injector: android-injector/src/
  - main.cpp (JNI_OnLoad), overlay/Overlay.cpp, jni/OverlayJni.cpp
  - game/HeapScan.cpp (findByVtable), game/PlayerWatch.cpp (tick), game/Offsets_android.h
- Smali patches: apk/patches/ (apply.sh)
- Tradutor de offsets: helpers/offsets_linux_to_arm64.py
- RE em runtime: android-injector/src/probe_reader.cpp (build/<abi>/probe_reader)

## Como buildar/testar (ciclo ~30s)
1. ( cd build/arm64-v8a && cmake --build . -j )
2. apk/repack.sh
3. tools/install_apk.sh apk/repacked/signed/*-debugSigned.apk
4. adb shell am force-stop com.aigrind.warspear && adb shell am start -n com.aigrind.warspear/.LoaderActivity
5. adb logcat -s WS_SDK

## Estado atual
M3.5 — vtable scan não acha o Container in-game. Ver docs/SESSION_2026-05-13_M3.5_OFFSETS.md.
Próximo: fechar M3.5, depois M4 (socket server).

## Regras
- Nunca use inline hooks (SIGILL sob Houdini). Só smali/JNI.
- Pra achar offset: probe_reader search <valor_visível>, não Ghidra.
- Toda leitura de memória do jogo via process_vm_readv (guard pages matam deref direto).
```

### 3. Dar as "mãos" ao agente (acesso ao device)

O loop de dev depende de `adb`. Garanta que o agente pode rodar:
- `adb` (build/install/logcat/shell) — ferramenta principal
- `cmake` / `apk/repack.sh` — build
- `probe_reader` via `adb shell su -c` — o "olho" do agente na memória do jogo

Com isso o agente consegue rodar o ciclo **editar → buildar → instalar → ler logcat → inspecionar memória** sozinho, e iterar sobre uma hipótese até o overlay mostrar dados certos.

### 4. Prompts de exemplo pra continuar

```
"Leia docs/SESSION_2026-05-13_M3.5_OFFSETS.md. O vtable scan não acha o
 Container in-game. Adicione log em findByVtable dumpando os primeiros 8 bytes
 de cada região rw-p anonymous quando der 0 hits, rebuilde, instale e me mostre
 o logcat. Estou com o personagem logado in-game."

"Implemente o M4 (socket server) portando ws-hack2/src/debug/SocketServer.cpp.
 Comece só com CMD_READ. Exponha via /data/local/tmp/ws_mem.sock e teste lendo
 o endereço do Container."

"Rode probe_reader search <HP> pra localizar o Container, depois varra
 offsets 0x300..0x400 procurando o Level (valor 1-50) e confirme contra a HUD."
```

### 5. Fluxo de trabalho sugerido

- Um milestone por sessão. Peça pro agente ler o doc relevante em `docs/` **antes** de codar.
- Sempre exigir a etapa de verificação (logcat / probe_reader) — não confie em "deve funcionar".
- Ao confirmar um offset novo, peça pro agente adicioná-lo à lista VERIFIED do tradutor e regenerar `Offsets_android.h`.
- Commits pequenos por milestone, seguindo o padrão do histórico (`feat(m4): ...`).

## Estrutura do repositório

```
ws-android/
├── README.md                              # você está aqui
├── CLAUDE.md                              # (crie você) contexto pro agente de IA
├── docs/                                  # documentação técnica (feita pra humano E agente)
│   ├── SETUP.md ARCHITECTURE.md SMALI_PATCHES.md HOOKING_SAGA.md
│   ├── MILESTONES.md PORT_GUIDE.md TROUBLESHOOTING.md
│   ├── SESSION_2026-05-13_M3.5_OFFSETS.md
│   └── m{1,2,3}_overlay_screenshot.png
│
├── android-injector/                      # NDK CMake project → libinjector.so
│   ├── CMakeLists.txt
│   └── src/
│       ├── main.cpp                       # JNI_OnLoad — entry point
│       ├── memory.{h,cpp}                 # /proc/self/maps, findModuleBase
│       ├── logger.{h,cpp}                 # ALOG* → adb logcat -s WS_SDK
│       ├── probe_reader.cpp               # tool standalone: lê /proc/PID/mem via root
│       ├── overlay/Overlay.{h,cpp}        # ImGui init + render frame (GLES2)
│       ├── jni/OverlayJni.cpp             # Java_com_wsAndroid_Overlay_{draw,onTouch}
│       └── game/
│           ├── HeapScan.{h,cpp}           # findByVtable / heap scan (process_vm_readv)
│           ├── PlayerWatch.{h,cpp}        # scan async + refresh per-tick
│           └── Offsets_android.h          # 532 offsets ARM64 (auto-gerado)
│
├── apk/
│   ├── warspear-13.3.0.apk                # APK original (gitignored)
│   ├── unpack.sh repack.sh
│   ├── patches/{Overlay.smali, apply.sh}  # nossa classe + aplicador de patches
│   └── extracted/ repacked/               # working dirs (gitignored)
│
├── helpers/
│   └── offsets_linux_to_arm64.py          # tradutor Offsets.h Linux → Offsets_android.h
│
├── data/offsets_android.json              # dump JSON dos offsets (auto-gerado)
│
├── tools/                                 # env, adb connect, mask_cpuinfo, install, etc.
│
├── third_party/                           # gitignored — clonado por setup_third_party.sh
│   ├── imgui/ xhook/ shadowhook/
│
└── build/                                 # gitignored — saída do CMake
    ├── arm64-v8a/{libinjector.so, probe_reader}
    └── armeabi-v7a/{libinjector.so, probe_reader}
```

## Relação com o SDK Linux original

| Aspecto | Linux SDK (`ws-hack2`) | Android port (este repo) |
|---|---|---|
| Injeção | `LD_PRELOAD` | **APK repack** + `System.loadLibrary` no smali |
| Render hook | PLT em `glXSwapBuffers` | **Smali patch** em `MDRenderer.onDrawFrame` |
| Input | `XNextEvent` hook | **Smali patch** em `MDSurfaceView.onTouchEvent` |
| Overlay | ImGui + OpenGL 1.x | ImGui + **OpenGL ES 2.0** |
| Mem reader (RPC) | Unix socket `/workspace/ws_mem.sock` | (M4 pendente) socket + `adb forward` |
| Packet hooks | xhook em `send`/`recv` | (M5 pendente) smali na rede Java |
| Logger | stdout/stderr | `__android_log_print` → `adb logcat -s WS_SDK` |
| Arquitetura | x86 32-bit | **arm64-v8a** (primária) + armeabi-v7a |
| Offsets/RVAs | fonte da verdade em `Offsets.h` | refeitos pra ARM64 (ptrs 8 bytes) via probe_reader + tradutor |

Os dois projetos compartilham 100% do RE das classes do jogo (mesmo framework MobileDragon) — só mudam ABI/RVAs/offsets.

## Aviso legal

Projeto de **engenharia reversa e pesquisa educacional** sobre um jogo mobile. Publicado como material de estudo de RE (smali patching, contorno de binary translation, localização de estruturas em memória, JNI, ImGui embarcado).

Usar modificações de cliente em servidores online geralmente **viola os Termos de Serviço** do jogo e pode resultar em banimento. Use por sua conta e risco, preferencialmente em ambiente offline/isolado. O autor não se responsabiliza pelo uso feito por terceiros.

---

<p align="center">
<sub>Feito por <b>GodIsBack</b> — Rogue de Tourmaline. Tornado público após banimento injusto pela AIGRIND.<br>
Não será mais continuado por mim. O caminho está mapeado — o resto é com você. 🗡️</sub>
</p>
