# cppwinrt 3.0.260818.1: native BCC64X integration

[TOC|Content]

**Status: [not verified]**. A declared source/package configuration is not a successful native build or consumer test.

Upstream is pinned in admin/build-libraries.xml. C++Builder 13 BCC64X, Ninja, installed dependency packages and the dynamic runtime are mandatory. Keep upstream features and tests where provided. Do not use unpinned network fetching or alternative MSVC/MinGW compiler pipelines.

The generator depends on the separate managed WinMD Reader source to avoid a FetchContent/ExternalProject download.

Complete Release/Debug, install, metadata, license, SBOM, publish, native test/consumer and FSM evidence remains **[not verified]**.

[Library catalog](/manual/libraries.md) · [Development news](/news/2026-11-22/)


## First native run and scoped correction

Original upstream tests failed to include SDK winstring.h. A local CMake patch adds explicit registered Windows SDK um/shared/winrt includes for native BCC64X, with checks and diagnostics. No tests were disabled. **[not verified]** until BCC64X Release and Debug rerun.

## BCC64X follow-up, 22 November 2026

The Windows SDK WinRT header winstring.h is provided in winrt/, rather than the probed um/ path. The new ordered, version-bound patch corrects the probe and failure diagnostic, retaining the registered SDK, the BCC64X toolchain and upstream tests.

Automatic C++ module scanning is disabled centrally by the BCC64X CMake toolchain (`CMAKE_CXX_SCAN_FOR_MODULES=OFF`) without lowering the C++23 language level.

**[not verified]** until native Release, Debug and consumer tests pass.

## Catch source compatibility in original C++/WinRT tests

The vendored Catch header selects a legacy `std::_isnan` path whenever `__BORLANDC__` is defined. BCC64X is Clang-based and provides standard `std::isnan`; the new version-bound source patch restricts that legacy path to non-Clang Borland compilers. The full upstream test suite stays active. Release and Debug results remain **[not verified]**.

## Original test_nocoro fatal handler (native BCC64X)

`test/test_nocoro/main.cpp` invokes Catch's `reportFatal` but omits `CATCH_CONFIG_WINDOWS_SEH`; other upstream test executables set that macro. A version-bound patch enables the same original SEH handler in test_nocoro. This preserves upstream test execution, rather than replacing or skipping the fatal error test path.

**[not verified]** until Release and Debug CTest passes.

## Native COM test signatures and generated test component (22 November 2026)

Release and Debug compiler logs now identify three independent original-test integration gaps: `winrt/test_component.h` is missing from the generated WinRT projection; BCC64X's `combaseapi.h` defines an `_CG_GUID_PPT_Helper(T**)` deduction overload incompatible with `IID_PPV_ARGS(&winrt::com_ptr<T>)`; and the native SDK's `IUnknown::QueryInterface` requires the two-parameter COM ABI, whereas upstream legacy tests use a one-parameter helper.

The versioned COM test patch supplies the overload for `winrt::com_ptr<T>*` in the original test translation unit and passes explicit `__uuidof(::IUnknown)` or `__uuidof(::IInspectable)` plus `put_void()` at the four affected QueryInterface calls. The tests and their assertions are retained.

The versioned test CMake patch now defines a real dependency chain: original `test_component/test_component.idl` → native Windows SDK `midlrt.exe` → `test_component.winmd` → the just-built `cppwinrt` generator → `winrt/test_component.h`. The required generated header is a dependency of the existing `build-cppwinrt-projection` target and therefore of the original `test-vanilla` target. Native SDK MIDLRT and Windows metadata are external tool prerequisites: a missing tool causes a clear configure error rather than silently omitting tests. No MSVC compiler, MinGW compiler, test exclusion, substitute projection or fabricated WinMD is used.

**[not verified]**: availability of `midlrt.exe` in the configured Windows Kits installation, exact MIDLRT/CPPWINRT command-line behavior, successful native builds and test execution.

## Production-server locale and FSM rebuild correction (28 November 2026)

The production-server run `BuildLog_20261127_211359_732.zip` finished 151 of 152 libraries; only C++/WinRT upstream CTest `test` and `test_old` failed in both Debug and Release. Eleven failed Catch2 assertions compared localized Windows HRESULT system messages with fixed English strings. The earlier `bcc64x-production-test-portability.patch` had been committed after the library timestamp still set to 2026-11-27T15:35:00Z; therefore the current FSM source/build evidence did not require those already completed stages to execute again. The newly increased C++/WinRT timestamp invalidates its source/build scopes so that the complete existing ordered patch chain is applied to rebuilt tests. The final two message-only comparisons are handled by `bcc64x-hresult-message-portability.patch`, retaining the adjacent HRESULT identity checks and the existing exception-type checks. No upstream test is disabled. Other library contracts and timestamps remain unchanged.

The complete native BCC64X Debug/Release validation and final 152/152 result remain **[not verified]** until a new production run.

## Locale-neutral verification gate for Windows Server 2025 (28 November 2026)

Production evidence `BuildLog_20261128_021453_964.zip` shows the expected commit and the successful source-patch actions, followed by Debug/Release builds and CTest processes that still evaluate original English HRESULT message assertions. The outstanding error is not evidence of incorrect HRESULT codes. A final version-bound patch removes the remaining URI-message assertion from `test/test/error_info.cpp`. A new scoped CMake verification gate checks the three affected source files, rejects remaining locale-specific system-message comparisons, checks that the ten legacy replacements are present, and refreshes only the three affected translation units' timestamps before Ninja builds. This avoids silently reusing object files from earlier source timestamps and makes a source-patch mismatch fail before the expensive test run. Existing functional exception type and HRESULT-code checks remain enabled. No unrelated library contract, SDK or toolchain changes.

The server's 152/152 result is still **[not verified]** until the next production run.

## Complete audit of locale-specific upstream assertions (28 November 2026)

Production log `BuildLog_20261128_032515_602.zip` shows that all source patches were successfully applied and the source-level verification gate failed early, specifically on an unmodified second `BAD is not a valid absolute URI.` assertion. The pinned original `test/old_tests/UnitTests/hresult_error.cpp` repeats system-generated English messages in two or three independently tested paths per HRESULT. The previous portability patch covered ten occurrences, not the complete twenty. The version-bound `bcc64x-remaining-hresult-portability.patch` handles the ten remaining system-message comparisons and the Windows XML parser's localized fallback in `test/test/error_info.cpp`. The verifier now requires at least twenty locale-neutral legacy message checks and rejects the XML parser's English literal. Explicit test-supplied diagnostic texts remain exact assertions. All HRESULT identity and exception-type assertions remain enabled; the native test suite is not disabled. No changes to other libraries or to the Windows SDK/toolchain were made.

**[not verified]** Native BCC64X server test outcome and 152/152 completion require another production run.
