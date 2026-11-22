# NVIDIA libdeflate GDeflate fork 1.8-8ba9502f

[TOC|Content]

**Status: [not verified].** Source integration correction after 22 November 2026 native BCC64X diagnostic.

## Reason for an independent package

Microsoft DirectStorage GDeflate repository commit `9352da00f98764ac9f2768e424dacc2cac7cd66d` contains a Git submodule to `NVIDIA/libdeflate` commit `8ba9502fb30d2bf728592d121f0d402e40c8cb05`. This upstream fork declares and implements `libdeflate_gdeflate_*` symbols required by Microsoft's original `GDeflateCompress.cpp` and `GDeflateDecompress.cpp`. Regular libdeflate 1.26 does not offer those functions and must not be substituted by a fabricated compatibility layer.

## Build and dependency boundary

A dedicated BCC64X CMake producer compiles the original fork C sources to `libdeflate_gdeflate.dll` with a versioned import library. Both Debug and Release products, the exact upstream license, native gdeflate API test and independent installed-package consumer are mandatory. The fork's public `libdeflate.h` is owned only by its versioned package, **not** published to the shared `Win64x/include` because a different revision already belongs to `libdeflate/1.26`. Only the GDeflate fork DLL and import library are published from this package. Microsoft GDeflate consumes the versioned upstream header and native CMake target.

This is an intentionally separate producer identity. The complete upstream fork test suite has not been ported and cannot be described as executed. **[not verified]** includes compilation, DLL exports, tests, native consumer, installed package and final FSM Ready.
