# skia 153: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `skia@153` · **Category:** graphics · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Pin Skia to an exact upstream commit and build a broad Windows desktop rendering stack via GN/Ninja/BCC64X. The managed OpenGL, zlib, Brotli, JPEG, PNG, HarfBuzz and FreeType packages replace duplicated Skia producers; other upstream-pinned components remain bundled and are recorded in SBOM. Enable carefully selected CPU, GPU, SVG, PDF, XPS, image and animation paths but exclude unrelated web/mobile integrations.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `opengl@26.2.1`
- `zlib@1.3.2`
- `brotli@1.2.0`
- `libjpeg-turbo@3.2.0`
- `libpng@1.6.58`
- `harfbuzz@14.4.0`
- `freetype@2.14.3`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

No switches of this category are explicitly selected in the parsed build arguments.

### Deliberately disabled switches

No switches of this category are explicitly selected in the parsed build arguments.

### Other selected build features and external settings

No switches of this category are explicitly selected in the parsed build arguments.

### GN arguments for the pinned target graph

```gn
skia_use_bcc64x = true
target_cpu = "x64"
target_cc = "bcc64x"
target_cxx = "bcc64x"
target_link = "bcc64x"
target_ar = "llvm-ar"
python_executable = getenv("CB_PYTHON")
bcc64x_rad_lib_dir = getenv("CB_BDS") + "/lib/win64x/release"
bcc64x_rad_psdk_lib_dir = getenv("CB_BDS") + "/lib/win64x/release/psdk"
extra_ldflags = [getenv("CB_BCC64X_UCRT_COMPAT")]
bcc64x_opengl_lib = getenv("CB_SKIA_OPENGL_LIB")
bcc64x_zlib_include = getenv("CB_SKIA_ZLIB_INCLUDE")
bcc64x_zlib_lib = getenv("CB_SKIA_ZLIB_LIB")
bcc64x_brotli_include = getenv("CB_SKIA_BROTLI_INCLUDE")
bcc64x_brotli_common_lib = getenv("CB_SKIA_BROTLI_COMMON_LIB")
bcc64x_brotli_decode_lib = getenv("CB_SKIA_BROTLI_DECODE_LIB")
bcc64x_libpng_include = getenv("CB_SKIA_LIBPNG_INCLUDE")
bcc64x_libpng_lib = getenv("CB_SKIA_LIBPNG_LIB")
bcc64x_libjpeg_include = getenv("CB_SKIA_LIBJPEG_INCLUDE")
bcc64x_libjpeg_lib = getenv("CB_SKIA_LIBJPEG_LIB")
bcc64x_harfbuzz_include = getenv("CB_SKIA_HARFBUZZ_INCLUDE")
bcc64x_harfbuzz_lib = getenv("CB_SKIA_HARFBUZZ_LIB")
bcc64x_harfbuzz_subset_lib = getenv("CB_SKIA_HARFBUZZ_SUBSET_LIB")
skia_system_freetype2_include_path = getenv("CB_SKIA_FREETYPE_INCLUDE")
skia_system_freetype2_lib = getenv("CB_SKIA_FREETYPE_LIB")
is_official_build = {SkiaOfficialBuild}
is_debug = {SkiaDebug}
is_component_build = true
is_trivial_abi = false
skia_enable_tools = false
skia_build_fuzzers = false
skia_build_rust_targets = false
skia_use_partition_alloc = false
skia_enable_precompile = true
skia_enable_discrete_gpu = true
skia_enable_gpu_debug_layers = false
skia_generate_workarounds = false
skia_enable_ganesh = true
skia_enable_graphite = true
skia_use_gl = true
skia_use_direct3d = true
skia_use_vulkan = true
skia_use_vma = true
skia_enable_spirv_validation = false
skia_use_dawn = false
skia_use_angle = false
skia_use_egl = false
skia_use_epoxy_egl = false
skia_use_webgl = false
skia_use_webgpu = false
skia_enable_fontmgr_win = true
skia_enable_fontmgr_win_gdi = true
skia_use_freetype = true
skia_use_system_freetype2 = true
skia_use_freetype_woff2 = true
skia_use_freetype_svg = true
skia_use_freetype_zlib = true
skia_use_freetype_zlib_bundled = false
skia_enable_fontmgr_custom_directory = true
skia_enable_fontmgr_custom_embedded = true
skia_enable_fontmgr_custom_empty = true
skia_use_fontconfig = false
skia_use_fontations = false
skia_use_icu = true
skia_use_system_icu = false
skia_enable_skunicode = true
skia_use_harfbuzz = true
skia_use_system_harfbuzz = true
skia_pdf_subset_harfbuzz = true
skia_use_client_icu = false
skia_use_bidi = false
skia_use_icu4x = false
skia_use_libgrapheme = false
skia_use_expat = true
skia_use_system_expat = false
```

