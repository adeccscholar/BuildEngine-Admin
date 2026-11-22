# dxc 1.9.2609: native BCC64X integration

[TOC|Content]

**Status: [not verified]**. A declared source/package configuration is not a successful native build or consumer test.

Upstream is pinned in admin/build-libraries.xml. C++Builder 13 BCC64X, Ninja, installed dependency packages and the dynamic runtime are mandatory. Keep upstream features and tests where provided. Do not use unpinned network fetching or alternative MSVC/MinGW compiler pipelines.

The LLVM-derived compiler must build and run its native tools and HLSL/DXIL/SPIR-V tests; its prebuilt DLL is not a valid native compilation substitute.

Complete Release/Debug, install, metadata, license, SBOM, publish, native test/consumer and FSM evidence remains **[not verified]**.

[Library catalog](/manual/libraries.md) · [Development news](/news/2026-11-22/)


## First native run and scoped correction

LLVM GetHostTriple.cmake previously tried to execute config.guess using sh on native Windows because BCC64X is neither MSVC nor MinGW. The local source patch identifies Clang+EMBT before the shell fallback and sets the Windows LLVM target triple. DXC's LLVM build still needs native verification. **[not verified]** until BCC64X Release and Debug rerun.

## Managed Python interpreter and host triple, 22 November 2026

The complete CMake configure log identified `Could NOT find Python3 (missing: Python3_EXECUTABLE Interpreter)` at the original LLVM root `CMakeLists.txt:451`. BuildEngine already validates Python 3.14.7. The DXC package now passes `Python3_EXECUTABLE={Tool:python}` explicitly into both Release and Debug configuration. The existing BCC64X host-triple patch remains active; a version-bound follow-up normalizes the corresponding CMake `else()` and `endif()` block keywords, resolving the recorded developer warning instead of suppressing it.

Upstream LLVM/DXC targets, SPIR-V features and CTest gates stay enabled. Native configure/build/test results are **[not verified]** until rerun.


## LLVMBuild target registry mismatch in the pinned DXC release

The 22 November Release configuration log now identifies the precise failure at root CMakeLists.txt:503: `llvm-build: error: invalid target to enable: 'AMDGPU' (not in project)`. The pinned source's `lib/Target/LLVMBuild.txt` has an empty target subdirectory registry, and neither `lib/Target/AMDGPU/CMakeLists.txt` nor `lib/Target/NVPTX/CMakeLists.txt` exists in `v1.9.2609`. Its root `CMakeLists.txt` nevertheless lists AMDGPU and NVPTX as default targets. This is an upstream internal configuration inconsistency, **not** a Python interpreter failure.

The BuildEngine now passes `LLVM_TARGETS_TO_BUILD=` explicitly. The pinned `llvm-build` implementation handles the empty string as an empty target list (rather than an invalid name). This does **not** disable any backend present in this upstream archive; the named backends have no corresponding source directories. HLSL DXIL conversion, SPIR-V code generation and upstream testing options remain enabled and must be validated by the next native build.

**[not verified]**: successful configure/build, shader tests or SPIR-V runtime output.

## Managed Khronos SPIR-V source integration

The detailed CMake configure log reported that `external/SPIRV-Headers` did not contain a `CMakeLists.txt` (CMake `external/CMakeLists.txt:31`). DXC's original SPIR-V code generator calls `add_subdirectory` for both SPIRV-Headers and SPIRV-Tools and requires actual `SPIRV-Tools-static` and `SPIRV-Tools-opt` build targets. Merely passing installed include directories would not satisfy the original upstream contract.

The library now declares dependencies on managed `spirv-headers/v1.202609.0` and `spirv-tools/2026.4` and passes their versioned extracted source directories under `build/thirdparty-test`. A global `SPIRV-Headers_SOURCE_DIR` also permits the nested SPIRV-Tools CMake project to reuse the same headers. `SPIRV_TOOLS_BUILD_STATIC=ON` retains the upstream targets and `SPIRV_TOOLS_USE_MIMALLOC=OFF` avoids an unrelated undeclared allocator requirement. DXC's HLSL/DXIL/SPIR-V functionality and its original CTest suite remain enabled. The two Khronos libraries remain separately managed and versioned; DXC does not download uncontrolled copies.

The source-root path contract and the cross-version API compatibility still require native BCC64X Release/Debug evidence. **[not verified]**.
