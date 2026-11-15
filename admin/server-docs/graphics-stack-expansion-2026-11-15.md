# Graphics and Shader Stack Expansion — 15 November 2026

[TOC|Content]

## Scope and evidence

This note tracks the requested BuildEngine expansion and deliberately distinguishes XML registration from verified native Windows/BCC64X builds. No source code package or binary is declared successful without a producer, install, and consumer smoke result.

## OpenCV 5.1.0

The 5.1.0 profile already consumes the pinned opencv_contrib checkout. Its BUILD_LIST was extended from 17 to 21 modules by adding `img_hash`, `optflow`, `quality`, and `ximgproc`. The module DLL and import-library expectations, installed-consumer component list, and a functional subset of the smoke test were updated. 5.0.0 remains unchanged. Validation: **[not verified]**.

Do not enable all upstream Contrib modules blindly: CUDA-only and external-data/service modules require separate dependencies and toolchain checks.

## VTK 9.7.2

The current BuildEngine contract already enables WGL/OpenGL2, RenderingVolumeOpenGL2, interaction, annotation, and PNG I/O. Optional Win32 OpenGL/Direct3D interop is disabled specifically for BCC64X. The rendering smoke asserts the concrete `vtkOpenGLGPUVolumeRayCastMapper` implementation rather than equality with its abstract factory class. No new VTK configuration change is required in this review. The interactive GPU rendering test remains distinct from the context-free package smoke, and its current result has not been independently rerun here.

## Registered package contracts (native execution pending)

The requested package list has now been entered into `admin/build-libraries.xml`. This is a definition change, not evidence of BCC64X compilation, installation or runtime success.

| Coordinate | Integration model | Qualification |
| --- | --- | --- |
| `highway@1.4.0` | CMake, static library, both variants | Upstream target/export to verify |
| `wuffs@0.3` | Pinned Wuffs release-C source file | Source distribution; no DLL |
| `libavif@1.4.2` | CMake, libaom and libyuv dependencies | Verify SYSTEM provider discovery and installed codec support |
| `libaom@3.15.1` | CMake, AV1 decode/encode | Newly added prerequisite for libavif; NASM/Clang toolchain to verify |
| `libjxl@0.13.0` | CMake, managed Highway/Brotli/LCMS2/libpng | Verify system dependency selection, headers and exports |
| `libyuv@r1924` | Pinned CMake source snapshot | Verify source mirror provenance, target installation and link artifacts |
| `vulkan-headers@1.4.368` | Header-only package | Vulkan loader is not provided |
| `vulkan-memory-allocator@3.4.0` | Header/source package | Consumer provides implementation; depends on Vulkan headers |
| `spirv-headers@942fe4b988359a0750b79f0ae7ed735994d3147d` | Pinned CMake headers package | Revision matches Skia 153's bundled manifest |
| `spirv-tools@2026.4` | CMake producer | Verify upstream release and SPIRV-Headers CMake discovery |
| `spirv-cross@b8fcf307f1f347089e3c46eb4451d27f32ebc8d3` | Pinned CMake producer | Verify static libraries, CLI and export paths |
| `glslang@16.6.0` | CMake producer | Verify upstream tag and downstream SPIR-V linker closure |

**No new package has a verified native BCC64X PASS result from this change set.** The assigned coordinates, archive layouts, CMake flags, and installation artifacts must be checked in the Windows environment. Do not use these registry entries as evidence that the source was compiled or that a third-party Skia dependency has already been externalized. In particular, upstream source pinning is not an independent runtime linkage guarantee.

## Outstanding integration and provenance checks

- Validate XML against `admin/schemas/build-libraries.xsd` (schema version 17); the GitHub edit alone does not perform XSD validation.
- Verify real upstream archives, license file paths, target names, and installed binary names for every new package.
- For `libavif`, validate that the configured SYSTEM AOM and libyuv backends are actually found and encoder/decoder APIs are functional.
- For `libjxl`, validate bundled-versus-system dependencies and the full linkage closure.
- For the shader group, validate SPIRV-Headers package layout, SPIRV-Tools matching version and glslang/SPIRV-Cross interfaces.
- Ensure all successfully built CMake packages expose functional Release/Debug consumer smokes and correct SBOM dependency metadata. At the moment these new entries do not all have dedicated smokes.
- Skia 153 remains unmodified. Its SBOM continues to disclose components as bundled until their externalization is implemented and verified.

## Validation sequence

1. Validate the expanded XML against `schemas/build-libraries.xsd` (schema 17).
2. Run OpenCV 5.1.0 Debug and Release, then installed consumer smoke.
3. Run VTK 9.7.2 installed-package and interactive rendering smokes.
4. Run each newly registered package with a consumer, and verify the DLL/import-library boundaries where applicable.
5. Resolve and implement the remaining codec and shader toolchain producers in dependency order.
6. Externalize bundled Skia dependencies one at a time only after exact GN and PE linkage is verified. Update bundled-component SBOM provenance only after an actual replacement.