**GN false switches:** `is_trivial_abi = false`, `skia_enable_tools = false`, `skia_build_fuzzers = false`, `skia_build_rust_targets = false`, `skia_use_partition_alloc = false`, `skia_enable_gpu_debug_layers = false`, `skia_generate_workarounds = false`, `skia_enable_spirv_validation = false`, `skia_use_dawn = false`, `skia_use_angle = false`, `skia_use_egl = false`, `skia_use_epoxy_egl = false`, `skia_use_webgl = false`, `skia_use_webgpu = false`, `skia_use_freetype_zlib_bundled = false`, `skia_use_fontconfig = false`, `skia_use_fontations = false`, `skia_use_system_icu = false`, `skia_use_client_icu = false`, `skia_use_bidi = false`, `skia_use_icu4x = false`, `skia_use_libgrapheme = false`, `skia_use_system_expat = false`. These are build-profile exclusions, not a claim about unsupported upstream capabilities.


## Build architecture, variants and paths

**Selected build flow:** GN/Ninja and explicit native toolchain targets. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- None declared for this category.

**Versioned source locations:**

- `https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator/archive/169895d529dfce00390a20e69c2f516066fe7a3b.tar.gz`
- `https://android.googlesource.com/platform/external/dng_sdk.git/+archive/dbe0a676450d9b8c71bf00688bb306409b779e90.tar.gz`
- `https://chromium.googlesource.com/external/github.com/libexpat/libexpat.git/+archive/6154446fccefbf3ca644894f598969113b0c7bcd.tar.gz`
- `https://chromium.googlesource.com/external/github.com/google/highway.git/+archive/457c891775a7397bdb0376bb1031e6e027af1c48.tar.gz`
- `https://chromium.googlesource.com/chromium/deps/icu.git/+archive/d578f2e8b7bd5938e21cfb6bf15c079e0aa5b738.tar.gz`
- `https://github.com/AOMediaCodec/libavif/archive/55aab4ac0607ab651055d354d64c4615cf3d8000.tar.gz`
- `https://chromium.googlesource.com/codecs/libgav1.git/+archive/5cf722e659014ebaf2f573a6dd935116d36eadf1.tar.gz`
- `https://chromium.googlesource.com/external/gitlab.com/wg1/jpeg-xl.git/+archive/332feb17d17311c748445f7ee75c4fb55cc38530.tar.gz`
- `https://chromium.googlesource.com/webm/libwebp.git/+archive/845d5476a866141ba35ac133f856fa62f0b7445f.tar.gz`
- `https://chromium.googlesource.com/libyuv/libyuv.git/+archive/d248929c059ff7629a85333699717d7a677d8d96.tar.gz`
- `https://android.googlesource.com/platform/external/piex.git/+archive/bb217acdca1cc0c16b704669dd6f91a1b509c406.tar.gz`
- `https://github.com/google/wuffs-mirror-release-c/archive/e3f919ccfe3ef542cfc983a82146070258fb57f8.tar.gz`

**Declared extraction profiles:**

- `zip / skia-2eed75b956045eb8603d3690a1e84bc582a2135d`

