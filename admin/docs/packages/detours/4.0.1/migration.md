# Microsoft Detours 4.0.1: native C++Builder / BCC64X integration

[TOC|Content]

**Current status: [not verified].** The upstream source and package contract was added on 22 November 2026. This declaration is not compiler, test, package, or runtime success evidence.

## Upstream

Source: [Microsoft repository](https://github.com/microsoft/detours/tree/v4.0.1) at pinned tag `v4.0.1`. License: MIT; preserve and install the upstream license. This version is a distinct logical BuildEngine library identity.

## Native build and package

native BCC64X DLL CMake adapter; source retains upstream NMAKE files. The managed BCC64X C++23 toolchain, Ninja, Windows SDK and dynamic runtime remain mandatory. Release and Debug variants are separate installed package products. No substitution with MSVC, clang-cl, or MinGW is permitted.

## Direct evidence

Dedicated native C++23 consumer: API interception transaction and detachable hook. This consumer tests the actual installed package, not just a permissive include directory.

Original Microsoft Detours sources use NMAKE. The new CMake adapter is an intentional native BCC64X port, **not** the entire original NMAKE upstream test suite. Its dynamic exports, DLL linking and transactions must be validated. The original sources are retained for Vulkan Loader test integration.

## Open gates

Source pin and extraction, Release/Debug compilation, all selected original targets and features, DLL or interface package export, installed license and header ownership, publisher collision check, security and CycloneDX metadata, and native consumer execution remain **[not verified]**.

[Microsoft integration overview](/manual/libraries.md) · [Development news](/news/2026-11-22/)

## BCC64X follow-up, 22 November 2026

The original modules.cpp treated undefined _MSC_VER as a legacy MSVC version and skipped <strsafe.h>; StringCchCopyA and StringCchCatA were therefore unavailable. The new ordered, version-bound patch corrects only that compiler guard, preserving the original build sources and native tests.

Automatic C++ module scanning is disabled centrally by the BCC64X CMake toolchain (`CMAKE_CXX_SCAN_FOR_MODULES=OFF`) without lowering the C++23 language level.

**[not verified]** until native Release, Debug and consumer tests pass.
