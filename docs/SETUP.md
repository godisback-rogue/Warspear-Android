# SETUP — do zero ao primeiro logcat

Setup completo do ambiente, do host Windows à primeira linha `WS_SDK` aparecendo. Total: ~30-60 minutos dependendo da velocidade da rede.

## Topologia do ambiente usado

```
┌──────────────────────────────────────────────┐
│  Host Windows (com VirtualBox + LDPlayer)    │
│                                              │
│  ┌─────────────┐    ┌──────────────────┐    │
│  │ LDPlayer 9  │    │ VirtualBox VM    │    │
│  │ rooted      │    │ (Linux dev env)  │    │
│  │ x86_64+ARM  │◄──▶│ adb daemon       │    │
│  │ trans.      │    │ NDK, apktool     │    │
│  │ adb 5557    │    │ ws-android/      │    │
│  └─────────────┘    └──────────────────┘    │
└──────────────────────────────────────────────┘
              netsh portproxy 5555/5557
```

Pode também usar o host Windows direto sem VM (mais simples). A documentação assume o setup VM-in-host porque foi o usado em dev.

## 1. Pré-requisitos no host Windows

- Windows 10/11
- **Hypervisor desligado** (`bcdedit /set hypervisorlaunchtype off` + reboot) — necessário pro VirtualBox conseguir expor virtualização à VM (se for usar VM). Se Hyper-V/WSL2/VBS estiverem ligados, conflita com VirtualBox. Detalhes em [TROUBLESHOOTING.md#vbs](TROUBLESHOOTING.md#vbs--hypervisor-bloqueando-virtualbox).
- **LDPlayer 9** instalado de https://www.ldplayer.net/. Ao criar instância:
  - Android 9 ou 11
  - **Root: ON** (Settings → Other → Root permission)
  - **ADB: ON** (Settings → Other → ADB → "Open local connection")
- **netsh portproxy** pra expor o ADB do LDPlayer pra dentro da VM:

  ```powershell
  # Como admin, no host Windows
  netsh interface portproxy add v4tov4 listenport=5555 listenaddress=0.0.0.0 connectport=5555 connectaddress=127.0.0.1
  netsh interface portproxy add v4tov4 listenport=5557 listenaddress=0.0.0.0 connectport=5557 connectaddress=127.0.0.1
  netsh advfirewall firewall add rule name="ADB LDPlayer" dir=in action=allow protocol=TCP localport=5555
  netsh advfirewall firewall add rule name="ADB LDPlayer 5557" dir=in action=allow protocol=TCP localport=5557
  ```

  > LDPlayer normalmente expõe a primeira instância em **5555**. Instâncias adicionais usam 5557, 5559, etc. Adicione regra correspondente.

## 2. VM (Linux) — instalar SDK e dependências

Dentro da VM (Linux Mint/Ubuntu/Debian):

```bash
# Java 21 (usually already there in 24.04+)
sudo apt install -y openjdk-21-jdk wget unzip

# Verificar
java -version

# Diretório do SDK
mkdir -p ~/Android/Sdk
cd ~/Android/Sdk

# Command-line tools (Linux)
wget https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip
unzip commandlinetools-linux-11076708_latest.zip
mkdir -p cmdline-tools/latest
mv cmdline-tools/{bin,lib,NOTICE.txt,source.properties} cmdline-tools/latest/
rm commandlinetools-linux-11076708_latest.zip

# Adicionar ao PATH (substitua o ws-android tools/env.sh por isso se quiser)
export ANDROID_HOME=$HOME/Android/Sdk
export PATH=$ANDROID_HOME/cmdline-tools/latest/bin:$PATH

# Aceitar licenças
yes | sdkmanager --licenses
```

## 3. SDK packages (manual download — opcional)

Se a conexão na VM for lenta, pode baixar no host e dropar via pasta compartilhada. URLs:

```
https://dl.google.com/android/repository/platform-tools_r35.0.2-linux.zip      (7 MB)
https://dl.google.com/android/repository/build-tools_r33.0.2-linux.zip         (53 MB)
https://dl.google.com/android/repository/android-ndk-r26b-linux.zip            (638 MB)
```

Caminhos onde extrair (em `~/Android/Sdk/`):
- `platform-tools_r35.0.2-linux.zip` → unzip direto na raiz, vira `~/Android/Sdk/platform-tools/`
- `build-tools_r33.0.2-linux.zip` → unzip, depois `mv android-13 build-tools/33.0.2`
- `android-ndk-r26b-linux.zip` → unzip, depois `mv android-ndk-r26b ndk/26.1.10909125`

Ou via `sdkmanager` (mais lento se a conexão da VM for ruim):

```bash
yes | sdkmanager "platform-tools" "build-tools;33.0.2" "ndk;26.1.10909125"
```

## 4. Clone do projeto

```bash
cd ~/Desktop
git clone https://github.com/lucas-freitas00/ws-hack-android.git ws-android
cd ws-android
source tools/env.sh           # exporta ANDROID_HOME, ANDROID_NDK_ROOT, PATH

# Baixa as dependências de terceiros (ImGui, xhook, shadowhook)
tools/setup_third_party.sh
```

## 5. APK do jogo

```bash
# Baixe direto do site oficial (versão 13.3.0)
mkdir -p apk
wget -O apk/warspear-13.3.0.apk \
  https://distr.warspear-online.com/distr/13.3.0/android/base/warspear.apk
```

Tamanho ~278 MB. URLs alternativas (mesmo conteúdo, variantes de assets):
- `base_no_gfx/warspear.apk` (~251 MB) — sem texturas HD
- `base_old/warspear.apk` (~251 MB) — compat legado

Todas têm exatamente o mesmo `libwarspear.so`, só mudam os assets gráficos.

## 6. Conectar ADB do LDPlayer

```bash
tools/bs_connect.sh
```

Script scaneia a LAN procurando IPs com port 5555/5557 abertos e roda `adb connect`. Deve mostrar:

```
[*] Found Bluestacks ADB at 192.168.1.100:5557
connected to 192.168.1.100:5557

=== device info ===
Model:     SM-N960N
Android:   9
ABI:       x86_64
ABIList:   x86_64,x86,arm64-v8a,armeabi-v7a,armeabi
```

> Se não achar: confirme que LDPlayer está rodando, ADB está habilitado nas settings, e o `netsh portproxy` no host Windows foi aplicado. Detalhes em [TROUBLESHOOTING.md](TROUBLESHOOTING.md).

## 7. Mascarar `/proc/cpuinfo`

Em **qualquer emulador x86 (LDPlayer, Bluestacks, MEmu, NoxPlayer, MuMu)**, o `libwarspear.so` detecta CPUs Intel/AMD via `/proc/cpuinfo` e se auto-encerra (~1s após start). Solução: bind-mount um cpuinfo falso (ARM Cortex-A78).

```bash
tools/mask_cpuinfo.sh
```

Verifica:
```bash
adb shell head -3 /proc/cpuinfo
# Deve mostrar: BogoMIPS: 38.40 / Features: fp asimd ... / CPU implementer: 0x41 ...
```

> Precisa root no device. Em LDPlayer, é o toggle "Root permission" das settings. **Persiste até a instância ser reiniciada** — re-rodar `mask_cpuinfo.sh` após cada restart do LDPlayer.

## 8. Build da `libinjector.so`

```bash
mkdir -p build/arm64-v8a build/armeabi-v7a
for ABI in arm64-v8a armeabi-v7a; do
  cd build/$ABI
  cmake ../../android-injector \
      -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI=$ABI \
      -DANDROID_PLATFORM=android-23 \
      -DCMAKE_BUILD_TYPE=Release
  cmake --build . -j
  cd ../..
done
ls -lh build/*/libinjector.so   # ~1 MB cada
```

## 9. Apktool + uber-apk-signer

```bash
tools/setup_apk_tools.sh   # baixa apktool 2.12.0 + uber-apk-signer 1.3.0
```

## 10. Unpack + patch + repack + sign + install

```bash
apk/unpack.sh apk/warspear-13.3.0.apk    # apktool decompile → apk/extracted/
apk/repack.sh                            # patches/apply.sh + apktool build + signing
                                          # output: apk/repacked/signed/*-debugSigned.apk
tools/install_apk.sh apk/repacked/signed/warspear-modded-aligned-debugSigned.apk
```

## 11. Lançar e verificar

```bash
adb logcat -c                            # limpa
adb shell am start -n com.aigrind.warspear/.LoaderActivity
adb logcat -s WS_SDK                     # deve mostrar:
                                          #   I WS_SDK: === ws-android injector loaded ===
                                          #   I WS_SDK: overlay: ImGui initialized
                                          #   I WS_SDK: overlay: draw() call #1
```

Pra ver visualmente: `adb exec-out screencap -p > /tmp/shot.png`. A janela aparece no canto superior esquerdo.

## Próximos passos

- Logar in-game e clicar "Scan Heap Now" pra preencher Name/Level/HP/MP reais
- Ler [ARCHITECTURE.md](ARCHITECTURE.md) pra entender o fluxo
- Modificar o overlay em `android-injector/src/overlay/Overlay.cpp` → rebuild → `apk/repack.sh` → instalar

## Ciclo dev rápido

Depois do setup inicial, o loop é:

```bash
( cd build/arm64-v8a && cmake --build . -j )    # ~5s
apk/repack.sh                                    # ~10s
tools/install_apk.sh apk/repacked/signed/*.apk   # ~15s
adb shell am force-stop com.aigrind.warspear
adb shell am start -n com.aigrind.warspear/.LoaderActivity
adb logcat -s WS_SDK
```

Total: ~30s por iteração.