**Package-path convention:** `{InstallRoot}/packages/skia/153/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

Skia component exporting and the absolute import-library flag propagation must remain targeted; indiscriminate `--export-all-symbols` is explicitly rejected by the existing integration report. The generic BCC64X UCRT fix is a generated toolchain archive, not a source patch in Skia.

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-d3d-getdesc.patch` | Repair Windows Direct3D descriptor/API signature compatibility under BCC64X. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-debug-spirv-validator.patch` | Correct the conditional wiring of the Debug SPIR-V validation target. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-desktop.patch` | Adapt the Windows desktop code paths for modern BCC64X compiler and SDK semantics. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-gn.patch` | Provide the GN BCC64X compiler/linker/toolchain contract absent upstream. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-harfbuzz-system.patch` | Route shaping to the exact managed HarfBuzz package. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-libgav1-winthreads.patch` | Adapt bundled libgav1 Windows threading compatibility. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-skarenaalloc-export.patch` | Export SkArenaAlloc across the applicable Skia component boundary. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-system-brotli.patch` | Replace Skia's internal Brotli link with the managed producer. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-system-freetype.patch` | Consume the managed FreeType headers and import library. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-system-libjpeg-turbo.patch` | Consume managed libjpeg-turbo without a duplicated bundled producer. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-system-libpng.patch` | Consume managed libpng without a duplicated bundled producer. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-system-link-propagation.patch` | Propagate raw absolute Windows/BCC64X linker flags through GN target chains; GN -l rewriting is unsuitable. |
| `admin/patches/skia/2eed75b956045eb8603d3690a1e84bc582a2135d/bcc64x-zlib-link.patch` | Link the managed zlib variant correctly from the GN graph. |

### Declarative repairs and preparation actions

- `patch-check`
- `patch-apply`
- `system-link-propagation-patch-check`
- `system-link-propagation-patch-apply`
- `desktop-patch-check`
- `desktop-patch-apply`
- `debug-spirv-validator-patch-check`
- `debug-spirv-validator-patch-apply`
- `skarenaalloc-export-patch-check`
- `skarenaalloc-export-patch-apply`
- `zlib-link-patch-check`
- `zlib-link-patch-apply`
- `system-brotli-patch-check`
- `system-brotli-patch-apply`
- `system-libjpeg-turbo-patch-check`
- `system-libjpeg-turbo-patch-apply`
- `system-libpng-patch-check`
- `system-libpng-patch-apply`
- `system-freetype-patch-check`
- `system-freetype-patch-apply`
- `harfbuzz-system-patch-check`
- `harfbuzz-system-patch-apply`
- `libgav1-winthreads-patch-check`
- `libgav1-winthreads-patch-apply`
- `d3d-getdesc-patch-check`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Broad Windows desktop profile. BuildEngine-provided external dependencies: OpenGL 26.2.1 zlib 1.3.2 Brotli 1.2.0 libjpeg-turbo 3.2.0 libpng 1.6.58 HarfBuzz 14.4.0 FreeType 2.14.3 These components are NOT downloaded or rebuilt inside Skia. Their exact package versions are represented by BuildEngine dependencies above. Bundled Skia dependencies remain pinned to the commits selected by this Skia revi
- ========================================================
- Bundled compression / text / image dependencies.
- Generic BuildEngine action. Inspect every pinned sibling download before any expensive source extraction or build starts.
- Populate bundled third_party\externals only.
- BuildEngine package dependencies are deliberately
- GitHub's pinned tarball contains a single D3D12MemoryAllocator-SHA directory, unlike the rootless Gitiles +archive tarball.
- GitHub pinned revision, root folder libavif-55aab4ac0607ab651055d354d64c4615cf3d8000. Verify pinned source before copying and remove the staging root.
- GitHub pinned revision, root folder wuffs-mirror-release-c-e3f919ccfe3ef542cfc983a82146070258fb57f8. Verify pinned source before copying and remove the staging root.
- GitHub pinned revision, root folder SPIRV-Headers-942fe4b988359a0750b79f0ae7ed735994d3147d. Verify pinned source before copying and remove the staging root.
- GitHub pinned revision, root folder SPIRV-Tools-388f5af1a6d4a4df4ab9bf69a1817255952e6ef1. Verify pinned source before copying and remove the staging root.
- BCC64X compatibility / system integration patches.
- Propagate raw BCC64X linker flags from Skia system dependency targets through source_set/group dependency chains to the final shared-library link.
- Restore the pinned Skia Vulkan debug validator source/dependency condition. bcc64x-desktop.patch had incorrectly tied this block to skia_enable_spirv_validation.
- Export SkArenaAlloc across Skia component DLL boundaries. modules/jsonreader is built as its own component DLL and depends on SkArenaAlloc implementation provided by skia.dll.
- Route Brotli, libjpeg-turbo and libpng through the already built and tested BuildEngine packages. Keep each integration in an independent patch so every patch can be checked against exactly one pinned upstream GN file.
- FreeType BuildEngine package integration.
- HarfBuzz BuildEngine package integration.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skia.dll -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skia.dll`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skia.lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skia.lib`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skunicode_core.dll -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skunicode_core.dll`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skunicode_core.lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skunicode_core.lib`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skunicode_icu.dll -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skunicode_icu.dll`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skunicode_icu.lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skunicode_icu.lib`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skshaper.dll -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skshaper.dll`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\skshaper.lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skshaper.lib`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\svg.dll -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\svg.dll`

