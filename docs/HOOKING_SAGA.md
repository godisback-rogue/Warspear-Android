# HOOKING_SAGA — por que NÃO usamos inline hooks

Documento histórico da decisão técnica de usar smali patches em vez de inline hooks ARM64. Útil para entender por que **3 libs de hook diferentes falharam** e a solução final ficou em pure-Java.

## Setup

Alvo do hook: `Java_com_aigrind_mobiledragon_Native_onDrawFrame` em `libwarspear.so`. É um JNI export chamado por `MDRenderer.onDrawFrame(GL10)` a cada frame. Hooká-lo daria controle sobre cada frame do jogo — o caso de uso clássico de overlay.

Ambiente: LDPlayer 9 com instância Android 9, ABI primária `x86_64`. APK só tem ARM ABIs (`arm64-v8a`, `armeabi-v7a`), então roda **via libhoudini.so** (binary translator ARM→x86 da Intel, embarcado no LDPlayer).

## Tentativa 1: xhook (PLT hook)

[xhook](https://github.com/iqiyi/xhook) faz **PLT hook**: substitui entradas na GOT de um módulo pra redirecionar chamadas a funções **importadas**.

```cpp
xhook_register(".*", "eglSwapBuffers",
               (void*)my_swap, (void**)&orig_swap);
xhook_refresh(0);
```

`xhook_refresh` retornou 0 (sucesso). **Mas `orig_swap` ficou em `0x0`** — nenhuma entrada GOT foi patcheada.

Por quê? `readelf --dyn-syms /tmp/libwarspear-arm64.so | grep egl`:

```
eglGetConfigAttrib
eglGetConfigs
eglGetDisplay
```

**libwarspear não importa `eglSwapBuffers`**. Só 3 funções EGL. O rendering passa pelo **Java GLSurfaceView**, que faz o swap internamente após `MDRenderer.onDrawFrame` retornar. Nada pra fazer PLT hook em.

xhook só funcionaria pra hookar imports tipo `send`/`recv`/`fopen`. Pra exports/funções internas, precisa de inline hook.

## Tentativa 2: Dobby (inline hook)

[Dobby](https://github.com/jmpews/Dobby) é o padrão pra inline hook ARM/ARM64. API simples: `DobbyHook(addr, my_fn, &orig_fn)`.

Tentei buildar com NDK r26b:

```
error: no member named 'load_address' in 'RuntimeModule'
      if (module.load_address) {
          ~~~~~~ ^
```

Dobby master tem um bug — vários arquivos referenciam `module.load_address` mas a struct foi refatorada pra ter só `module.base`. Tentei patchear com sed:

```bash
find third_party/dobby -name '*.cc' -exec sed -i 's/module\.load_address/module.base/g' {} +
```

Subsequent error:
```
error: no member named 'start' in 'MemRange'
  return (a.start < b.start);
                    ^
```

Mais problemas estruturais. Abandonei.

## Tentativa 3: shadowhook (Bytedance)

[shadowhook](https://github.com/bytedance/android-inline-hook) é uma lib de inline hook mantida pela ByteDance. Build limpo, API limpa:

```cpp
shadowhook_init(SHADOWHOOK_MODE_SHARED, false);
void* stub = shadowhook_hook_sym_name(
    "libwarspear.so",
    "Java_com_aigrind_mobiledragon_Native_onDrawFrame",
    (void*)my_hook, (void**)&orig);
```

Build: OK. Init falhou em runtime:

```
E shadowhook_tag: shadowhook init, return: 12, real-init: yes
E WS_SDK         : shadowhook_init failed: 12 (Init linker mod failed)
```

Errno 12 = `SHADOWHOOK_ERRNO_INIT_LINKER`. shadowhook precisa patchear partes do **Bionic linker** pra interceptar `dlopen` (necessário pro modo SHARED). Sob **Houdini**, o linker é um misto de ARM + x86, e a patcha falha.

## Tentativa 4: inline hook próprio

Escrevi um inline hook minimalista ARM64. ~100 linhas:

```cpp
// Method:
//   - Save first 16 bytes of target function
//   - Allocate trampoline page with saved bytes + jump-back
//   - Patch target with: LDR x16, [pc, #8]; BR x16; <hook_addr>
//   - Return trampoline pointer to caller for invoking original
```

Build: OK. Hook installation reportou sucesso:

```
WS_SDK: hooks: resolved onDrawFrame=0x4b5e7c4
WS_SDK: hooks: onDrawFrame hooked  (trampoline=0x7639175ce000)
```

Próximo log: **processo morto, signal 4**. Tombstone:

```
F DEBUG : pid: 9878, tid: 9961, name: GLThread 230
F DEBUG : signal 4 (SIGILL), code 1 (ILL_ILLOPC)
F DEBUG : #00 pc 0x117bf  /system/lib64/libhoudini.so
F DEBUG : #01 pc 0x1db14f /system/lib64/libhoudini.so
```

**SIGILL dentro do libhoudini.so**. O processo crashou **traduzindo nossa instrução**.

## Por que Houdini quebra inline hooks ARM64

Houdini (e produtos similares: Microsoft's HAGS, NetEase NB, etc.) faz **JIT translation** de ARM pra x86:

1. Quando uma função ARM64 é chamada pela primeira vez, Houdini lê os bytes ARM, traduz pra x86, **armazena no cache**, e executa a versão x86.
2. Chamadas subsequentes usam direto o cache — não relêem os bytes ARM.
3. Cache é invalidado por sinais como `mprotect`/`madvise` (em teoria).

Quando fazemos inline hook:
1. Patcheamos os primeiros 16 bytes da função ARM (substituímos por nossa trampoline).
2. Houdini **ainda tem o x86 cache do código original** — próxima call executa código antigo.
3. Eventualmente, alguma operação força re-tradução: Houdini relê os bytes, **vê nossas instruções `LDR x16, [pc, #8]; BR x16` apontando pra heap (nossa hook function)**. Pra Houdini, isso é confuso:
   - O `LDR + BR` aponta pra heap que tem código **ARM64** (nossa lib injetada).
   - Houdini precisa traduzir ESSA região também — mas ela não estava registrada como código.
   - Resultado: tentativa de executar bytes inválidos → SIGILL.

A mesma técnica funciona perfeitamente em devices ARM reais (sem Houdini), e em emuladores nativos (qemu-system-aarch64). Mas **todos os emuladores x86 populares (LDPlayer, Bluestacks, MEmu, NoxPlayer, MuMu) usam Houdini ou equivalente**, então inline hooks ARM são impraticáveis nesse ambiente.

## Solução: smali patch + JNI

A epifania foi: **só modificar o lado Java do APK**, não tocar em bytes ARM. Plano:

1. **Adicionar uma classe Java nova** ao APK: `com.wsAndroid.Overlay` com `static native draw()`.
2. **Patchear o smali do jogo** pra chamar `Overlay.draw()` no lugar que a gente queria hookar (`MDRenderer.onDrawFrame` após `Native.onDrawFrame`).
3. **Implementar `Java_com_wsAndroid_Overlay_draw` em libinjector** — é só uma função JNI normal exportada.
4. ART/Dalvik resolve nosso símbolo via `dlsym(libinjector.so, "Java_com_wsAndroid_Overlay_draw")` na primeira call.

Por que isso passa por Houdini sem problema:

- **Nossa libinjector.so (ARM64) é traduzida UMA VEZ** pelo Houdini no carregamento, igual a libwarspear.so. Sem patching → sem cache invalidation → sem confusion.
- A "modificação" é no `classes.dex` — bytecode Java executado pela ART. **Bytecode não é traduzido pela Houdini** (a Houdini só traduz código nativo ARM). O bytecode patcheado roda nativo na ART (que pode ela própria ser x86, mas a abstração é VM-level).
- Resultado: zero impacto na cache de tradução do Houdini, e o mesmo APK roda idêntico em devices ARM reais.

Tradeoffs:

| Inline hook (real device) | Smali patch (LDPlayer + real device) |
|---|---|
| Hookar QUALQUER função nativa | Só pontos chamados a partir do Java |
| Sem APK repack necessário (carrega via `LD_PRELOAD` ou outras técnicas) | APK repack obrigatório |
| Risco: function prologue não é PIC, hook quebra | Sem risco — smali é estável |
| Performance: ~zero overhead | Performance: overhead de uma chamada JNI extra por hook |

Pra ws-android, os pontos que queremos hookar são **todos** chamados a partir do Java (per-frame render, touch events, talvez network). Smali resolve. Se algum dia precisarmos hookar algo puramente nativo (ex: uma função usada internamente pela libwarspear sem ponte Java), aí teremos que recorrer a inline hook — e nesse caso, **só funciona em devices ARM reais**.

## Conclusão

xhook/shadowhook/Dobby ficam vendoradas em `third_party/` como referência futura — quando o projeto migrar pra suportar devices ARM reais como target primário, voltam à mesa. Por enquanto:

- **LDPlayer/Bluestacks/MEmu**: smali patches via `apk/patches/apply.sh`
- **Real Android phone**: smali patches funcionam idênticos; inline hooks adicionais possíveis se precisarmos

A escolha de smali é, ironicamente, a opção **mais portátil** — funciona em qualquer ambiente onde ART roda.
