# Jolt Physics 5.7.0 – BCC64X migration

## Scope and status

Jolt Physics 5.7.0 is newly registered as a standalone, source-built DLL-only BCC64X integration. The managed producer and smoke consumer now use a pinned x86-64 SSE2 baseline; full native Debug/Release and production-server execution evidence remains **[not verified]**.

## Build contract

- CMake root: `<Workspace>/Build` (not repository root).
- `BUILD_SHARED_LIBS=ON`, `JPH_BUILD_SHARED_LIBS=ON`, `ENABLE_INSTALL=ON`.
- Disable IPO/LTO initially (`INTERPROCEDURAL_OPTIMIZATION=OFF`), upstream flag overrides and all-warnings-as-errors to isolate the compiler/ABI path.
- Disable unit tests, performance/hello-world targets, samples and viewer. Disable optional DX12/Vulkan/Metal compute paths; use core CPU path.
- Dynamic BCC64X runtime, install-scoped `Jolt.dll` and import library, no foreign prebuilt binaries.
- The consumer initializes the allocator and factory, registers types, then unregisters and releases the factory, using the standard CHECK/RESULT output contract.

## Acceptance and risk

Confirm Clang-specific compilation branches, shared-library export macros, Debug postfix, runtime allocations/ownership across the DLL boundary, dependency closure, license/SBOM paths and source archive SHA-256. The factory smoke does not certify deterministic simulation, SIMD portability or serialization compatibility. Any observed compile errors must be fixed centrally or in an explicit package adapter, not by changing the compiler to MSVC/MinGW.

## Cross-host reproducibility correction (26 November 2026)

The pinned Jolt 5.7.0 upstream defaults enable SSE4.1, SSE4.2, AVX, AVX2, LZCNT, TZCNT, F16C and FMA. The prior consumer hardcoded the matching optional `JPH_USE_*` feature defines, but this did not establish a portable CPU baseline. A DLL compiled with an unsupported CPU instruction can fail on a separate host; a loader failure alone does not prove an ISA mismatch.

The BuildEngine producer now explicitly disables all optional x86 ISA features listed above and AVX512, selecting Jolt's SSE2 baseline. The Release consumer removes those optional feature defines while retaining the pinned shared-library, object-layer, object-stream, renderer and profiler ABI defines. The consumer's `VerifyJoltVersionID()` remains mandatory, and the loader runs against the staged package DLL.

The runtime-staging helper still copies exactly selected package DLLs and `libc++-370.dll`, not arbitrary host PATH content. The two executables share the same output directory. A native production rerun must verify the absence of Win32 1114 or record its precise remaining dependency/initialization cause. **[not verified]**: this correction eliminates the previously observed production loader failure.
