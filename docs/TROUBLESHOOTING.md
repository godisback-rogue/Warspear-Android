# TROUBLESHOOTING

Problemas comuns e soluções, baseado em real debugging durante a setup.

## ADB / Conexão

### `adb connect 192.168.1.100:5555` retorna "failed to connect"

**Diagnóstico**: 
```bash
timeout 1 bash -c "</dev/tcp/192.168.1.100/5555" && echo OPEN || echo CLOSED
```

| Resultado | Significado | Solução |
|---|---|---|
| CLOSED | `netsh portproxy` não configurado no host Windows | Aplicar regras do [SETUP.md §1](SETUP.md#1-pré-requisitos-no-host-windows) |
| OPEN mas adb falha | Algo escutando 127.0.0.1:5555 no host **não é ADB** | Ver "porta ocupada por outro processo" abaixo |

### Porta 5555 ocupada por outro processo (BlueStacks/orfão)

Sintoma: `adb connect` retorna sucesso, mas `adb devices` mostra "offline". TCP handshake funciona, mas protocolo ADB nem inicia.

No host Windows (PowerShell admin):
```powershell
netstat -ano | findstr ":5555"
# Pega o PID da última coluna
Get-Process -Id <PID>
# Se for HD-Player/BlueStacks/etc, mate:
Stop-Process -Id <PID> -Force
```

Depois reinicie a instância LDPlayer.

### `adb devices` mostra `offline`

Mesmo padrão acima. Ver primeiro se `netstat` mostra a porta apontando pro processo LDPlayer correto (`dnplayer.exe` ou `Ld9BoxHeadless`). Se aponta pra outro processo, mate-o ou reconfigure o LDPlayer.

### ADB do próprio LDPlayer não conecta no localhost

Falha sintomática: dentro do Windows, `C:\LDPlayer\LDPlayer9\adb.exe connect 127.0.0.1:5555` falha. Significa o **LDPlayer não está expondo TCP**.

Solução:
1. Settings da instância → **Other** → ADB → escolher **"Open local connection"**
2. Save → reiniciar a instância (`ldconsole.exe quit --index 0` + `ldconsole.exe launch --index 0`)

### Múltiplas entradas "offline" + uma "device" em `adb devices -l`

```
192.168.1.100:5554       offline transport_id:1
192.168.1.100:5555       offline transport_id:2
192.168.1.100:5557       device  product:SM-N960N
```

Limpar as offline:
```bash
adb disconnect 192.168.1.100:5554
adb disconnect 192.168.1.100:5555
```

Scripts (`mask_cpuinfo.sh`) que usam `adb shell` sem `-s` falham quando há múltiplas devices listadas (mesmo offline). Sempre limpar antes.

---

## VBS / Hypervisor bloqueando VirtualBox

### Sintoma

Dentro da VM, `/dev/kvm` não existe. `cat /proc/cpuinfo | grep -E 'vmx|svm'` retorna vazio. `kvm-ok` diz "Your CPU does not support KVM extensions" mesmo num Ryzen moderno.

### Causa

Windows host com **Hyper-V**, **VBS** (Virtualization-Based Security), **WSL2** ou **Memory Integrity** ativos. Esses sistemas pegam controle exclusivo das extensions de virtualização do CPU; VirtualBox não consegue mais expor pra guest.

### Diagnóstico no host Windows

```powershell
# Hypervisor launchtype no boot
bcdedit /enum {current} | findstr hypervisorlaunchtype
# Esperado pra VBox: "hypervisorlaunchtype Off"

# VBS status
Get-CimInstance -ClassName Win32_DeviceGuard -Namespace root\Microsoft\Windows\DeviceGuard `
  | Select-Object VirtualizationBasedSecurityStatus, SecurityServicesRunning

# Optional features
Get-WindowsOptionalFeature -Online `
  | Where-Object {$_.FeatureName -like "*Hyper*" -or $_.FeatureName -like "*Virtual*"} `
  | Where-Object {$_.State -eq "Enabled"}
```

`VirtualizationBasedSecurityStatus` = 2 significa "Running" — bloqueia VBox.

### Solução

```powershell
# 1. Desligar hypervisor launch
bcdedit /set hypervisorlaunchtype off

# 2. Desligar Core Isolation/Memory Integrity:
#    Win Security → Device Security → Core Isolation → Memory Integrity: OFF

# 3. Reboot do Windows
```

> Custo: WSL2 e Docker Desktop **não funcionam** com hypervisor desligado. Se você usa esses, considere usar **ws-android sem VirtualBox** — rode tudo direto no host Windows com WSL2 ou Cygwin/MSYS2.

### Alternativa: rodar tudo no LDPlayer sem VM

Se Hyper-V é crítico no seu Windows, skip o VirtualBox. LDPlayer roda direto no host. Pra ter um dev env Linux pra build de NDK, use **WSL2** (que coexiste com Hyper-V). A única coisa que muda é o caminho de instalação do SDK e como você acessa o ADB (via 127.0.0.1 direto, sem portproxy).

---

## Houdini / Anti-emulator

### App abre e fecha em ~1s

Logcat mostra padrão:
```
I Warspear: onCreate
I Warspear: + AppData::Init
I Warspear: mdPause
I Warspear: onStop
I Warspear: onDestroy
```

**Causa**: `libwarspear.so` chamou `MDActivity.isEmulator` via JNI **OU** leu `/proc/cpuinfo` e bateu contra strings "bluestack"/"AuthenticAMD". Auto-suicide.

**Solução**: rodar `tools/mask_cpuinfo.sh` (precisa root no device). Persiste até reboot da instância.

Verificar:
```bash
adb shell head -3 /proc/cpuinfo
# Deve mostrar BogoMIPS 38.40, Features fp asimd ... — não "vendor_id AuthenticAMD"
```

Se sair "AuthenticAMD" ou "GenuineIntel", o bind-mount não tá ativo. Re-rodar `mask_cpuinfo.sh`.

### Crashpad: "has unexpected e_machine: 183 (EM_AARCH64)"

Sintoma:
```
java.lang.UnsatisfiedLinkError: dlopen failed: ".../libcrashlytics-handler.so" has unexpected e_machine: 183 (EM_AARCH64)
```

**Causa**: Firebase Crashlytics tenta carregar seu .so ARM64 via `app_process64` (não via Houdini) — o linker x86_64 nativo rejeita e64=183.

**Quando aparece**: depois que o app inicial crasha, o Crashpad tenta inicializar pra mandar o crash report → o próprio Crashpad também crasha por causa do ABI mismatch. Loop.

**Solução real**: corrigir o crash inicial (que é a causa primária — normalmente Houdini SIGSEGV/SIGILL do inline hook). O Crashpad é só consequência.

Pra suprimir Crashpad enquanto debug:
```bash
adb shell pm clear com.aigrind.warspear   # clears app data
```

### `libwarspear.so doesn't import eglSwapBuffers`

`readelf --dyn-syms` no .so do APK mostra só `eglGetDisplay`/`eglGetConfigs`/`eglGetConfigAttrib`. Confirma que o rendering vem da camada Java GLSurfaceView → MDRenderer chama Native.onDrawFrame, e GLSurfaceView faz `eglSwapBuffers` por conta própria após.

**Implicação**: PLT hook em `eglSwapBuffers` (estilo `xhook`) **não pega nada** em libwarspear. Tem que ir pelo smali (M2's path).

---

## APK / repack

### `classes4.dex` vazio (0 bytes) no APK final

Sintoma: rebuilds via apktool produzem APK válido mas runtime crasha com:
```
java.lang.ClassNotFoundException: Didn't find class "com.google.firebase.provider.FirebaseInitProvider"
```

Verificar:
```bash
unzip -l apk/repacked/signed/*-debugSigned.apk | grep \.dex$
# classes.dex, classes2.dex, classes3.dex e classes4.dex devem TODOS ter > 0 bytes
```

**Causa**: cache do apktool corrompido. Algumas vezes acontece após patches.

**Solução**:
```bash
rm -rf ~/.local/share/apktool ~/apktool_cache 2>/dev/null
rm -rf apk/extracted apk/repacked
apk/unpack.sh apk/warspear-13.3.0.apk   # fresh
apk/repack.sh
```

### `uber-apk-signer` printa help em vez de assinar

Sintoma: chamada `java -jar uber-apk-signer.jar -a <apk> -o <out> --overwrite --allowResign` retorna a tela de help, não processa nada.

**Causa**: combinação de args inválidos (algumas flags antigas mudaram). 

**Solução**: usar a forma mínima:
```bash
java -jar tools/uber-apk-signer.jar --apks <apk> --out <out_dir>
```

Sem `--overwrite`, sem `--allowResign`. O output vai pra `<out_dir>/<apk_name>-aligned-debugSigned.apk`.

### `aapt2: error: ... extension '.9.png' not allowed in res/drawable-...`

Sintoma raro: build falha por NinePatch reconhecimento. Geralmente é flag legado.

**Solução**:
```bash
java -jar tools/apktool.jar b -p ~/apktool_framework -o output.apk apk/extracted
```

Ou em último caso, `apktool d --no-res` (sem recursos) — só funciona se você não modificar resources.

### Instalação retorna "Failure [INSTALL_FAILED_VERSION_DOWNGRADE]"

Sintoma:
```
adb install -r apk/repacked/signed/*-debugSigned.apk
Failure [INSTALL_FAILED_VERSION_DOWNGRADE]
```

**Causa**: você modificou o `versionCode` ou tem outra versão maior instalada.

**Solução**:
```bash
adb uninstall com.aigrind.warspear
adb install -r apk/repacked/signed/*-debugSigned.apk
```

---

## Build / NDK

### `error: use of undeclared identifier 'errno'`

C++ não trag `errno` automaticamente — incluir `<cerrno>`.

```cpp
#include <cerrno>   // não confuso com <errno.h>
```

### `Cannot determine link language for target xhook`

Sintoma: `project(... CXX)` mas o target tem `.c` files → CMake confuso.

**Solução**: declarar a lib como `C`:
```cmake
project(ws_injector_android C CXX)   # ou C CXX ASM se tiver .S
```

### JNI export não encontrado em runtime

Sintoma logcat:
```
java.lang.UnsatisfiedLinkError: No implementation found for void com.wsAndroid.Overlay.draw()
```

**Causa**: o símbolo `Java_com_wsAndroid_Overlay_draw` em `libinjector.so` tem visibilidade `hidden` por causa do `-fvisibility=hidden`.

**Solução** (no CMakeLists):
```cmake
set_source_files_properties(src/jni/OverlayJni.cpp PROPERTIES
    COMPILE_FLAGS "-fvisibility=default")
```

Ou usar `__attribute__((visibility("default")))` no header da função.

Verificar:
```bash
nm -D --defined-only build/arm64-v8a/libinjector.so | grep wsAndroid
# Deve mostrar: 000... T Java_com_wsAndroid_Overlay_draw
```

`T` (text/code) significa visível. `t` minúsculo é hidden.

---

## Runtime

### SIGSEGV/SIGILL durante heap scan

Sintoma: tombstone aponta pra `libhoudini.so` ou pra função dentro do nosso scan.

**Causa**: leitura direta de endereço inválido (guard pages no meio de regiões anonymous rw-p).

**Solução**: usar `process_vm_readv` em vez de `*ptr`:
```cpp
struct iovec local  { buf, len };
struct iovec remote { (void*)addr, len };
ssize_t n = process_vm_readv(getpid(), &local, 1, &remote, 1, 0);
// n < 0 ou != len → endereço inválido, pular sem crashar
```

### Player Container não encontrado pelo heap scan

Sintoma: scan acha 5-10 candidatos mas todos com valores estranhos (1600x900 = display size, 65536 = default ints, etc).

**Causa**: o user está na tela de login/loading, não in-game. O Container só é alocado quando o jogador está controlando um personagem in-world.

**Solução**:
1. Logar
2. Selecionar personagem
3. Esperar carregar in-game (mapa visível, HUD presente)
4. Clicar "Scan Heap Now" no overlay (ou esperar o auto-scan no frame 180)

Se mesmo in-game vier garbage, ajustar heurísticas em `HeapScan.cpp` (range de HP, score function).

### Touch não funciona / janela não move

Sintoma: janela renderiza, mas tap nela não arrasta nem minimiza.

**Diagnóstico**:
```bash
adb logcat -d | grep MotionEvent
```
Deve mostrar `OverlayJni: MotionEvent JNI ids cached` na primeira interação. Se não:
- Smali patch em `MDSurfaceView.onTouchEvent` não foi aplicado. Verificar `grep 'Overlay;->onTouch' apk/extracted/smali/com/aigrind/warspear/MDSurfaceView.smali`.
- Reaplicar `apk/patches/apply.sh`.

Se o tap mostra "onTouch call" mas a janela não responde:
- ImGui pode não estar capturando o tap (touch caiu fora da janela). Mire bem no header.
- DPI scaling: o `style.ScaleAllSizes(1.6f)` torna a janela maior. Em telas pequenas pode ficar mal proporcionado.

---

## Performance

### Frame rate cai pra <5 FPS

**Causa comum 1**: heap scan rodando enquanto você navega. O scan é single-shot (~1s) mas pode causar stutters.

**Causa comum 2**: ImGui::ShowDemoWindow() habilitado.

**Causa comum 3**: `ScaleAllSizes(1.6f)` + janela com muitas widgets em altos DPI.

**Solução**: scan em thread separada (já está), DPI menor (`ScaleAllSizes(1.0f)`), evitar update de UI 60fps em widgets caros.

---

## Quando tudo mais falha

1. Logcat full: `adb logcat -d > /tmp/full.log`
2. Tombstone: `adb shell ls /data/tombstones/` (precisa root)
3. Process info: `adb shell ps -A | grep aigrind`
4. Filesystem: `adb shell ls /data/app/com.aigrind.warspear*/lib/arm64/`

Se o problema é Houdini-related, a saída é mover pra device ARM real ou aceitar a limitação (não usar inline hooks).
