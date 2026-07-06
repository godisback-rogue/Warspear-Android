# SMALI_PATCHES

Cada patch que aplicamos no APK, linha a linha, com justificativa. Todos os patches são aplicados por [`apk/patches/apply.sh`](../apk/patches/apply.sh) — idempotente (re-rodar não duplica).

## Patches aplicados

| Arquivo modificado | Tipo | Função |
|---|---|---|
| `smali/com/aigrind/warspear/MDActivity.smali` | inline insert no `<clinit>` | Carrega `libinjector.so` antes da `libwarspear.so` |
| `smali/com/aigrind/warspear/MDRenderer.smali` | inline insert em `onDrawFrame` | Chama `Overlay.draw()` após o render do jogo |
| `smali/com/aigrind/warspear/MDSurfaceView.smali` | inline insert em `onTouchEvent` | Roteia touch pelo `Overlay.onTouch()` antes do jogo |
| `smali/com/wsAndroid/Overlay.smali` | classe nova | Skeleton com `static native draw()` e `onTouch(MotionEvent)` |

`lib/arm64-v8a/libinjector.so` e `lib/armeabi-v7a/libinjector.so` são adicionadas ao APK pelo mesmo script.

---

## 1. MDActivity.smali — load injector first

### Antes

```smali
.method static constructor <clinit>()V
    .locals 2

    invoke-static {}, Lcom/aigrind/utils/workarounds/Workarounds;->ENABLE()V

    .line 63
    :try_start_0
    const-string v0, "Warspear"

    const-string v1, "Loading warspear"

    invoke-static {v0, v1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 64
    const-string v0, "warspear"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0
    ...
```

### Depois

```smali
.method static constructor <clinit>()V
    .locals 2

    invoke-static {}, Lcom/aigrind/utils/workarounds/Workarounds;->ENABLE()V

    .line 63
    :try_start_0
    const-string v0, "Warspear"

    const-string v1, "Loading warspear"

    invoke-static {v0, v1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    # === ws-android injector: load BEFORE libwarspear ===
    const-string v0, "injector"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
    # === end injector ===

    .line 64
    const-string v0, "warspear"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0
    ...
```

### Por quê

- **DENTRO do `try_start_0`**: se `libinjector.so` faltar ou der erro, o `catch_0` existente cobre e o jogo continua (degrada graciosamente).
- **ANTES de `loadLibrary("warspear")`**: nosso `JNI_OnLoad` precisa estar registrado pra rodar o setup do background thread antes do jogo começar a carregar suas próprias estruturas. Hoje não dependemos disso (não há hooks pré-warspear), mas a ordem permanece pra deixar a porta aberta a M4/M5.
- **Reuso de `v0`**: o registrador foi usado pra "Warspear" mas o valor não é mais necessário. Reescrever com "injector" é safe.
- O smali registrer `.locals 2` permanece (v0, v1 são os já existentes).

### Aplicado por

```python
# apk/patches/apply.sh (extrato Python embedded)
ANCHOR = 'invoke-static {v0, v1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I'
INJECT = ANCHOR + '''

    # === ws-android injector: load BEFORE libwarspear ===
    const-string v0, "injector"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
    # === end injector ===
'''
```

Idempotência: detecta `'ws-android injector'` no arquivo, skips se já patcheado.

---

## 2. MDRenderer.smali — draw overlay per frame

### Antes (linhas 95-100)

```smali
    .line 82
    iget-object v0, p1, Lcom/aigrind/warspear/MDActivity;->mGLView:Lcom/aigrind/warspear/MDSurfaceView;

    if-eqz v0, :cond_2

    .line 82
    invoke-static {}, Lcom/aigrind/mobiledragon/Native;->onDrawFrame()V

    .line 84
    :cond_2
    monitor-exit p1
```

### Depois

