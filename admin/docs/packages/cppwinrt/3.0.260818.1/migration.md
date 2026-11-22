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
