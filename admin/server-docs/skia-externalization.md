# Skia 153: External Library Candidates and DLL Boundaries

[TOC|Content]

## DNG SDK remains embedded (16 November 2026)

Skia 153 still enables `skia_use_dng_sdk = true` and compiles the pinned DNG SDK from `third_party/externals/dng_sdk`; this is not a separately built BuildEngine DLL. The operator's Release and Debug builds failed on obsolete `std::auto_ptr` uses in `dng_pthread.cpp`. A version-bound C++23 source patch replaces those two owners with `std::unique_ptr`, retaining RAW support and the original ownership-transfer calls. **[not verified]** The revised build has not been run yet. The current bundled-component manifest continues to list DNG SDK.

## Current AV1 codec ownership (16 November 2026 XML audit)

The current managed codec closure is `Skia 153 -> libavif 1.4.2 -> libgav1 0.20.0`. libgav1 is also declared as a direct Skia dependency to establish the FSM build gate and provide its DLL location in the per-configuration Skia runtime environment. libavif selects `AVIF_CODEC_LIBGAV1=SYSTEM`, disables AOM decoding and retains managed libaom 3.15.3 for encoding. libyuv r1924 remains a libavif dependency and is selected with `AVIF_LIBYUV=SYSTEM`.

The current XML dependency list and Skia SBOM manifest agree on all 17 direct external dependencies. Five additional components remain recorded as bundled: D3D12MemoryAllocator, dng_sdk, SPIRV-Headers, SPIRV-Tools and glslang. No libgav1 archive is downloaded by the Skia source workflow. Its decoder implementation is built only by the independent libgav1 0.20.0 producer and consumed through the libavif DLL. The bundled component manifest lists libgav1 as an external BuildEngine dependency rather than bundled code.

**Validation pending:** libgav1 BCC64X DLL and import library, libavif codec discovery and linkage, Skia runtime loader path and AVIF decoding smoke. The previous 109/109 result does not verify this updated chain.


## Managed libwebp externalization (15 November 2026)

Skia 153 now declares `libwebp@1.6.0` as a direct BuildEngine dependency, uses `skia_use_system_libwebp = true` with WebP decode/encode enabled, and receives per-configuration import library paths via `DependencyRoot:libwebp` and a version-pinned `bcc64x-system-libwebp.patch`. The previous Skia-bundled WebP archive and extract actions were removed. The SBOM manifest tracks libwebp as external, not bundled; inactive PIE-X is also absent from the bundled list.

**Validation pending:** The supplied libwebp Release log failed because upstream `WEBP_ENABLE_SIMD=OFF` introduced `-mno-sse2`, producing E714 floating-point ABI errors and compiler crashes. The producer was reconfigured with `WEBP_ENABLE_SIMD=ON`; no successful retest, DLL import-library-name verification, GN generation, Skia Debug/Release link or loader smoke is yet available. The historical 109/109 FSM result predates these changes.

## Incremental source cleanup and libwebp registration (15 November 2026)

Skia 153 no longer downloads or extracts PIE-X because the active GN contract sets `skia_use_piex = false`. The unrelated DNG SDK remains enabled and its source is retained. The Skia SPIR-V, glslang, and SPIRV-Cross archives remain pinned while their GN-generated targets have not been replaced by demonstrably compatible managed imports.

**libwebp 1.6.0** is now registered as a separately managed BuildEngine library. Its BCC64X build and Skia's configured GN DLL imports require validation. The active Skia configuration now sets `skia_use_system_libwebp = true`; its bundled WebP source download/extraction has been removed from the declared source workflow. Do not mark the 109/109 historical build as proof for the newly added library.

[libwebp 1.6.0 integration guide](/package/libwebp/1.6.0/migration).

## PIE-X producer follow-up (15 November 2026)

`piex@0.27` is now registered with the seven C++ sources from the upstream GYP build translated into a BuildEngine CMake adapter. The independent DLL, Debug/Release imports, source compatibility and Skia RAW-preview smoke have **not** yet been verified. Skia 153 still has `skia_use_piex = false`. The earlier unused bundled PIE-X download was removed independently, and must not be reintroduced until a specific active fallback requires it. [PIE-X integration guide](/package/piex/0.27/migration).

## Current status: 15 November 2026

**BuildEngine FSM completed successfully: 109 / 109 versioned library integrations, with no failures or blocked jobs.** The earlier text below documents a 10 November proposal and is historical. The active Skia 153 GN configuration already uses external zlib, Brotli, libpng, libjpeg-turbo, HarfBuzz, FreeType, ICU, Expat, libavif and libjxl, plus independently declared OpenGL and Vulkan Memory Allocator dependencies. Transitive managed packages include Highway, libyuv and Vulkan-Headers.

Skia's bundled manifest now excludes Highway, libyuv and Vulkan-Headers to avoid conflicting external coordinates in CycloneDX. **The standalone SPIR-V packages do not yet imply that Skia's GN targets use their external builds**: SPIRV-Tools, SPIRV-Cross, SPIRV-Headers and glslang still have version-pinned source components in the Skia build. Do not remove source trees or claim binary externalization without GN wiring and actual link verification.

[Current Skia GN settings and complete dependency diagram](/package/skia/153/migration).

**Historical contract review dated 10 November 2026.** These proposals do not modify Skia. Each proposed new DLL integration would require a separate, versioned BCC64X build with installed-consumer smoke tests. The current PDFium XML contract references **Skia 153**; its DLL/ABI runtime behavior must be verified separately through FSM and loader smokes.

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
| libWebP | Decode/encode enabled, `skia_use_system_libwebp=false` | **Priority 3:** Potential independent image codec; managed `libwebp@1.6.0` now registered; Skia GN DLL wiring not yet verified |
| libavif / libgav1 | Sources pinned internally | Later: assess the full codec and AV1 dependency chain |
| JPEG XL / highway | Internally pinned | Later: investigate complex C++ and SIMD interfaces |
| SPIRV-Cross, glslang, SPIRV-Tools | Part of the shader/GPU pipeline | Do not assume a trivial drop-in DLL replacement |
| Wuffs, skcms | Internal Skia decoders and color processing | Keep bundled for now |
| DNG SDK, Vulkan Memory Allocator | Bundled utility components | Extract only with additional real consumers and a stable DLL API |

## Controlled externalization procedure

First demonstrate that the existing Skia 153 installed consumer works in both Release and Debug. Then externalize **one** library at a time, starting with Expat, and validate the GN integration against the actual installed DLL and import library. Add a version-specific dependency record to the BuildEngine graph only after the corresponding build has been verified. The project SBOM must distinguish newly externalized components from sources that remain bundled.

**Important:** Bundled upstream source is not automatically interchangeable with a separately built DLL. ICU data, private symbol versions and shader tooling may constrain compatibility. None of the proposed conversions has separate BCC64X PASS evidence so far: **[not verified]**.

[Skia 153](/package/skia/153/migration) · [News](/news/) · [Libraries](/manual/libraries.md)
