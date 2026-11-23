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

## Native Ninja TAEF discovery

The next BCC64X configure progressed past the managed SPIR-V sources and reached `cmake/modules/FindTAEF.cmake:57`. The pinned upstream module assumes `CMAKE_C_COMPILER_ARCHITECTURE_ID` or `CMAKE_GENERATOR_PLATFORM` is populated by an MSVC/Visual Studio generator; neither is necessarily set with BCC64X + Ninja.

A version-bound source patch recognizes native Win64 using `WIN32`, `CMAKE_SIZEOF_VOID_P=8`, and the empty compiler architecture ID. It selects the original x64 TAEF header/library directories and amd64 runner path. It does not redefine compiler identity, skip Windows tests, or substitute a different test framework.

**External dependency:** Windows HLSL unit tests actually use Microsoft's TAEF, supplied by the Windows Driver Kit or a compatible provisioned TAEF package. Upstream `find_package(TAEF REQUIRED)` and its original tests remain intact. If the local WDK/TAEF header, libraries or `te.exe` are absent, configure must still fail with the missing prerequisite rather than falsely reporting success. A further local trace is required to identify any such missing file. Microsoft documents the test dependency in `docs/BuildingAndTestingDXC.rst`.

**[not verified]**: post-patch configure, native test build and test execution.

## TAEF runtime binary is a separately required prerequisite

The 22 November 2026 Release configure log reached `FindTAEF.cmake:80` and reported `Unable to find TAEF binaries.` The native x64 architecture detection patch is effective: discovery now reaches the `find_program(TAEF_EXECUTABLE te.exe ...)` check rather than failing architecture identification. The upstream caller is `projects/dxilconv/unittests/CMakeLists.txt:4`.

This is a missing native Microsoft TAEF test runtime, not a C++ compiler failure or a reason to disable tests. The actual TAEF header `Wex.Common.h`, x64 import libraries `Te.Common.lib`, `Wex.Common.lib` and `Wex.Logger.lib`, and an x64 `te.exe` must be available from an official compatible Microsoft SDK/WDK provision. No package version or source archive has yet been verified for reproducible BuildEngine provisioning, therefore **no unverified TAEF package has been invented or declared ready**.

## TAEF-to-GoogleTest native test migration (22 November 2026)

**Status: [not verified].** The pinned Microsoft DXC v1.9.2609 TAEF discovery and runner dependency is removed from the active BuildEngine source contract. This is an authored migration, **not** yet evidence that the native Release/Debug builds or all original test cases pass.

The active version-bound source action runs `admin/cmake/dxc/MigrateHLSLGoogleTest.cmake` and then `gtest/MigrateDXCOriginalTests.py` with the managed Python interpreter. The migration uses original test translation units and methods rather than substituting only smoke tests:

- `ClangHLSLTests`: original Windows HLSL sources, GoogleTest driver, GoogleMock and CTest.
- `HLSLTestLib`: existing original helper sources, GoogleTest headers instead of `find_package(TAEF REQUIRED)`.
- `dxilconv-tests`: the six original batch/regression groups compiled as a GoogleTest executable. The original manual test remains explicitly disabled. The independent portable DXIL/SPIR-V smoke checks remain additive.
- `ExecHLSLTests`: original native Windows D3D12 execution, long-vector and linear-algebra sources, including dynamic test method registration. Both the original source-level GPU capability checks and failures remain in place.
- `ShaderOpArithTable.xml` and other pinned original execution XML test tables: generated standard C++ typed data access and per-row CTest test names. Original parameter values are extracted directly from XML, not replaced with dummy values.
- Existing historical TAEF LLVM/LIT suite configurations are adapted to discover and execute the corresponding GoogleTest binaries, while parameterized XML rows are owned by CTest to prevent incorrect unparameterized repetition.

The old native-TAEF architecture patch and the old DXIL converter patch that **replaced** original regression tests with two smoke cases are not applied in the current source contract. No `te.exe`, TAEF headers or TAEF import libraries should be required by the migrated test CMake targets.

**Native acceptance gates**: original BCC64X Release and Debug configure/build, all generated GoogleTest/CTest targets, LLVM/LIT discovery, D3D12 execution capability reporting, XML row/value count comparison, independently executed original six DXIL converter groups, actual DLL/COFF import-library runtime validation, and zero active TAEF lookups. If any original test cannot be compiled or executed, retain an explicit INCOMPLETE/FAIL state rather than counting it as a pass.


## Native Windows macro isolation contract (23 November 2026)

**Status: [not verified]** until a full BCC64X Debug/Release build and consumer test.

Windows SDK names in this LLVM-derived source tree have two distinct collision classes:

1. **A/W API aliases** (for example `CreateFile`, `GetModuleFileName`, `ReplaceText`): use the concrete Win32 `...W` / `...A` entry point only for actual Windows API invocations, matching the pointer encoding and signature. Do not let an SDK macro silently rewrite a Clang/LLVM class method such as `Rewriter::ReplaceText`. Undefine that alias locally where the C++ method is declared or defined.
2. **PE/COFF constants** (`IMAGE_...`): the SDK defines preprocessor macros with the same spelling as `llvm::COFF` enumeration members. The C++ namespace qualifier does not suppress macro expansion. Keep the LLVM enum declarations ahead of SDK macros where required, and undefine only the conflicting macro names at the narrowest stable integration boundary before parsing or using qualified LLVM identifiers. In LLVM code, use `llvm::COFF::...`; in actual Windows API code, use explicitly named Win32 functions and suitably typed SDK constants.

Never apply broad `#undef` directives to the entire project via compiler flags, never rename upstream LLVM enum members, and never disable an upstream feature to evade a macro collision. An include-order-only fix cannot prevent later macro expansion in `llvm::COFF::...` expressions.

The version-bound `bcc64x-win32-symbol-collisions.patch` currently isolates `ReplaceText` in `Rewriter.cpp` and the COFF macros in `ObjectFilePCHContainerOperations.cpp`. The existing `bcc64x-clang-attr-interface-macro.patch` uses a scoped push/undef/pop around generated Clang attribute declarations. The required acceptance gate is native Debug/Release compilation and verification that real Win32 call sites still reference the correct `A`/`W` exports. Do not claim that all transitive header collisions have been eliminated without that evidence.
