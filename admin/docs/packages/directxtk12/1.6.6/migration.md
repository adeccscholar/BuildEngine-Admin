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
