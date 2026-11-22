# Microsoft integration: BCC64X platform-switch audit (2026-11-22)

Status: static source review; native BCC64X regression runs pending. Do not claim PASS.

## Boundary

BCC64X is Clang on native Windows. The BCC64X SDK also exposes MinGW-compatible macros and headers. Compiler frontend, runtime header compatibility, Windows API availability, calling convention, and target triple are different facts. A macro such as `__MINGW32__` is **not** evidence that MinGW's platform/runtime workarounds are appropriate. Likewise `_MSC_VER` is not reliable as a general capability predicate.

Reviewed:
- BuildEngine-Admin Microsoft-focused version-bound CMake and source patches: DirectX-Headers, DXC, Detours, C++/WinRT, WIL, WinMD, DirectXMath, DirectStorage/GDeflate and smoke CMake files.
- Microsoft DirectX-Headers v1.619.1 original runtime tests.
- DirectXShaderCompiler v1.9.2609 original GetHostTriple and HLSL/HLSLTestLib test CMake, plus GitHub code-search matches for `__MINGW32__`, `__BORLANDC__`, `_MSC_VER` (non-exhaustive for the ~19,666-file tree).
- Actual preprocessed active BCC64X translation units have not been provided; no claim of a complete source-tree or expanded-macro inventory.

## Corrected: DirectX-Headers v1.619.1

Both original native-Windows tests contain an upstream `#ifdef __MINGW32__` implementation of `DXCoreCreateAdapterFactory` that returns `0` without constructing the factory. Under BCC64X this can produce S_OK and a null COM pointer. A version-bound workspace-only source adaptation now excludes that stub for `ADECC_NATIVE_WINDOWS_DXCORE`; the native Windows tests link `dxcore` without using MSVC/MINGW compiler identity. A null-factory guard now explicitly reports the contract failure in both test executables before `CreateAdapterList`. No test target has been removed. **Unverified**: native configure/link/runtime and COM factory pointer.

## DXC v1.9.2609: priority findings

| File | Switch | Risk / decision | Verification |
| --- | --- | --- | --- |
| `utils/unittest/googletest/include/gtest/internal/gtest-port.h` | `defined(_MSC_VER) || defined(__BORLANDC__)` for `GTEST_HAS_EXCEPTIONS` | Legacy C++Builder code chooses `_HAS_EXCEPTIONS` instead of Clang's exception feature probe. High-priority source compatibility candidate; do not change exceptions policy without comparing macro expansion and assertions. | BCC64X `-dM -E` and test build in both variants |
| same file | same for `GTEST_HAS_SEH` | Conflates compiler brand with SEH support. Check actual BCC64X SEH compile support; retain the original fault tests. | Compile/run SEH test, original suite |
| `utils/unittest/googletest/include/gtest/internal/gtest-string.h` | `__BORLANDC__` forces `<mem.h>` | Legacy Borland compatibility header chosen under Clang; check native BCC64X CRT/header declarations. | Include trace and original cases |
| `utils/unittest/googletest/include/gtest/internal/gtest-internal.h` | `__BORLANDC__` chooses alternate typed-overload path | Legacy workaround may change test helper behavior; avoid changing without a source and feature comparison. | Original typed tests |
| `utils/unittest/googletest/src/gtest.cc` | `__BORLANDC__` / `__MINGW32__` | Time/Singleton compatibility branches. `__MINGW32__` path assumes absent `localtime_s`; API probe needed. | Test time & concurrency |
| `lib/Support/Windows/TimeValue.inc` | `__MINGW32__` | Old MinGW branch calls `localtime` instead of `_localtime64_s`; potential correctness and reentrancy issue, but do not force an unavailable CRT declaration. | Probe symbol/header availability and time conversion |
| `lib/Support/Windows/RWMutex.inc` | `__MINGW32__` | Redeclares `SRWLOCK`/RTL SRWLOCK; may conflict with registered Windows SDK. | Compile with SDK include trace |
| `lib/Support/Windows/DynamicLibrary.inc` | `__MINGW32__` | Selects `imagehlp.h` rather than `dbghelp.h`; actual declarations/imports must decide. | SDK include and dynamic-library test |
| `lib/Support/Windows/Signals.inc`, `lib/Support/Windows/Process.inc` | `__MINGW32__` | PSAPI and other historical MinGW fallbacks may select stale API declarations. | Run original LLVM process/signals tests |
| `utils/unittest/googletest/include/gtest/internal/gtest-port-arch.h` | `__MINGW32__` selects `GTEST_OS_WINDOWS_MINGW` | Can cascade into platform-specific test implementations. | Check expanded macros; all original tests |
| `cmake/modules/GetHostTriple.cmake` | BCC64X version patch sets `x86_64-w64-mingw32` | A target triple is not proof of MinGW GCC; follow all consumers before changing. | Observe compiler `-dumpmachine` and generated config |
| `tools/clang/unittests/HLSL/CMakeLists.txt`, `HLSLTestLib/CMakeLists.txt` | `WIN32` selects TAEF | Genuine platform split but original test cases require a complete port. | TAEF->CTest case equivalence and counts |
| `projects/dxilconv/CMakeLists.txt` (local portable patch) | Replaces `unittests` and `test` with `portable` | Not equivalent to upstream cases; retain INCOMPLETE status. | Six original DXIL converter regression cases plus LIT fixtures |

Additional search candidates: `lib/Support/raw_ostream.cpp`, `lib/DxilCompression/miniz.c`, `lib/Support/Windows/TimeValue.inc`, `lib/Support/Windows/RWMutex.inc`, `lib/Support/Windows/DynamicLibrary.inc`, `tools/clang/lib/Basic/VirtualFileSystem.cpp`. The search result also included Clang's **cross-target source** such as `tools/clang/lib/Basic/Targets.cpp` and `tools/clang/lib/Headers/mm_malloc.h`; these are not automatically host-BCC64X problems.

## Other Microsoft packages

- Detours v4.0.1 already has explicit `!defined(__clang__)` protection for historical `_MSC_VER` branches; verify SEH, import and ABI correctness in the original and native smoke tests.
- C++/WinRT v3.0.260818.1 Catch patch explicitly excludes Clang from a legacy `__BORLANDC__` isnan polyfill. Its `CMAKE_CXX_SIMULATE_ID=EMBT` guard currently affects SDK include discovery, **not** selection of MinGW ABI.
- WIL SDK-atomics compatibility is opt-in through `ADECC_BCC64X_WIL_SDK_ATOMICS`, not automatic compiler identity.
- DirectXMath SHMath patch uses `CMAKE_CXX_COMPILER_FRONTEND_VARIANT` to decide whether an MSVC-style switch is accepted; correct kind of capability distinction.

## Required acceptance gate

1. Run source search against **exact extracted, version-pinned Microsoft build trees**, including all nested vendored code.
2. Capture active preprocessor definitions and selected include paths using native BCC64X for Release and Debug. Do not indiscriminately undefine MinGW macros, as some CRT headers require them.
3. Classify each conditional by **actual dependency**: frontend syntax, CRT API, Windows SDK API, ABI/calling convention, target triple, packaging, or test runner.
4. For each changed conditional, compile a focused capability probe plus the unchanged original test target; preserve original test counts and assertions.
5. Confirm DXCore real `dxcore.dll` binding, valid non-null factory and COM methods before accepting native DirectX-Headers integration.

No MSVC/MinGW compiler fallback, invented API implementation, or silently disabled tests.
