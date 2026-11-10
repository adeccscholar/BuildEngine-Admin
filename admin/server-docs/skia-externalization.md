# Skia 153: External Library Candidates and DLL Boundaries

[TOC|Content]

**Contract review dated 10 November 2026.** These proposals do not modify Skia. Each proposed new DLL integration would require a separate, versioned BCC64X build with installed-consumer smoke tests. The current PDFium XML contract references **Skia 153**; its DLL/ABI runtime behavior must be verified separately through FSM and loader smokes.

## Libraries already configured as external

The current BuildEngine contract for Skia 153 explicitly references the following externally managed components:

| Library | Version | GN contract |
| --- | --- | --- |
| zlib | 1.4 | `skia_use_system_zlib = true` |
| Brotli | 1.2.1 | `skia_use_system_brotli = true` |
| libpng | 1.6.59 | `skia_use_system_libpng = true` |
| libjpeg-turbo | 3.2.1 | `skia_use_system_libjpeg_turbo = true` |
| HarfBuzz | 15.1.0 | `skia_use_system_harfbuzz = true` |
| FreeType | 2.15.0 | `skia_use_system_freetype2 = true` |

OpenGL 26.2.1 is additionally declared as a BuildEngine dependency. Enabling a GN option does **not** by itself prove a DLL import relationship. The PE import tables and exact Debug/Release artifacts must be inspected for verification.

## Components currently bundled and potentially reusable

| Component | Current contract | Assessment |
| --- | --- | --- |
| Expat | `skia_use_expat=true`, `skia_use_system_expat=false` | **Priority 1:** Evaluate reuse of `expat@2.10.0`; XML/SVG smoke tests required |
| ICU | `skia_use_icu=true`, `skia_use_system_icu=false` | **Priority 2:** Investigate `icu@79.1`; first resolve the Chromium pin, ICU data, ABI and Unicode layout |
| libWebP | Decode/encode enabled, `skia_use_system_libwebp=false` | **Priority 3:** Potential independent image codec; no `webp` entry in the active XML catalog reviewed |
| libavif / libgav1 | Sources pinned internally | Later: assess the full codec and AV1 dependency chain |
| JPEG XL / highway | Internally pinned | Later: investigate complex C++ and SIMD interfaces |
| SPIRV-Cross, glslang, SPIRV-Tools | Part of the shader/GPU pipeline | Do not assume a trivial drop-in DLL replacement |
| Wuffs, skcms | Internal Skia decoders and color processing | Keep bundled for now |
| DNG SDK, Vulkan Memory Allocator | Bundled utility components | Extract only with additional real consumers and a stable DLL API |

## Controlled externalization procedure

First demonstrate that the existing Skia 153 installed consumer works in both Release and Debug. Then externalize **one** library at a time, starting with Expat, and validate the GN integration against the actual installed DLL and import library. Add a version-specific dependency record to the BuildEngine graph only after the corresponding build has been verified. The project SBOM must distinguish newly externalized components from sources that remain bundled.

**Important:** Bundled upstream source is not automatically interchangeable with a separately built DLL. ICU data, private symbol versions and shader tooling may constrain compatibility. None of the proposed conversions has separate BCC64X PASS evidence so far: **[not verified]**.

[Skia 153](/package/skia/153/migration) · [News](/news/) · [Libraries](/manual/libraries.md)
