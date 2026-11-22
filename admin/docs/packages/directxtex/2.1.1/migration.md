# Microsoft DirectXTex 2.1.1: native C++Builder / BCC64X integration

[TOC|Content]

**Current status: [not verified].** The upstream source and package contract was added on 22 November 2026. This declaration is not compiler, test, package, or runtime success evidence.

## Upstream

Source: [Microsoft repository](https://github.com/microsoft/DirectXTex/tree/may2026) at pinned tag `may2026`. License: MIT; preserve and install the upstream license. This version is a distinct logical BuildEngine library identity.

## Native build and package

original upstream CMake with D3D11, D3D12, command-line tools, sample and OpenEXR/JPEG/PNG/OpenMP features requested. The managed BCC64X C++23 toolchain, Ninja, Windows SDK and dynamic runtime remain mandatory. Release and Debug variants are separate installed package products. No substitution with MSVC, clang-cl, or MinGW is permitted.

## Direct evidence

Dedicated native C++23 consumer: scratch image and mipmap generation. This consumer tests the actual installed package, not just a permissive include directory.

The pinned upstream tag contains no packaged Tests/ tree; BUILD_TESTING is left available. Consumer checks are independent native evidence and must not be represented as an executed full upstream test suite.

## Open gates

Source pin and extraction, Release/Debug compilation, all selected original targets and features, DLL or interface package export, installed license and header ownership, publisher collision check, security and CycloneDX metadata, and native consumer execution remain **[not verified]**.

[Microsoft integration overview](/manual/libraries.md) · [Development news](/news/2026-11-22/)