```smali
    .line 82
    iget-object v0, p1, Lcom/aigrind/warspear/MDActivity;->mGLView:Lcom/aigrind/warspear/MDSurfaceView;

    if-eqz v0, :cond_2

    .line 82
    invoke-static {}, Lcom/aigrind/mobiledragon/Native;->onDrawFrame()V

    # === ws-android overlay: draw on top of the game's framebuffer ===
    invoke-static {}, Lcom/wsAndroid/Overlay;->draw()V
    # === end overlay ===

    .line 84
    :cond_2
    monitor-exit p1
```

### Por quê

- **DEPOIS de `Native.onDrawFrame()`**: o jogo renderiza seu mundo no EGL surface; em seguida nosso `Overlay.draw()` chama ImGui que desenha por cima. Sequência preservada → overlay aparece on top.
- **DENTRO do `monitor-enter p1 / monitor-exit p1`**: o jogo usa monitor pra serializar acesso ao MDActivity. Mantém a invariante.
- **DEPOIS do `if-eqz v0, :cond_2`**: só desenha quando o GLView existe. Evita chamadas com surface destruída.
- Sem `.locals` changes — nossa chamada é `static, ()V`, não consome registrador.

### Aplicado por

```python
ANCHOR = '    invoke-static {}, Lcom/aigrind/mobiledragon/Native;->onDrawFrame()V'
INJECT = ANCHOR + '''

    # === ws-android overlay: draw on top of the game's framebuffer ===
    invoke-static {}, Lcom/wsAndroid/Overlay;->draw()V
    # === end overlay ==='''
```

---

## 3. MDSurfaceView.smali — touch routing

### Antes (linhas 551-575)

```smali
.method public onTouchEvent(Landroid/view/MotionEvent;)Z
    .locals 5

    .line 102
    invoke-static {}, Lcom/aigrind/warspear/WeakActivityHolder;->get()Lcom/aigrind/warspear/MDActivity;

    move-result-object v0

    const/4 v1, 0x0

    if-nez v0, :cond_0

    return v1

    .line 108
    :cond_0
    ...
```

### Depois

```smali
.method public onTouchEvent(Landroid/view/MotionEvent;)Z
    .locals 5

    # === ws-android: let Overlay see the touch first ===
    invoke-static {p1}, Lcom/wsAndroid/Overlay;->onTouch(Landroid/view/MotionEvent;)Z
    move-result v4
    if-eqz v4, :ws_touch_pass
    const/4 v0, 0x1
    return v0
    :ws_touch_pass
    # === end overlay touch ===

    .line 102
    invoke-static {}, Lcom/aigrind/warspear/WeakActivityHolder;->get()Lcom/aigrind/warspear/MDActivity;

    move-result-object v0

    const/4 v1, 0x0

    if-nez v0, :cond_0

    return v1

    .line 108
    :cond_0
    ...
```

### Por quê

- **ANTES de tudo**: nosso `Overlay.onTouch` precisa ver o evento antes do jogo. Se ele consumir (ImGui::WantCaptureMouse=true), retornamos `true` cedo e o jogo nunca recebe o touch.
- **Uso de v4**: o método já declarava `.locals 5` (v0-v4). v4 era usado em um caminho específico mais tarde mas naquela região é livre. Não precisamos aumentar `.locals` (que requereria mais cuidado com a tabela de registradores).
- **`const/4 v0, 0x1; return v0`** retorna boolean true (1). O método retorna `Z`, então isso é a representação correta de `true`.
- **Label `:ws_touch_pass`**: usa prefixo `ws_` pra evitar colisão com qualquer label existente do jogo.

### Aplicado por

```python
ANCHOR = '.method public onTouchEvent(Landroid/view/MotionEvent;)Z\n    .locals 5'
INJECT = ANCHOR + '''

    # === ws-android: let Overlay see the touch first ===
    invoke-static {p1}, Lcom/wsAndroid/Overlay;->onTouch(Landroid/view/MotionEvent;)Z
    move-result v4
    if-eqz v4, :ws_touch_pass
    const/4 v0, 0x1
    return v0
    :ws_touch_pass
    # === end overlay touch ==='''
```

