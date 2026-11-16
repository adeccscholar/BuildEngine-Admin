# Adobe DNG SDK 1.7.1 Build 2802: independent BuildEngine producer

[TOC|Content]

## Integration and ownership

This is a new, **independent BuildEngine library** for the Adobe Digital Negative (DNG) SDK, identified by `dng-sdk@1.7.1.2802`. The intent is to replace Skia 153's old, embedded Android DNG SDK code with a managed native DLL and matching import library, while retaining Skia's RAW/DNG decoder.

**Status (16 November 2026): declared; native BCC64X build, installed outputs and Skia decode consumer are [not verified].** The prior Skia builds failed on `std::auto_ptr` in the older bundled `dng_pthread.cpp`. Do not interpret the new integration as a successful native port before matching execution evidence.

## Source provenance and license

- Adobe DNG SDK version: **1.7.1, Build 2802**, confirmed by the upstream `dng_flags.h` version macros in the selected source.
- Pinned revision-preserving source mirror: [hfiguiere/dng_sdk](https://github.com/hfiguiere/dng_sdk), commit `3a31897afe53aad5f15854eba0f04bab77cc4f82`.
- Source archive: `https://github.com/hfiguiere/dng_sdk/archive/3a31897afe53aad5f15854eba0f04bab77cc4f82.tar.gz`.
- Original Adobe license: `LICENSE.txt` in the source mirror, installed as `licenses/dng-sdk/LICENSE.txt`. The license is **not an OSI-approved open-source license**; it includes separate distribution and indemnification terms. The declarative license signal is yellow, not green.
- The source mirror's Meson build is Linux-oriented and produces a static library. BuildEngine supplies a version-specific Windows CMake adapter; the upstream Meson build is **not** treated as a proven BCC64X DLL build.

The GitHub mirror is not an Adobe-controlled repository. The revision is pinned to avoid moving-branch downloads; the license and vendor authenticity of a future release should be reviewed independently.

## Native producer contract

The BuildEngine CMake adapter is located at `admin/cmake/adapters/dng-sdk/CMakeLists.txt`. Its source list is curated from the pinned 1.7.1.2802 DNG Meson producer. It builds a **shared** `dng_sdk` library under the existing BCC64X CMake/Ninja rules, using the dynamic C++ runtime, with exact Release/Debug output destinations.

### Direct dependencies

- `zlib@1.4`
- `libjpeg-turbo@3.2.1`
- `libjxl@0.13.0`

These are declared through versioned BuildEngine coordinates. The adapter resolves installed variant-specific import libraries, rather than compiling privately bundled copies. XMP is deliberately disabled in this first RAW decoder producer; its separate XMP Toolkit subtree is not built. JPEG XL sources remain enabled and use the managed JPEG XL producer. The adapter explicitly excludes `dng_xmp_sdk.cpp` because that file unconditionally imports the XMP Toolkit even when XMP support is disabled.

Required per-variant artifacts:

```text
include/dng_sdk/source/dng_host.h
lib/win64/{Configuration}/dng_sdk.lib
bin/win64/{Configuration}/dng_sdk.dll
licenses/dng-sdk/LICENSE.txt
```

All paths are relative to the versioned package root. Output and import-library names are explicitly pinned to avoid the earlier Windows naming mismatch seen in other BCC64X packages.

## Skia 153 external consumer

The version-specific GN patch `bcc64x-system-dng-sdk.patch` changes `third_party/dng_sdk/BUILD.gn` to select a `system("dng_sdk")` group instead of a source-building `third_party("dng_sdk")` target when `skia_use_system_dng_sdk=true`.

BuildEngine passes the managed header root and import library through `CB_SKIA_DNG_SDK_INCLUDE` and `CB_SKIA_DNG_SDK_LIB`, and adds the package DLL directory to each applicable runtime `PATH`. `skia_use_dng_sdk=true` remains enabled. The public compile definitions align the SDK layout between DLL and consumer, and set `SK_DNG_VERSION=0x01070100` for Skia's version-dependent RAW code.

The old Skia-side DNG Gitiles archive, extraction and obsolete pthread patch are removed from its active source workflow. The GN upstream fallback remains inside the patch for reference, but is not selected by the declared configuration. Skia's SBOM manifest now reports DNG SDK as external.

```mermaid
flowchart LR
   skia["Skia 153 / RAW codec"] --> dng["dng-sdk 1.7.1.2802 / DLL"]
   dng --> jpeg["libjpeg-turbo 3.2.1"]
   dng --> zlib["zlib 1.4"]
   dng --> jxl["libjxl 0.13.0"]
```

## Installed DLL consumer smoke

The declarative package contract includes `smokes/dng-sdk/consumer` with `scope="package"`. This C++23 smoke uses the managed Release package, compiles against the installed `dng_host.h`, links to `dng_sdk.lib`, constructs a `dng_host` and executes `PerformAreaTaskThreads()` through the DLL. This tests the binary import boundary beyond the mere existence of the files. A separate RAW/DNG image decode consumer remains necessary for functional acceptance.

## Verification gate

The following checks must all pass before calling this migration complete: native Release and Debug configuration/build/install; exact DLL/import library and exported-symbol checks; GN generation proving the bundled `dng_pthread.cpp` is **not** in Skia's compilation graph; Skia Release and Debug link; PE DLL dependency closure; and a real RAW/DNG decode consumer test.

**[not verified]** None of those post-migration native Windows results has yet been supplied. Compiler, upstream-API, symbol-export and ABI issues uncovered during the first native attempt must be corrected at the producing layer, not hidden by re-enabling embedded DNG sources.

[Skia 153 integration](/package/skia/153/migration) · [Library catalog](/manual/libraries.md) · [Skia externalization](/manual/skia-externalization.md)
