# OpenAL Soft 1.25.2: repeatable Windows Unicode bridge
[TOC|Content]

**Target:** C++Builder 13 / BCC64X / Windows x64.
**Status:** source and consumer contract corrected; native rebuild **[not verified]**.

## Purpose

OpenAL Soft is a shared audio API implementation. The BuildEngine contract retains the Windows audio backends, ALSOFT_UTILS=ON and configured upstream tests. The independent C++23 consumer calls the genuine OpenAL C API without a physical audio device.

## Incident and correction

The openal-info patch could not be applied or reverse-checked against the existing source (CMakeLists.txt line 2045). Repeatedly applying a context-fragile patch does not establish the effective source content.

The version-bound CMake helper verifies the exact upstream `openal-info` source registration and link anchors, then adds the BCC64X entry-point bridge to upstream `wmain` and the required shell32 library only if absent. Re-running it verifies the effective source without editing it. Unexpected upstream content is fatal.

The existing WASAPI channel-mask and Windows SDK patches are processed through the central verified ApplyManagedPatch.cmake helper. The consumer now uses the configuration-specific import library, stages the managed runtime closure and produces both CHECK and RESULT records.

## Acceptance

Audio backends, utilities and tests remain enabled. Source patch steps must be idempotent, and both native variants must compile, link, stage and pass the independent smoke. BCC64X results are **[not verified]**.