---

## 4. Overlay.smali — classe nova

Dropada em `smali/com/wsAndroid/Overlay.smali`:

```smali
.class public Lcom/wsAndroid/Overlay;
.super Ljava/lang/Object;
.source "Overlay.java"


# direct methods
.method public constructor <init>()V
    .locals 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V
    return-void
.end method

# Native per-frame draw — implemented in libinjector as
# Java_com_wsAndroid_Overlay_draw. Called by MDRenderer.onDrawFrame after
# the game has finished rendering into the EGL surface.
.method public static native draw()V
.end method

# Native touch handler — implemented in libinjector as
# Java_com_wsAndroid_Overlay_onTouch. Called by MDSurfaceView.onTouchEvent
# BEFORE the game processes the touch.
.method public static native onTouch(Landroid/view/MotionEvent;)Z
.end method
```

### Por quê

- **Classe estática**: não precisamos instâncias; tudo é estático. Faz com que `invoke-static` no patch dos outros arquivos funcione.
- **Pacote `com.wsAndroid`**: evita colisão com qualquer pacote do jogo (`com.aigrind.*`). JNI mangling produz `Java_com_wsAndroid_Overlay_draw`.
- **`<init>` vazio**: nunca chamado, mas Dalvik exige um construtor padrão pra a classe ser carregável.
- **`.method public static native`**: Dalvik resolve esses símbolos via `dlsym` no `libinjector.so`. Como nosso CMakeLists faz `-fvisibility=default` específico em `OverlayJni.cpp`, os exports ficam visíveis pra ele.

---

## Como o `apply.sh` se mantém idempotente

Cada patch detecta seu marcador antes de aplicar:

```bash
if grep -q 'ws-android injector' "$SMALI_ACT"; then
    echo "[*] MDActivity.smali already patched — skipping load injection"
else
    # ... apply patch ...
fi
```

Markers usados:
- `MDActivity.smali`: `ws-android injector`
- `MDRenderer.smali`: `com/wsAndroid/Overlay` (testa se a invoke-static já está lá)
- `MDSurfaceView.smali`: `Overlay;->onTouch`
- `Overlay.smali`: sempre re-copiado (não importa)

## Adicionando libs no APK

`apply.sh` também faz:

```bash
cp build/arm64-v8a/libinjector.so apk/extracted/lib/arm64-v8a/
cp build/armeabi-v7a/libinjector.so apk/extracted/lib/armeabi-v7a/
```

O APK do Warspear original tem só `lib/arm64-v8a/` e `lib/armeabi-v7a/` — exatamente as nossas ABIs target.

> `extractNativeLibs="false"` no AndroidManifest.xml: as `.so` ficam compactadas dentro do APK e carregadas direto sem extração pra `/data/...`. Não muda nada pra gente, mas explica por que `nativeLibraryDirectories` no logcat mostra `/data/app/.../base.apk!/lib/arm64-v8a` (pseudo-path inside zip).

## Re-aplicando após update do jogo

Quando a Aigrind soltar uma versão nova do Warspear, é provável que os arquivos `.smali` tenham mudado (refactor, novas linhas). Os ANCHORS no `apply.sh` foram escolhidos pra serem **frases relativamente estáveis** (texto de `Log.i`, nomes de método). Se um patch falhar:

1. Procurar o anchor manualmente: `grep -n "anchor_text" apk/extracted/smali/...`
2. Atualizar a constante `ANCHOR` no `apply.sh`
3. Rodar `apply.sh` de novo

A estrutura geral do jogo (classes `MDActivity`, `MDRenderer`, `MDSurfaceView`, native `mdTouch`/`onDrawFrame`/etc) faz parte do **framework MobileDragon** que a Aigrind reusa há anos — improvável que mude.
