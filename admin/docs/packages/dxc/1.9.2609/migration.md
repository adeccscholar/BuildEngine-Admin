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


## Native ATL contract audit, 23 November 2026

**Status: [not verified]** until BCC64X Debug and Release builds and the upstream DXC consumers/tests pass.

The native BCC64X adaptation lives in `admin/compat/bcc64x/dxc-native-atl.h`. Its concrete classes now use the explicit `AdeccDxc` prefix. Historic ATL names are source-compatibility aliases in the same header, not claims of Microsoft ATL identity or ABI equivalence.

The interface audit compared the actual native DXC uses with its upstream `include/dxc/WinAdapter.h` adapter and a source search for ATL usage. The audited contracts are:

| BuildEngine implementation | Source-compatible name | Audited functionality |
| --- | --- | --- |
| `AdeccDxcCComPtr<T>` | `CComPtr<T>` | AddRef/Release ownership, copy/move, Attach/Detach, QueryInterface, CopyTo, Get/GetAddressOf, overloaded address-of, Swap and COM identity |
| `AdeccDxcCHeapPtr<T>` | `CHeapPtr<T>` and `CComHeapPtr<T>` | CoTaskMem allocation/reallocation, Attach/Detach, transfer and cleanup |
| `AdeccDxcCSimpleArray<T>` | `CSimpleArray<T>` | Add, GetSize, GetData, RemoveAll, indexing and ownership of stored COM pointers |
| `AdeccDxcCComBSTR` | `CComBSTR` | SysAllocString/SysFreeString ownership, move, Attach/Detach, Copy, Length, Empty, Append (UTF-8/UTF-16 and counted strings), indexing, `m_str`, COM output address |
| `AdeccDxcCA2W`, `AdeccDxcCW2A` | `CA2W`, `CW2A` | Explicit UTF-8/UTF-16 Windows conversion, pointer conversion and upstream `m_psz` compatibility |
| `AdeccDxcCHandle`, `AdeccDxcCAllocator` | `CHandle`, `CAllocator` | CloseHandle and CoTaskMem allocation contracts |

An overloaded `operator&` is reserved for COM output parameters; self-comparison and other object address operations must use `std::addressof`. UTF conversion buffer aliases are non-owning; the wrappers are neither copyable nor movable, preventing aliases to invalidated internal storage. `CComBSTR` remains move-only, and `Copy` creates a distinct BSTR. Compile-time `static_assert` checks cover signatures, ownership traits, conversion types and indexed container access.

The build log `BuildLog_20261123_165510_295.zip` independently identified (Debug and Release): missing `CComBSTR::Detach`, `std::ofstream(std::wstring)` not supported by the BCC64X libc++ implementation and the C++17 removal of `std::const_mem_fun_t`. The source-scoped patches now use `std::filesystem::path` for Unicode output and `std::mem_fn` for the Clang static analyzer; neither feature is disabled.

**Remaining acceptance requirements:** real BCC64X header compilation, COM reference-count and BSTR memory-ownership runtime checks, embedded-null BSTR append/copy, Unicode filenames, all DXC original tests and Release/Debug DLL consumers. The compile-time checks do not substitute for runtime proof, and the helper is deliberately limited to DXC's observed ATL surface rather than claiming a complete replacement of Microsoft ATL.

## Native BCC64X linker follow-up, 23 November 2026

**Status: [not verified].** BuildLog_20261123_175931_777.zip shows Release/Debug link failures of dxbc2dxil, opt and dxcompiler:

- `dxbc2dxil.exe`: `WinMain` unresolved with upstream `wmain`. The native Unicode adapter forwards the Windows command line to the unchanged original entry point.
- `dxcompiler.dll`: `Rewriter::ReplaceTextA` unresolved due to a Windows macro. Isolate `ReplaceText` before the class declarations in `Rewriter.h`, not only the implementation.
- `dxcompiler.dll`: `LangOptionsBase` members unresolved. Their out-of-class definitions in `LangOptions.cpp` are currently guarded by `LLVM_ON_UNIX`; extend this to native Windows Clang/BCC64X.
- `opt.exe`: duplicate LLVM `.weak` ABI-check symbols remain open; preserve real link-time ABI diagnostics.
- `dxcompiler.dll`: `CreateDxcDiaDataSource` remains unresolved; determine the true producer and target linkage instead of inserting a stub.

The first three changes are authored in `bcc64x-linkage-compat.patch` and `dxc-wmain.cpp`, with the DXC source XML actions and timestamp updated. Successful BCC64X builds and runtime tests remain **[not verified]**.

## Full native DIA/PIX implementation restored, 23 November 2026

**Status: [not verified]** until native BCC64X Debug/Release compilation, linking and runtime verification.