**Declared smoke tests:**

- `consumer (Release, scope package; source smokes\skia\consumer)`
- `consumer (Debug, scope package; source smokes\skia\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_PYTHON={Tool:python}`
- `CB_BCC64X_UCRT_COMPAT={Tool:bcc64x-ucrt-compat}`
- `CB_SKIA_OPENGL_LIB={InstallRoot}\packages\opengl\26.2.1\lib\win64\{Configuration}\opengl32.lib`
- `CB_SKIA_ZLIB_INCLUDE={InstallRoot}\packages\zlib\1.3.2\include`
- `CB_SKIA_ZLIB_LIB={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibLibraryName}.lib`
- `CB_SKIA_BROTLI_INCLUDE={InstallRoot}\packages\brotli\1.2.0\include`
- `CB_SKIA_BROTLI_COMMON_LIB={InstallRoot}\packages\brotli\1.2.0\lib\win64\{Configuration}\libbrotlicommon.lib`
- `CB_SKIA_BROTLI_DECODE_LIB={InstallRoot}\packages\brotli\1.2.0\lib\win64\{Configuration}\libbrotlidec.lib`
- `CB_SKIA_LIBPNG_INCLUDE={InstallRoot}\packages\libpng\1.6.58\include`
- `CB_SKIA_LIBPNG_LIB={InstallRoot}\packages\libpng\1.6.58\lib\win64\{Configuration}\{LibPngLibraryName}.lib`
- `CB_SKIA_LIBJPEG_INCLUDE={InstallRoot}\packages\libjpeg-turbo\3.2.0\include`
- `CB_SKIA_LIBJPEG_LIB={InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\{Configuration}\{LibJpegLibraryName}.lib`
- `CB_SKIA_HARFBUZZ_INCLUDE={InstallRoot}\packages\harfbuzz\14.4.0\include\harfbuzz`
- `CB_SKIA_HARFBUZZ_LIB={InstallRoot}\packages\harfbuzz\14.4.0\lib\win64\{Configuration}\{HarfBuzzLibraryName}.lib`
- `CB_SKIA_HARFBUZZ_SUBSET_LIB={InstallRoot}\packages\harfbuzz\14.4.0\lib\win64\{Configuration}\{HarfBuzzSubsetLibraryName}.lib`
- `CB_SKIA_FREETYPE_INCLUDE={InstallRoot}\packages\freetype\2.14.3\include\freetype2`
- `CB_SKIA_FREETYPE_LIB={InstallRoot}\packages\freetype\2.14.3\lib\win64\{Configuration}\{FreeTypeLibraryName}.lib`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

## Verification and evidence boundaries

The contract includes one or more upstream tests, validation actions or consumer smokes. Their mere declaration does not establish a successful execution; use the FSM state evidence and process logs for the specific version and configuration.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.
