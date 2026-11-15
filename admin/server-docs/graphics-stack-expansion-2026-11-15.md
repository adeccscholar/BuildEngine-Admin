# Graphics and Shader Stack Expansion — 15 November 2026

[TOC|Content]

## Scope and evidence

This note tracks the requested BuildEngine expansion and deliberately distinguishes XML registration from verified native Windows/BCC64X builds. No source code package or binary is declared successful without a producer, install, and consumer smoke result.

## OpenCV 5.1.0

The 5.1.0 profile already consumes the pinned opencv_contrib checkout. Its BUILD_LIST was extended from 17 to 21 modules by adding `img_hash`, `optflow`, `quality`, and `ximgproc`. The module DLL and import-library expectations, installed-consumer component list, and a functional subset of the smoke test were updated. 5.0.0 remains unchanged. Validation: **[not verified]**.

Do not enable all upstream Contrib modules blindly: CUDA-only and external-data/service modules require separate dependencies and toolchain checks.

## VTK 9.7.2

The current BuildEngine contract already enables WGL/OpenGL2, RenderingVolumeOpenGL2, interaction, annotation, and PNG I/O. Optional Win32 OpenGL/Direct3D interop is disabled specifically for BCC64X. The rendering smoke asserts the concrete `vtkOpenGLGPUVolumeRayCastMapper` implementation rather than equality with its abstract factory class. No new VTK configuration change is required in this review. The interactive GPU rendering test remains distinct from the context-free package smoke, and its current result has not been independently rerun here.

## Registered packages (native execution pending)

| Coordinate | Integration model | Notes |
| --- | --- | --- |
| `highway@1.4.0` | Standalone CMake producer | Static library, Release and Debug; CMake install |
| `wuffs@0.3` | Pinned single-file C distribution | Header/source package; no artificial DLL |
| `vulkan-headers@1.4.368` | API header package | Vulkan loader is **not** included |
| `vulkan-memory-allocator@3.4.0` | Header-based implementation package | Requires Vulkan headers; implementation compiled by consumers |
| `spirv-headers@942fe4b988359a0750b79f0ae7ed735994d3147d` | Pinned source/header package | Shared revision with the Skia 153 bundled manifest |

No native PASS evidence has been generated for these new packages. They must not be counted as successfully translated libraries yet.

## Remaining requested producer packages (not yet registered)

| Component | Integration prerequisites |
| --- | --- |
| `libavif` | Explicit versioned AV1 encoder/decoder backend closure (`libaom` or `libgav1`), codec options, CMake exports |
| `libjxl` | Versioned Brotli, Highway, color-management dependencies and upstream CMake system-library resolution |
| `libyuv` | Upstream revision pin, source archive provenance, real CMake install outputs |
| `SPIRV-Tools` | Matching SPIRV-Headers CMake package/export and optimizer tool artifacts |
| `SPIRV-Cross` | Pinned upstream CMake targets, installed libraries, tests |
| `glslang` | Matching SPIRV-Tools/Headers dependency and configured optimizer/linker tools |

The group must retain dependency versions centrally rather than hard-coding build-output paths. Debug/Release artifacts, install layout, licenses, CycloneDX metadata, and consumer smoke tests are required before declaring adoption complete.

## Validation sequence

1. Validate the expanded XML against `schemas/build-libraries.xsd` (schema 17).
2. Run OpenCV 5.1.0 Debug and Release, then installed consumer smoke.
3. Run VTK 9.7.2 installed-package and interactive rendering smokes.
4. Run each newly registered package with a consumer, and verify the DLL/import-library boundaries where applicable.
5. Resolve and implement the remaining codec and shader toolchain producers in dependency order.
6. Externalize bundled Skia dependencies one at a time only after exact GN and PE linkage is verified. Update bundled-component SBOM provenance only after an actual replacement.
