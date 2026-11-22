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

## GoogleTest discovery during native builds

In the current full BCC64X build, both Release and Debug reached the `OpenAL_Tests` link step, but CMake 4.1.1 aborted in `GoogleTestAddTests.cmake:132`. That source line reports an unsuccessful execution of `OpenAL_Tests.exe --gtest_list_tests` during POST_BUILD discovery, not a failure to compile the tests.

The OpenAL shared library is produced from the root upstream CMake project while `OpenAL_Tests` is created in `tests/`. The configuration now sets `CMAKE_RUNTIME_OUTPUT_DIRECTORY` to the configuration-specific native build root. This colocates `OpenAL_Tests.exe` with `OpenAL32.dll` during build-time discovery. It does **not** disable GoogleTest discovery, any upstream tests, any audio backend or any utility.

The full `Result:` and `Output:` lines from `GoogleTestAddTests.cmake` were not present in the summarized FSM log. The missing-DLL interpretation is therefore **[not verified]** until those lines and the next native run are inspected. CMake `gtest_discover_tests` and the later CTest test execution remain mandatory.

## Acceptance

Audio backends, utilities and tests remain enabled. Source patch steps must be idempotent, and both native variants must compile, link, stage and pass the independent smoke. BCC64X results are **[not verified]**.

## Shared SDK publish boundary (22 November 2026)

The previously configured publish operation copied all of the installed `include` tree, all `.lib` files and all DLLs from the Release package. The installed tree also contains GoogleTest/GoogleMock development artifacts required for the upstream test build. Those artifacts belong to their own package, not to the public OpenAL consumer API. Publishing the entire tree led to a collision at `include/gmock/gmock-actions.h`.

The Schema 17 publish declaration now projects `include/AL` only, selects import-library names already searched by the OpenAL package smoke (`OpenAL32.lib`, `libOpenAL32.lib`, `OpenAL.lib`, `libopenal.lib`), and selects `OpenAL32.dll`. A required `files` selector fails if no named artifact is found. Optional CMake configuration forwarding is limited to OpenAL-prefixed configuration files. This is publication filtering, not a change to the native producer or test installation. Actual artifact-name selection remains subject to BCC64X run evidence.

The library timestamp and already completed source/build/test/install scopes were not changed. The unsuccessful publish scope must be retried by the ordinary incremental FSM path; no forced rebuild or manual state manipulation is required. Publish manifests are materialization evidence, not independent library ownership or Current-State authority.

**Verification:** [not verified]. The final acceptance gate is a BCC64X `--make` run reaching `openal-soft/1.25.2:ready` and reporting all 141 libraries finished. No successful native rerun is claimed here.
