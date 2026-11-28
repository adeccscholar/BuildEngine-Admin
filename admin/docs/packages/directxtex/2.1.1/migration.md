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

## Integration retrospective and showcase contract (28 November 2026)

DirectXTex provides texture and image-processing routines, including mipmap generation and texture formats. A visual texture laboratory should measure actual mip-levels, memory usage and fidelity; avoid claiming that CPU processing is GPU acceleration.

**Operational boundary:** The operator reported that the developer and Windows Server 2025 integration environments now complete their library work. This is an operator-reported outcome, not a newly executed BCC64X build in this documentation update. Shader compilation, graphics-card selection, measured frame rate and future showcase acceptance remain independent checks. Do not replace the managed C++Builder 13 / BCC64X toolchain, change another library's timestamp or disable upstream tests to obtain a visual result.

[Engineering retrospective](/news/2026-11-28/) · [Library catalog](/manual/libraries.md) · [Showcases](/showcases/)