The `CreateDxcDiaDataSource` undefined reference was traced to an upstream CMake source-selection error, **not** to a missing Windows API or an unavailable vendor runtime DLL. Its real implementation exists in `lib/DxilDia/DxilDiaDataSource.cpp` at the top-level function `HRESULT CreateDxcDiaDataSource(REFIID, LPVOID *)`. `tools/clang/tools/dxcompiler/dxcapi.cpp` calls it for `CLSID_DxcDiaDataSource`; `dxcompiler` already links `LLVMDxilDia`.

The pinned `lib/DxilDia/CMakeLists.txt` formerly selected **all** original DIA and PIX implementation translation units only for `MSVC`. Its non-MSVC path built **only** `DxcPixLiveVariables_FragmentIterator.cpp`, thus creating a nearly empty `LLVMDxilDia` archive for BCC64X despite the real native Windows COM and DIA dependencies. This would also omit other original DIA and PIX entrypoints, not merely the one visible unresolved symbol.

The new version-bound `bcc64x-native-dia-implementation.patch` selects the full original source list when `MSVC OR (WIN32 AND ADECC_BCC64X_NATIVE_X64)` and retains the restricted fallback on other platforms. Native Windows BCC64X requires the real Microsoft DIA SDK `dia2.h`; the patch resolves its include directory from the same candidate paths as the existing `dxcompiler` setup and fails explicitly if unavailable. The original MSVC path, DIA and PIX sources, and the native COM ABI remain intact. No stubs, disabled methods or alternate compiler builds are introduced.

The new patch is registered as an idempotent source action in `admin/build-libraries.xml` and the `dxc/1.9.2609` library timestamp is advanced. Its three unified-diff hunks were checked against the complete pinned upstream CMake file and found to match. The remaining native acceptance gates are compile and link of all included DIA and PIX translation units, correct `LLVMDxilDia` linkage, release/debug `dxcompiler.dll`, `CLSID_DxcDiaDataSource` creation, and actual PDB / PIX consumer tests.

## Early patch syntax preflight

The DXC source stage now calls the reusable `admin/cmake/packages/PreflightSourcePatches.cmake` immediately after extraction and **before the first patch action**. It recursively enumerates every `*.patch` within `admin/patches/dxc/1.9.2609`, including the GoogleTest patches, then runs `git apply --numstat --no-index` for each. Git parses the complete unified diff and rejects malformed hunk counts, truncated patches and corrupt syntax without modifying upstream sources. An empty patch is also rejected. Any failure terminates the source stage before expensive compilation.

This early check is **syntax validation only**: source-context and patch-order compatibility still require `git apply --check` against the actual workspace, as already enforced by `ApplyIdempotentSourcePatch.cmake` immediately before each apply. Patches following generated-source transformations are necessarily checked at their actual application point. Never claim an entire source patch series has applied successfully based on the early syntax preflight alone.

The preflight was exercised locally with both valid and deliberately malformed unified diffs. All 24 pinned DXC patch files were additionally inspected for matching hunk counts. Native Windows invocation remains **[not verified]**.

## Native DIA/PIX compilation fixes from BuildLog_20261124_012949_447.zip

The 24 November 2026 build successfully completed source patching and Debug/Release CMake configuration. Both native BCC64X variants then failed while compiling the full original `LLVMDxilDia` implementation, with the same eight root compiler diagnostic sites. The failures are clustered into three native Windows COM compatibility categories:

- `comdef.h` is a Microsoft C++ helper header unavailable in the pinned BCC64X environment. The original `DxilDiaSymbolManager.cpp` and `DxilDiaTableSymbols.cpp` now conditionally exclude this header only for native Windows non-MSVC Clang; the one concrete `_bstr_t` expression uses the owned native `CComBSTR` contract instead.
- DXC's BCC64X-owned `CComBSTR` lacked ATL-compatible wide-string assignment and `CopyTo(BSTR*)`. These are implemented using genuine `SysAllocString`, `SysFreeString`, and allocation-safe ownership transfer. The `CA2W` adapter remains UTF-8 to UTF-16. The newly needed `CComVariant` implements the original DIA value setters with native `VARIANT`, `VariantInit`, `VariantCopy`, `VariantClear`, and `VT_UI4`/`VT_BSTR` values, rather than an empty placeholder.
- DIA-derived `Symbol*` and generic `TObject*` interfaces are converted explicitly to their actual native COM interface base before writing interface out-parameters. The generic `DoBasicQueryInterface_recurse` stores the converted interface address in `void**` rather than assuming unadjusted object pointers. All original COM reference count operations are preserved.

The source corrections are supplied as `bcc64x-native-dia-source-compat.patch`, applied after the existing native COM query-call patch in the library's source contract. The native helpers are updated at `admin/compat/bcc64x/dxc-native-atl.h`. All ten patch hunks were checked against the complete pinned original source files.

**Acceptance [not verified]:** BCC64X Release/Debug compile and link, COM reference ownership, DIA factory activation, interface round-trips, BSTR/VARIANT memory management and all original tests still require an actual build and runtime verification.
