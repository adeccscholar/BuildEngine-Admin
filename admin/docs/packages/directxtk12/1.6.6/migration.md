# directxtk12 1.6.6: native BCC64X integration

[TOC|Content]

**Status: [not verified]**. A declared source/package configuration is not a successful native build or consumer test.

Upstream is pinned in admin/build-libraries.xml. C++Builder 13 BCC64X, Ninja, installed dependency packages and the dynamic runtime are mandatory. Keep upstream features and tests where provided. Do not use unpinned network fetching or alternative MSVC/MinGW compiler pipelines.

The D3D12 toolkit requires managed DirectXMath, DirectX-Headers and native DXC shader generation.

Complete Release/Debug, install, metadata, license, SBOM, publish, native test/consumer and FSM evidence remains **[not verified]**.

[Library catalog](/manual/libraries.md) · [Development news](/news/2026-11-22/)

## BCC64X XInput and XAudio compatibility (27 November 2026)

The Windows desktop GamePad backend explicitly uses XInput (`BUILD_XINPUT=ON`, `BUILD_WGI=OFF`); the Windows.Gaming.Input header is not required. XAudio 2.9 remains enabled. The version-bound `bcc64x-audio-interlocked-long.patch` changes the two one-shot counters in `Audio/SoundEffect.cpp` and `Audio/WaveBank.cpp` to `volatile LONG`, matching Windows Interlocked intrinsic pointer types. The already version-bound DXC shader-runtime patch remains unchanged.

Build, install and consumer-smoke success for these corrections remains **[not verified]** until a native BCC64X Debug/Release run.

## DLL link completion (27 November 2026)

The native Debug and Release builds now compile all DirectXTK12 C++ translation units but previously failed to link the shared DLL. The linker reported missing XInputGetState, XInputGetCapabilities, XInputSetState, CreateFX, CreateAudioReverb, X3DAudioInitialize and X3DAudioCalculate. The version-bound `bcc64x-link-input-audio.patch` augments the upstream CMake shared target with the platform import libraries `xinput.lib` when XInput is enabled and `xaudio2.lib` when XAudio 2.9 is enabled. Neither component is disabled. The native link and subsequent consumer smoke remain **[not verified]** until the next BCC64X run.

## XAudio2 import library (27 November 2026)

The 19:04 UTC development run reached the shared-library link in both variants, but the BCC64X linker could not find `-lxaudio2`. The existing `bcc64x-link-input-audio.patch` remains ordered before the new `bcc64x-xaudio2-import.patch`. The latter creates a COFF import library with the already managed LLVM linker from a declaration of the required exports of the Windows system `xaudio2_9.dll`, then links that import library. No Microsoft SDK lib directory or additional managed tool is introduced. The generated .lib is an import library, not a statically linked XAudio implementation. Debug/Release native validation remains **[not verified]** until a new BuildEngine run.
