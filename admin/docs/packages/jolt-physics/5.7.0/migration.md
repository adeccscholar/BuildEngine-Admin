# Jolt Physics 5.7.0 – BCC64X migration

## Scope and status

Jolt Physics 5.7.0 is newly registered as a standalone, source-built DLL-only BCC64X integration. Native Win64x Debug/Release, runtime dependency and consumer smoke evidence are pending.

## Build contract

- CMake root: `<Workspace>/Build` (not repository root).
- `BUILD_SHARED_LIBS=ON`, `JPH_BUILD_SHARED_LIBS=ON`, `ENABLE_INSTALL=ON`.
- Disable IPO/LTO initially (`INTERPROCEDURAL_OPTIMIZATION=OFF`), upstream flag overrides and all-warnings-as-errors to isolate the compiler/ABI path.
- Disable unit tests, performance/hello-world targets, samples and viewer. Disable optional DX12/Vulkan/Metal compute paths; use core CPU path.
- Dynamic BCC64X runtime, install-scoped `Jolt.dll` and import library, no foreign prebuilt binaries.
- The consumer initializes the allocator and factory, registers types, then unregisters and releases the factory, using the standard CHECK/RESULT output contract.

## Acceptance and risk

Confirm Clang-specific compilation branches, shared-library export macros, Debug postfix, runtime allocations/ownership across the DLL boundary, dependency closure, license/SBOM paths and source archive SHA-256. The factory smoke does not certify deterministic simulation, SIMD portability or serialization compatibility. Any observed compile errors must be fixed centrally or in an explicit package adapter, not by changing the compiler to MSVC/MinGW.
