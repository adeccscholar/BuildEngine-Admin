# winmd 0f1eae3b: native BCC64X integration

[TOC|Content]

**Status: [not verified]**. A declared source/package configuration is not a successful native build or consumer test.

Upstream is pinned in admin/build-libraries.xml. C++Builder 13 BCC64X, Ninja, installed dependency packages and the dynamic runtime are mandatory. Keep upstream features and tests where provided. Do not use unpinned network fetching or alternative MSVC/MinGW compiler pipelines.

The WinMD Reader is a header-only source snapshot identified by its upstream commit.

Complete Release/Debug, install, metadata, license, SBOM, publish, native test/consumer and FSM evidence remains **[not verified]**.

[Library catalog](/manual/libraries.md) · [Development news](/news/2026-11-22/)


## First native run and scoped correction

The installed consumer failed with E4141 on std::equal_range in view.h. Upstream base.h omits the standard <algorithm> header; a version-pinned source patch adds it. Consumer and final Ready still require another native run. **[not verified]** until BCC64X Release and Debug rerun.

## BCC64X follow-up, 22 November 2026

The installed-header consumer uses std::vector<TypeSig> before TypeSig is fully defined, triggering an incomplete-type diagnostic in libc++. The version-bound patch moves GenericTypeInstSig's vector-access and special-member definitions below the complete TypeSig declaration; its API and consumer test remain enabled.

Automatic C++ module scanning is disabled centrally by the BCC64X CMake toolchain (`CMAKE_CXX_SCAN_FOR_MODULES=OFF`) without lowering the C++23 language level.

**[not verified]** until native Release, Debug and consumer tests pass.
