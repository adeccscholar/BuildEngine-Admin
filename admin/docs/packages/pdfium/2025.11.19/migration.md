# PDFium 2025.11.19: BCC64X migration and integration record

> **Shared translation inventory:** [General BCC64X/GN translation rules](../../../../server-docs/translation-rules.md) records the implemented compiler flags, linker response rewriting, library-specific GN/Skia decisions, and verification limits. This migration records package-specific evidence rather than duplicating the generic adapter table.

[TOC|Content]

**Contract identity:** `pdfium@2025.11.19` · **Category:** documentation · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** Pinned PDFium GN/BCC64X integration; validation pending

Pin the PDFium and Chromium-DEPS revisions and build entirely offline from the downloaded source evidence via GN/Ninja/BCC64X. Use a single complete static pdfium.lib, preserve separate Debug and Release GN build directories, and prevent parallel variants from modifying shared source. The Windows DirectWrite, COM and aligned-memory compatibility changes are confined to the one-time source preparation.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `libjpeg-turbo@3.2.0`
- `libpng@1.6.58`
- `freetype@2.14.3`
- `icu@78.3`
- `lcms2@2.19.1`
- `openjpeg@2.5.4`

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
target_cpu = "x64"
custom_toolchain = "//build/toolchain/bcc64x:bcc64x"
is_clang = false
is_component_build = false
pdf_is_complete_lib = true
clang_use_chrome_plugins = false
pdf_enable_v8 = false
pdf_enable_xfa = false
pdf_use_partition_alloc = false
pdf_use_skia = true
pdf_bundle_freetype = false
pdf_is_standalone = false
is_debug = {PdfiumDebug}
```

**GN false switches:** `is_clang = false`, `is_component_build = false`, `clang_use_chrome_plugins = false`, `pdf_enable_v8 = false`, `pdf_enable_xfa = false`, `pdf_use_partition_alloc = false`, `pdf_bundle_freetype = false`, `pdf_is_standalone = false`. These are build-profile exclusions, not a claim about unsupported upstream capabilities.


## Build architecture, variants and paths

**Selected build flow:** GN/Ninja and explicit native toolchain targets. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- None declared for this category.

**Versioned source locations:**

- `https://chromium.googlesource.com/chromium/src/build/+archive/371ecb089d96d60e5208ec2d25c9b4f0860e556c.tar.gz`
- `https://chromium.googlesource.com/chromium/src/buildtools/+archive/24b075a4d7ea447126ff322e3e8bfecb78012b75.tar.gz`
- `https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp/+archive/14a5b78f39684f85a19690e63c722d6bb680f2cd.tar.gz`
- `https://skia.googlesource.com/skia/+archive/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0.tar.gz`
- `https://chromium.googlesource.com/external/github.com/fastfloat/fast_float/+archive/cb1d42aaa1e14b09e1452cfdef373d051b8c02a4.tar.gz`
- `https://chromium.googlesource.com/chromium/src/tools/win/+archive/24494b071e019a2baea4355d9870ffc5fc0bbafe.tar.gz`
- `https://github.com/chromium/pdfium/archive/a84323421e94f484faca52dd9d027934eba42ab8.tar.gz`

**Declared extraction profiles:**

- `libarchive / pdfium-a84323421e94f484faca52dd9d027934eba42ab8`

**Package-path convention:** `{InstallRoot}/packages/pdfium/2025.11.19/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

PDFium source adjustments are deliberately completed in the single Source state. A previous FreeType `BUILD.gn` write from both Debug and Release caused an apparent race; moving every GN adapter and shared-source mutation ahead of the two parallel builds prevents this class of failure. The complete static archive uses `pdf_is_complete_lib = true`, and GN/llvm-ar uses a response file to handle long object lists.

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Declarative repairs and preparation actions

- `prepare-chromium-build-dir`
- `prepare-chromium-buildtools-dir`
- `prepare-abseil-cpp-source-dir`
- `prepare-skia-source-dir`
- `prepare-fast_float-source-dir`
- `prepare-chromium-tools-win-dir`
- `prepare-icu-gn-adapter`
- `verify-offline-gn-inputs`
- `prepare-jpeg-gn-dir`
- `prepare-zlib-gn-dir`
- `prepare-libpng-gn-dir`
- `install-jpeg-gn-adapter`
- `install-zlib-gn-adapter`
- `install-libpng-gn-adapter`
- `prepare-bcc64x-toolchain-dir`
- `install-bcc64x-toolchain`
- `verify-pdfium-gn-adapters`
- `verify-bcc64x-ninja-toolchain`
- `prepare-pdfium-cmake-package`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Pinned Chromium GN sources from the PDFium DEPS snapshot.
- Materialize the exact PDFium DEPS checkout, not just its download.
- Non-configurable dependencies are provided at their PDFium DEPS revisions.
- Pinned PDFium revision a84323421e94f484faca52dd9d027934eba42ab8 commit timestamp, used as reproducible standalone build timestamp.
- gclient_args.gni is generated deterministically instead of running gclient.
- Fail during Source rather than reporting success before the GN imports from this pinned Chromium revision have been provided. All inputs are local. This check never downloads or updates.
- BCC64X Windows intrinsics require a signed volatile LONG pointer. Keep the COM AddRef/Release ULONG return type unchanged.
- BCC64X SDK exposes the two-argument COM QueryInterface only. Adapt typed Skia COM pointers with exact IID and void** output.
- Windows BCC64X is Clang-based: compiler identity must not select POSIX allocation. Change alloc/free/header guards together.
- Prepare shared GN adapters exactly once in Source. Build variants generate separate outputs without modifying the shared workspace.
- Verify source-level GN targets used by both build variants.
- Validate source evidence on resumed builds before generating GN targets.
- Use the locally provisioned Windows toolchain; avoid Chromium depot_tools.
- Verify the GN inputs installed by the single Source scope, read-only.
- Upstream: emit a single complete static archive, rather than a source_set.
- Validate the GENERATED build, not merely the requested GN args.
- Inspect commands reachable from the PDFium target, not all unrelated Chromium host-toolchain rules.
- Do not declare success without a generated GN build and PDFium target.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\obj\pdfium.lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\pdfium.lib`
- `{Workspace}\public -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{BuildFileDir}\cmake\pdfium\PDFiumConfig.cmake -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\cmake\PDFium\PDFiumConfig.cmake`
- `{Workspace}\LICENSE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses\pdfium\LICENSE`
- `{BuildFileDir}\patches\pdfium\2025.11.19\bundled-components.json -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\sbom\bundled-components.json`

**Declared smoke tests:**

- `consumer (Release, scope package; source smokes\pdfium\consumer)`
- `consumer (Debug, scope package; source smokes\pdfium\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_PYTHON={Tool:python}`
- `CB_GN_ARGUMENT_ADAPTER={BuildFileDir}\\programs\\bcc64x\\gn_argument_adapter.py`
- `DEPOT_TOOLS_WIN_TOOLCHAIN=0`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_PDFIUM_ICU_INCLUDE={InstallRoot}\\packages\\icu\\78.3\\include`
- `CB_PDFIUM_ICU_LIB={InstallRoot}\\packages\\icu\\78.3\\lib\\win64\\{Configuration}\\{PdfiumIcuImport}.lib`
- `CB_PDFIUM_FREETYPE_INCLUDE={InstallRoot}\packages\freetype\2.14.3\include\freetype2`
- `CB_PDFIUM_FREETYPE_LIB={InstallRoot}\packages\freetype\2.14.3\lib\win64\{Configuration}\{PdfiumFreeTypeImport}.lib`
- `CB_PDFIUM_ZLIB_INCLUDE={InstallRoot}\packages\zlib\1.3.2\include`
- `CB_PDFIUM_ZLIB_LIB={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{PdfiumZlibImport}.lib`
- `CB_PDFIUM_PNG_INCLUDE={InstallRoot}\packages\libpng\1.6.58\include`
- `CB_PDFIUM_PNG_LIB={InstallRoot}\packages\libpng\1.6.58\lib\win64\{Configuration}\{PdfiumPngImport}.lib`
- `CB_PDFIUM_JPEG_INCLUDE={InstallRoot}\packages\libjpeg-turbo\3.2.0\include`
- `CB_PDFIUM_JPEG_LIB={InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\{Configuration}\{PdfiumJpegImport}.lib`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

**Static PDFium:** The complete `pdfium.lib` uses an installed `PDFium::pdfium` package contract, public C API headers and a consumer smoke that creates/renders a small PDF in memory. Static dependency closure is still subject to actual link verification; do not infer its completeness from `ready`.

## Verification and evidence boundaries

The contract includes one or more upstream tests, validation actions or consumer smokes. Their mere declaration does not establish a successful execution; use the FSM state evidence and process logs for the specific version and configuration.

- The supplied FSM trace on 17 October 2026 reports Release/Debug Build PASS (736.2 s / 736.5 s), per-variant Install PASS, metadata PASS, Doxygen PASS and Ready PASS. The newly added external consumer smoke has **not** been shown passing.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.

## Native BCC64X libc++ ABI and installed-package link closure (17 October 2026)

The first installed-package consumer smoke failed while linking the executable in Release and Debug. The linker reported 22 errors and first identified `std::__Cr::__libcpp_verbose_abort(char const*, ...)`. The PDFium archive, installation, publication and Doxygen had completed. This was not a compiler error.

**ABI investigation:** Chromium uses a private `__Cr` libc++ ABI namespace to separate its own standard library implementation from the system implementation. PDFium is instead linked into a native Embarcadero BCC64X executable using the BCC64X libc++ runtime. The pinned GN profile now explicitly sets `use_custom_libcxx = false`, alongside `is_component_build = false` and `pdf_is_complete_lib = true`, to avoid mixing distinct standard library ABIs. Its effect requires a fresh GN compilation and a consumer smoke. **[not verified]**

**External dependency propagation:** The PDFium GN environment binds the exact managed ICU, FreeType, libpng, libjpeg-turbo and zlib version-specific import libraries. The installed CMake target `PDFium::pdfium` now publishes those same five imports for Release/Debug via `INTERFACE_LINK_LIBRARIES`, using absolute BuildEngine package paths with fail-fast missing-file checks. No host discovery is used.

**Scope:** LCMS2 and OpenJPEG are declared PDFium package dependencies but not currently passed to GN via the same five explicit import variables; do not add more libraries without examining the complete remaining unresolved symbol list.

**Runtime policy:** Consumers continue to use the mandatory dynamic BCC64X runtime (`-tR`). Do not provide a synthetic `std::__Cr::__libcpp_verbose_abort`, ignore unresolved errors, or link a foreign libc++ archive.

**Verification pending:** Re-run both native installed-package consumer smokes; inspect their complete `02-cmake-build.log` files and confirm that the Chromium ABI mismatch has actually disappeared.

**FSM observation:** The technical `ready` action was marked PASS while smoke actions were failing. The library must not be considered ready until the required smokes succeed; this requires a separate sequencing/presentation correction in the FSM.

## Consumer CMake package-root correction (17 October 2026)

Both Release and Debug consumer CMake configure attempts failed in `PDFiumConfig.cmake:39` at the explicit managed-import existence check. The configuration initially computed `_PDFIUM_PACKAGES_ROOT` as the parent of the PDFium *version* root, which resolves to `<install>/packages/pdfium`, **not** `<install>/packages`. All sibling imports (ICU, FreeType, libpng, libjpeg-turbo, zlib) were consequently looked up below the wrong package coordinate.

**Correction:** `get_filename_component(_PDFIUM_PACKAGES_ROOT "${_PDFIUM_ROOT}/../.." ABSOLUTE)`. This changes only consumer path resolution; no PDFium/GN rebuild or upstream source patch is needed for this error. Existing absolute version/configuration imports and failure-on-missing-file checks remain unchanged. Actual file names and further unresolved symbols are still subject to the next consumer configure/link test **[not verified]**.

The separately uploaded `02-cmake-build(3).log` is the older 21:11:36 UTC linking run; it does not contain the newer CMake line-39 configure error and cannot identify any additional current missing import file.


## Consumer link: system FreeType glyph table and Abseil Windows threading (18 October 2026)

**Recorded evidence:** The 18 October 2026 installed-package Release consumer link in `02-cmake-build(4).log` resolves the previously missing ICU, FreeType, PNG, JPEG and zlib imports and no longer reports Chromium's private `std::__Cr` libc++ symbols. The remaining missing symbols are `ft_adobe_glyph_list` (referenced by `pdfium.lib(fxge.fx_freetype.o)`) and three pthread TLS functions (`pthread_setspecific`, `pthread_key_create`, `pthread_getspecific`) referenced by the bundled `pdfium.lib(base.thread_identity.o)`.

**Root cause 1: PDFium's glyph-name table.** The pinned `core/fxge/BUILD.gn` explicitly defines `DEFINE_PS_TABLES_DATA` for `if (is_component_build || use_system_freetype)`. This definition emits `ft_adobe_glyph_list` from PDFium's own `third_party/freetype/include/pstables.h`; the external FreeType DLL does not export this internal data table. Our preceding GN profile had `pdf_bundle_freetype = false` but left `use_system_freetype = false` at its default, so the generated target failed to define the table. The fix is `use_system_freetype = true` while retaining `pdf_bundle_freetype = false` and the managed FreeType 2.14.3 import path. No additional FreeType producer or fake glyph table is required.

**Root cause 2: Abseil's TLS mode.** The exact Chromium-DEPS Abseil sources in PDFium's complete static archive selected a POSIX `pthread_*` identity mode when built through the custom BCC64X GN toolchain. The pinned Abseil implementation exposes `ABSL_FORCE_THREAD_IDENTITY_MODE`, with mode `2` choosing its C++11 `thread_local` plus RAII cleanup implementation. The versioned PDFium BCC64X C++ toolchain therefore now compiles C++ targets with `-DABSL_FORCE_THREAD_IDENTITY_MODE=2`. This affects only the PDFium toolchain (including its bundled Abseil sources), not the separately installed BuildEngine Abseil package. Do not add a generic pthread library or manually synthesize these symbols on Windows.

**Validation required:** Regenerate the GN graph and rebuild both `pdfium.lib` variants. An existing archive still contains the old objects and cannot be fixed by rerunning the consumer smoke alone. Then verify `pdfium-consumer.exe` links, loads, renders its in-memory test PDF and releases resources in Debug and Release. These two corrections are **[not verified]** by a new local BCC64X run.

## Early Skia FreeType include failure and adapter correction (18 October 2026)

**Observed failure:** The first rebuild using `use_system_freetype = true` failed in both BCC64X variants at `third_party/skia/src/ports/SkFontHost_FreeType.cpp:50` because `ft2build.h` could not be resolved. This is a compiler include-path failure, not proof that the installed FreeType package or its import library is missing.

**Root cause:** Chromium's `//build/config/freetype` GN group has distinct bundled and system-FreeType branches. The previous `bind-local-freetype` preparation only replaced the bundled branch (`public_deps = [ "//third_party/freetype" ]`); it left the system branch's `//build/linux:freetype_from_pkgconfig` configuration unchanged. Activating `use_system_freetype` therefore bypassed the managed BuildEngine include path. Previously the bundled branch had worked.

**Correction:** The existing XML-driven Source action now replaces **both** upstream branch alternatives with `public_configs = [ "//build/config/freetype:buildengine_freetype" ]`. The config continues to read `CB_PDFIUM_FREETYPE_INCLUDE` and `CB_PDFIUM_FREETYPE_LIB`, pointing to the exact managed FreeType 2.14.3 package. The action is idempotent, rejects unexpected duplicates, and rejects an unmodified Linux pkg-config reference; there is no new standalone helper script or change to the installed FreeType producer.

**Early validation:** The GN/Ninja command verification now inspects the reachable `SkFontHost_FreeType.cpp` compile command and asserts that its effective include arguments contain the managed FreeType header directory. This ensures that missing `ft2build.h` search paths are detected before a lengthy concurrent Debug/Release compilation. The check does not disable the FreeType renderer or the Adobe glyph-name table. **[not verified]** A new BCC64X run must confirm the generated GN graph, actual compilation and consumer link.

## Installed consumer evidence: Release validation and Debug linker (18 October 2026)

**Provided logs (2026-10-17 22:54 UTC):** Release executed and reported `SMOKE|CHECK|memory-pdf-render|PASS|installed PDFium` with exit code 0. The generic validator rejected the run only because `SMOKE|RESULT` was absent. The versioned consumer source now emits the mandatory terminal record in addition to the check, without modifying the shared validator.

**Debug consumer link:** The installed PDFium archive has unresolved `fxcrt::StringViewTemplate<char/wchar_t>` methods, including `GetLength()` and the null-terminated constructor, plus numerous internal bundled LittleCMS2 functions such as `_cmsQuickSaturateWord`, `_cmsInitMutexPrimitive`, `IsInkSpace` and `_cmsToFixedDomain`. Both missing-symbol families arise *inside* `pdfium.lib`, not from missing ICU/FreeType/PNG/JPEG/zlib import paths.

**LCMS2 source ownership correction:** The pinned PDFium repository contains its own `third_party/lcms/src/lcms2_internal.h`, with `cmsINLINE` selecting MSVC/Borland-style `__inline` before the Clang case. The independent BuildEngine LittleCMS2 2.19.1 package had already solved this compiler-family mismatch with a `static inline` priority under `__clang__`. A checked, idempotent source-phase adapter now applies the identical macro selection to PDFium's bundled copy. The adapter changes neither producer nor separately installed LCMS2 and precedes both build variants. **[not verified]** Debug/Release rebuilt archive and consumer-link evidence is required to establish success.

**C++ template investigation pending:** The pinned PDFium header `core/fxcrt/string_view_template.h` declares two `extern template class StringViewTemplate<...>` specializations; `bytestring.cpp` and `widestring.cpp` provide explicit template instantiations. An unresolved inline member is not sufficient evidence to rewrite the declarations or synthesize functions. Inspect the archive's exported and undefined symbol tables and the corresponding compilation commands first. In particular, distinguish missing definition emission, C++ ABI spelling, and archive-member extraction. Do not suppress missing symbols or add duplicate strong definitions.

**Rebuild scope:** The LCMS2 source adjustment changes the complete PDFium producer archive and requires regeneration/rebuild of Debug and Release. The smoke terminal record is consumer-only. Both changes remain **[not verified]** until a new native BCC64X run.

## Remaining four FXCRT string-view symbols after LCMS2 repair (18 October 2026)

**Evidence:** The supplied 23:06 UTC Debug consumer link now reports only four unresolved members: `fxcrt::StringViewTemplate<char/wchar_t>::GetLength() const` and `fxcrt::StringViewTemplate<char/wchar_t>::StringViewTemplate(T const*)`. Earlier bundled LittleCMS2 missing symbols are absent from this new error log. This is narrower evidence, not proof that either full consumer smoke passed.

**Pinned source ownership:** `core/fxcrt/string_view_template.h` defines the affected methods inline and contains `extern template class StringViewTemplate<char>` and `<wchar_t>`; `bytestring.cpp` and `widestring.cpp` explicitly instantiate the corresponding classes. The source-phase BCC64X adapter now removes only those two extern-template suppressions, retaining both upstream explicit instantiations, so normal use-site inline COMDAT emission is possible. The adapter fails on an unexpected source version and is idempotent. No synthetic implementations, ignored unresolved symbols or changes to the generic toolchain were introduced.

**Verification boundary:** This is a targeted correction derived from source and linker evidence; **[not verified]** BCC64X archive symbol emission, Debug consumer linking and both consumer smoke results require a new native run. The PDFium library timestamp was incremented to invalidate stale state and rebuild the changed producer.

## Required shared-library correction (18 October 2026)

**Architectural decision:** PDFium is shipped and consumed as a **native BCC64X Windows DLL**, not a complete static archive. Earlier `pdf_is_complete_lib=true` and `PDFium::pdfium STATIC IMPORTED` integration was a violation of this project's shared-library boundary, regardless of successful consumer smokes on that artifact.

**Pinned upstream mechanism:** `component("pdfium")` in PDFium's `BUILD.gn` switches to a shared library with GN `is_component_build=true`; `pdf_is_complete_lib=false` removes the complete-static override. `fpdfsdk/BUILD.gn` consumes the upstream public-headers implementation config that provides `FPDF_IMPLEMENTATION` for its exported C API.

**Compiler and linker:** Version-pinned BCC64X GN toolchain `solink` now uses `-tD -tR`, emits a Windows DLL plus a deterministic COFF import library via `-Wl,--out-implib`, writes long link arguments to a response file, and declares both outputs to GN. Link dependencies use the import library; runtime uses the DLL. `WIN32` is explicitly defined for PDFium's upstream `FPDF_EXPORT` branches. Source preparation remains single-writer before Debug and Release variants.

**Early failing gate:** Generated-Ninja preflight requires a BCC64X DLL link command with import-library emission, dynamic RTL, and `COMPONENT_BUILD`/`FPDF_IMPLEMENTATION` defines for `fpdfsdk/fpdf_view.cpp`. This check precedes expensive compilation.

**Installed and published artifacts:** Each variant requires and installs `bin/win64/<Configuration>/pdfium.dll` and `lib/win64/<Configuration>/pdfium.lib`; the Release publish scope also includes the DLL. Both artifacts must exist before the package can progress. A complete static PDFium archive is no longer installed or exported.

**External consumer contract:** `PDFium::pdfium` is `SHARED IMPORTED` with separate `IMPORTED_LOCATION_<CONFIG>` and `IMPORTED_IMPLIB_<CONFIG>`, and the consumer gets `COMPONENT_BUILD`. Runtime smoke checks the loaded `pdfium.dll` and public C exports via `GetModuleHandleW`/`GetProcAddress`, in addition to rendering the memory PDF. Dependency closure and runtime DLL search paths are supplied by BuildEngine's existing package-scope smoke environment.

**Status:** Changes reflect a checked static-source contract and toolchain documentation. **[nicht verifiziert]** GN regeneration, Windows BCC64X component linking, DLL exports and both consumer-smoke runs still require real local evidence. Never report this as a completed DLL build without that evidence.

## Shared PDFium GN debug CRT option (18 October 2026)

The native Debug build failed during argument translation before invoking the compiler: `gn_argument_adapter.py` rejected MSVC `/MDd`. The complete supplied log contains the same unsupported-switch diagnostic for all reported failed translation units; no BCC64X compiler diagnostic was reached. Chromium correctly selects `/MDd` for component Debug and `/MD` for component Release. The existing adapter already maps `/MT` and `/MTd` to the project's dynamic BCC64X RTL `-tR`; its explicit translation set now also contains `/MD` and `/MDd`. The PDFium generated-Ninja preflight imports the real adapter and tests all four switches plus the expected generated GN switch per build variant **before** running Ninja. This retains the project rule against MSVC CRT use and static runtime boundaries. The adapter itself is a build-time input, not a PDFium source patch. **[not verified]** A subsequent native DLL build and smoke run must establish completion.

## Skia dllimport/inline failure: isolate PDFium DLL instead of global component mode

The actual Debug and Release compiler diagnostics report `'dllimport' attribute ignored on inline function [-Werror,-Wignored-attributes]` in Skia's `include/private/base/SkAssert.h`. The pinned Skia `SkAPI.h` applies Windows DLL import/export decoration when `SKIA_DLL` is set; Skia's `BUILD.gn` enables `SKIA_DLL` under **global** `is_component_build=true`. This is a wrong build graph for the BuildEngine target: the intent is one exported PDFium DLL, not a graph of Chromium/Skia component DLLs.

**Corrected ownership:** `is_component_build=false` restores the previously proven Chromium/Skia non-component compilation. A version-pinned Git patch replaces only PDFium's `component("pdfium")` target with GN `shared_library("pdfium")`. It adds `BUILDENGINE_PDFIUM_DLL` alongside the existing `FPDF_IMPLEMENTATION` in PDFium's C-API implementation config, and extends the public `FPDF_EXPORT` condition to use this local producer-only define. The external CMake consumer still defines upstream `COMPONENT_BUILD` for its normal DLL imports. This leaves the installed PDFium artifact **dynamic**, with the existing BCC64X `solink` producing the import library, while removing `SKIA_DLL` and `COMPONENT_BUILD` from its third-party compile graph.

The source-phase patch has `git apply --check`, `git apply`, and `git apply --reverse --check` steps and targets exactly pinned upstream paths. Generated-Ninja preflight rejects global component leakage and requires the PDFium-specific API export defines before triggering compilation. The existing BCC64X runtime adapter still maps Chromium's `/MT[d]` flags to project-defined dynamic `-tR` (no MSVC static CRT is used). **[nicht verifiziert]** GN and DLL linking require an actual BCC64X run.

## MSVC CET Shadow Stack linker flags under BCC64X (18 October 2026)

**Evidence:** Both PDFium component DLL link builds failed with `bcc64x: error: no such file or directory: '/CETCOMPAT'`. This is a link-driver argument error, not a missing source file. Chromium enables `/CETCOMPAT` from its Windows `cet_shadow_stack` linker config (under `enable_cet_shadow_stack`). The GN BCC64X C/C++ commands already use the approved `gn_argument_adapter.py`, but `solink` had previously passed the GN response file directly to the Clang GNU-style link driver without adapting MSVC-format linker switches.

**Actual LLVM 20.1.7 support:** `lld/MinGW/Options.td` defines `-Xlink` to forward an option to the COFF linker, `lld/MinGW/Driver.cpp` forwards these options to the COFF driver, and `lld/COFF/Options.td` implements `/cetcompat`. The existing adapter therefore now translates GN linker-response MSVC flags to `-Xlinker -Xlink=/FLAG`, including `/CETCOMPAT`. That uses Clang's `-Xlinker` to pass the native `-Xlink=/FLAG` option to the MinGW driver, while preserving comma-containing flags such as `/OPT:REF,ICF`. No security switch is suppressed. The COFF driver remains responsible for validating support. GNU flags, relative GN object files, and Windows drive-qualified paths remain unchanged.

**Integration:** Both `link` and `solink` in the versioned PDFium GN BCC64X toolchain now invoke the *existing* argument adapter in `--link-rsp` mode, write a deterministic translated response file next to the GN original, then run BCC64X with dynamic RTL. A generated-Ninja preflight checks the mode plus direct tests of /CETCOMPAT, /NXCOMPAT, /DYNAMICBASE, /FIXED:NO and /OPT:REF,ICF before the expensive Ninja build. `is_component_build=false` and `shared_library("pdfium")` are unchanged.

**Verification boundary:** The mapping is source-verified against LLVM 20.1.7, but a native BCC64X PDFium DLL link and both consumer smokes remain **[not verified]**.

## Windows import-library naming in the BCC64X GN response adapter (18 October 2026)

**Observed native linker failure:** Both DLL variants completed their compilation phase and reached LLD, which rejected `-ladvapi32.lib` with `unable to find library -ladvapi32.lib`. This is the GN default `-l` library switch applied to PDFium's Windows-style `libs = [ "advapi32.lib", "gdi32.lib", "user32.lib" ]` entries. It is not evidence that the Windows SDK or those system libraries are missing.

**Verified LLVM 20.1.7 MinGW lookup semantics:** `lld/MinGW/Driver.cpp::searchLibrary` resolves `-l<name>` by searching for `lib<name>.dll.a`, `lib<name>.a` and `<name>.lib` in configured library directories. GN's `-ladvapi32.lib` therefore requests the wrong logical library name and can search for `advapi32.lib.lib`. The existing GN linker response adapter now normalizes the exact token form `-lname.lib` to `-lname` (case-insensitive), so LLD may find `name.lib` using its own established library search. There is no change to Windows SDK paths or dynamic RTL, and no static PDFium fallback.

**Safety boundary:** No transformation of `-l:exact.lib`, existing `-lname`, or path-qualified `D:/.../libname.lib` tokens. The PDFium generated-Ninja preflight checks advapi32, gdi32, user32 and ole32 normalization plus exact-filename and absolute-path preservation before launching compilation. This is a link-only adapter correction; the library's source timestamp is deliberately unchanged so completed source evidence is not invalidated solely to alter a response-file translation.

**Status:** The six focused normalizer cases were executed locally; the native BCC64X DLL link and consumer smokes are **[not verified]** until the next run.

## GN full Windows import-library paths in BCC64X response files (18 October 2026)

Both variants reached DLL linking, but LLD reported missing `-lD:localembarcaderotest_v3installpackagesfreetype...` libraries. The diagnostic already shows the drive-qualified FreeType import-library path with backslashes removed by response-file parsing; it does not establish that the underlying import library is absent. The GN tool's default `-l` prefix was applied to an absolute Windows path. In LLD MinGW, `-l<name>` invokes its own basename search rather than supplying an exact file.

**Correction:** The existing BuildEngine BCC64X `gn_argument_adapter.py` translates GN drive-absolute `-lD:\\...\\libfreetype.lib` tokens to direct `D:/.../libfreetype.lib` linker inputs before the driver expands the response file. It preserves quoting for paths containing spaces. The established `-lname.lib` to `-lname` normalization for system libraries remains separate. Existing exact `-l:filename.lib` and normal `-lname` tokens, and the MSVC-to-LLD CET/ASLR option forwarding, remain intact. An unmatched drive-qualified `-lX:` fails explicitly rather than being passed through silently.

**Validation:** Nine isolated regression cases (both observed FreeType variants, forward-slash path, both quoted conventions, system libraries, exact filename, already-normalized full path, and /CETCOMPAT forwarding) passed against the new implementation logic. The PDFium XML preflight additionally checks both actual FreeType path patterns. Only the adapter and preflight change; the pinned source content, GN toolchain, PDFium DLL-only artifact contract and source timestamp remain unchanged. **[nicht verifiziert]** The actual BCC64X DLL link and consumer smokes require a subsequent native execution.

## Debug PDFium DLL: bundled Abseil BigUnsigned<4>::Digits10 (18 October 2026)

**Confirmed user log:** Native PDFium 2025.11.19 **Release DLL build PASSED** in 13.8 s. Debug reaches LLD and fails with `undefined symbol: absl::strings_internal::BigUnsigned<4>::Digits10()`. This is not a library-path failure, and no evidence suggests the successful Release variant needs a linker or GN-configuration rewrite.

**Existing repository solution reused, not reinvented:** Standalone Abseil 20260817.0 already integrates `admin/patches/abseil/20260817.0/bcc64x-charconv-digits10.patch`, addressing the same BCC64X template inline/linkage failure. Its `charconv_bigint.h` and `charconv_bigint.cc` hunks replace the two runtime expressions using `Digits10()` with an enumerator `kDigits10` while retaining the original constexpr method for API compatibility. PDFium uses a separate, pinned Chromium-DEPS Abseil checkout, so standalone Abseil patches do not automatically reach it.

**XML source-only correction:** Immediately after extracting `third_party/abseil-cpp`, the PDFium Source FSM applies the **same existing** Abseil patch by `git apply --directory=third_party/abseil-cpp`. A preliminary `--check` and subsequent `--reverse --check` verify that the intended, unmodified source locations match and that the patch was applied. The original patch file and standalone Abseil project are unchanged; no separate helper script, link-time synthetic symbol, different Abseil DLL or static PDFium fallback was introduced. A local Git check confirmed `--directory` with `--no-index` handles nested checkout paths. The Chromium Abseil-source pattern matches the version-pinned 20250814.1 Abseil header and source comparison, but the exact downloaded DEPS checkout cannot be compiled here.

**Build invalidation:** PDFium's library timestamp increases from `2026-10-18T00:32:00Z` to `00:33:00Z` because the nested header and source compiled into PDFium have changed. The complete dependency graph does not require rebuilding other 56 libraries. **[nicht verifiziert]** Actual Debug DLL link and both smokes remain to be confirmed by a native BCC64X run.

## Application-local DLL deployment and reusable CMake consumer bootstrap (18 October 2026)

The BuildEngine consumer tree now provides `BuildEngineRuntime.cmake` and `BuildEngineStageRuntime.cmake` with `adecc_stage_runtime(target PACKAGE_ROOTS ...)` and `adecc_install_application(target DESTINATION ...)`. Staging inspects exact selected package roots and the current Debug/Release bin subtree, validates AMD64 PE/COFF DLL headers and rejects duplicate DLL names with differing SHA-256 digests before copy. It never selects a new library version or modifies the producer build. The application-local copy makes Windows loader resolution deterministic relative to the executable; this addresses one class of `STATUS_INVALID_IMAGE_FORMAT` problems but cannot by itself certify all transitive imports or runtime ABI compatibility.

The generic smoke builder supplies the already-computed XML dependency-closure package roots to CMake as `ADECC_RUNTIME_PACKAGE_ROOTS`. PDFium's installed package consumer now invokes the common staging helper after its link. `BuildEngineConsumer.cmake` includes the runtime helper for published applications. Full OS-level dependency tree introspection and portable Stage-0 bootstrap remain separately planned; the existing StackBuilder archive/lock and `bootstrap/Bootstrap.cpp` are not replaced.

**[nicht verifiziert]** Native Windows PE64 staging, deployed PDFium smoke load and Debug/Release execution still require actual BCC64X/Windows runs.

## Windows 0xC000007B loader diagnostic preflight (18 October 2026)

**Observation:** The 2026-10-18 `04-smoke-validation(4).log` still records `process_failed`, exit code `3221225595` (`0xC000007B`, `STATUS_INVALID_IMAGE_FORMAT`), with zero smoke checks and no stdout/stderr. The preceding CMD shell-pipeline staging defect is no longer evidenced by this log, but **there is no evidence that PDFium or its transitive DLL imports actually load**. A PE header/machine check of staged DLLs alone cannot prove that.

**Added diagnostic:** `admin/programs/bcc64x/RuntimeLoaderProbe.cpp` is a generic, native Windows-only, independent executable built by the PDFium smoke CMake project. It is NOT linked against PDFium. The existing XML `<smoke>` declares `ADECC_RUNTIME_PROBE_DLL=pdfium.dll` and a `<run executable="pdfium-loader-probe.exe"/>` pre-run for Debug and Release. The probe checks the exact DLL adjacent to the EXE, reports the PE32+ x64 header and its static PE import names, and calls `LoadLibraryExW` with `LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS`. On failure it records the actual Win32 error and message; on success it records the loaded module location. The pre-run is a separate process, avoiding inserting experimental third-party DLLs into the BuildEngine host process. BuildEngine writes its findings to `03-run-01-pdfium-loader-probe.log` and only subsequently executes the existing SMOKE protocol.

**Staging provenance:** `BuildEngineStageRuntime.cmake` additionally logs each selected local x64 DLL's exact source filename and SHA-256, to distinguish wrong-version and same-name DLL substitution without altering the package dependency closure or using PATH for version selection.

**Limits:** A failure from a DLL's *transitive* import may still surface at the root `LoadLibraryExW`; listed import names and a Windows loader trace are required to identify its precise child. If the probe EXE itself fails with `0xC000007B`, the issue is in the application/BCC64X runtime startup rather than any direct PDFium import. This is a diagnostic enhancement, **not a claimed fix** for the underlying incorrect image. The native BCC64X loader test is [nicht verifiziert].

## Ordered library-by-library LoadLibraryExW probe (18 October 2026)

The PDFium consumer's generic `RuntimeLoaderProbe.cpp` now reads an ordered `ADECC_RUNTIME_PROBE_DLLS` semicolon-separated list from the XML smoke environment. It probes each known binary independently, **continues on failure**, reports the PE x64 header and direct imports, attempts explicit `LoadLibraryExW`, reports the resolved module path and individual PASS/FAIL (with hex Win32 code), and ends with an overall passed/failed count. Each successful load is released before the next test so the final PDFium check reproduces a fresh load. The actual listed names, for both Debug and Release, are `libc++-370.dll → libunwind-370.dll → libz.dll → libjpeg.dll → libicuuc78.dll → libfreetype.dll → pdfium.dll`: the two Embarcadero runtime DLLs, the four BuildEngine dependencies actually imported by PDFium, then `pdfium.dll`. It intentionally does not try to load Windows API-set contracts individually.

The probe checks application-local DLLs first. If a named Embarcadero runtime is already loaded into the probe, it resolves and probes **that exact module path** rather than searching for an arbitrary DLL on PATH. It reports a missing file or module explicitly and prints ASCII-only Win32 error names to avoid the previously observed localized `FormatMessageW` output causing a UTF-8 decoding error in BuildEngine's logging. No changes to the installed packages, BuildEngine loader, GN link rules, or PDFium C API were necessary. The PDFium source timestamp is unchanged: only the consumer smoke executable and XML smoke environment change.

**Verification:** Review of the source establishes ordering, diagnostic branch coverage and no PDFium import by the probe itself. Native BCC64X compile and dynamic loads require an actual Windows run and remain [nicht verifiziert].

## Next Release loader probe: FreeType transitive imports (18 October 2026)

The newly uploaded native Release loader log proves `libc++-370.dll`, `libunwind-370.dll`, `libz.dll`, `libjpeg.dll`, and `libicuuc78.dll` each load successfully. `libfreetype.dll` fails `LoadLibraryExW` with **Win32=126 (ERROR_MOD_NOT_FOUND)**; its direct import list identifies `bzip2.dll`, `libpng16.dll`, `libharfbuzz.dll` and `libbrotlidec.dll`, alongside the already-good `libz.dll`. `pdfium.dll` separately fails Win32=193; it remains unproven whether this is solely consequent on FreeType or a separate broken image.

Without touching PDFium's compiled artifacts or the smoke probe C++ source, the **Release** XML smoke's ordered probe list now includes the actual observed direct FreeType imports (and Brotlidec's common runtime, plus ICU data) before FreeType. This will identify which particular staged DLLs are missing or cannot be loaded by filename, with the existing module-by-module PASS/FAIL diagnostics. The root list is declarative XML; the engine executes unchanged. The **Debug list stays unchanged** until a Debug PE import log verifies its suffix-sensitive names (`bzip2d.dll`, `libicuuc78d.dll`, etc.) instead of guessing. No library Source/Build timestamp is changed, because this affects only smoke inputs.

**Caution:** PE direct imports alone do not prove the full nested dependency chain or Windows API-set resolution. Failure 193 in PDFium may have an independent cause; inspect it again once FreeType's 126 has been resolved. Native execution of the expanded Release test is [nicht verifiziert].

## HarfBuzz loader error 126: explicitly stage Embarcadero pthread runtime (18 October 2026)

The uploaded 13-module Release loader probe reports 10 PASS and three FAIL: `libharfbuzz.dll` with Win32 126, `libfreetype.dll` with Win32 126, and `pdfium.dll` with Win32 193. All checked Brotli, BZip2, libpng, ICU, zlib and JPEG modules load successfully. HarfBuzz's direct PE imports include `libwinpthread-1-370.dll`, which the BuildEngine package-only staging did not provide. This is an evidence-based missing-runtime hypothesis, not yet proof of the *resolved* Windows module path.

The shared `BuildEngineRuntime.cmake` now accepts optional `TOOLCHAIN_DLLS` (or the XML-provided `ADECC_RUNTIME_TOOLCHAIN_DLLS`). The native selected RAD Studio installation is resolved via `CB_BDS/bin64`. A second newline-delimited CMake manifest passes only the named Embarcadero runtime files to the existing stage script. Staging verifies the same Win64 PE headers, SHA-256 provenance, and filename collisions as third-party DLLs, without copying all files from the IDE directory. Explicitly missing DLLs fail early. Both PDFium smoke variants now request `libwinpthread-1-370.dll` and probe it separately; the core runtime contract `-tR` is unchanged. Distribution of RAD Studio runtimes must follow the Embarcadero licensing terms.

**Not yet verified:** Presence of the declared DLL at `CB_BDS/bin64` on the Windows host, successful subsequent HarfBuzz/FreeType loads, or whether PDFium's distinct error 193 has an independent cause.

## Kernel PE mapping diagnostic following 13/14 PASS (18 October 2026)

After making the explicitly required Embarcadero `libwinpthread-1-370.dll` available, the uploaded Release loader probe reports **13/14 PASS**: pthread, HarfBuzz, FreeType and all separately checked dependencies load. The only remaining failure is `pdfium.dll` itself (`LoadLibraryExW`, Win32 193 / `ERROR_BAD_EXE_FORMAT`), despite reporting x64 PE32+ headers. Therefore further recursive DLL staging is **not justified** by this evidence; the GN-linked PDFium image must be scrutinized.

For any `ERROR_BAD_EXE_FORMAT`, the generic native C++ RuntimeLoaderProbe now logs the PE file's size, DLL characteristic, image/header sizes, entry point, alignments, base relocations/import/exception directories and section raw/RVA bounds. Then it tests `CreateFileMappingW(PAGE_READONLY | SEC_IMAGE_NO_EXECUTE)` followed by `MapViewOfFile` on that same DLL. This asks Windows to map the PE as an image **without resolving its imports or invoking its DLL entry point**. If kernel image mapping fails, focus on the PDFium PE/LLD output. If mapping succeeds but `LoadLibraryExW` remains 193, investigate the dynamic loader's processing of imports/relocations and other loader-only checks. Neither result on its own proves a specific root cause. Diagnostics are informational and do not modify source, GN link parameters, installed packages or the runtime DLL closure.

**Native test pending.** The previous Release evidence demonstrates import closure but does not prove the DLL image is valid.

## CFG and TLS image metadata investigation (18 October 2026)

The uploaded Release loader evidence shows all 13 prerequisite DLLs load successfully and the 8,841,216-byte `pdfium.dll` maps via `SEC_IMAGE_NO_EXECUTE`. Its seven PE sections have valid recorded raw/RVA bounds, but regular `LoadLibraryExW` still fails Win32=193. Crucially, its `DllCharacteristics=0x4160` includes the `IMAGE_DLLCHARACTERISTICS_GUARD_CF` (0x4000) bit. The image has a `.tls` section; whether TLS/CFG loader metadata is internally valid remains unverified.

The generic `RuntimeLoaderProbe.cpp` now records the actual load-config directory RVA/size, its declared size and `GuardFlags` when safely readable, plus the TLS directory and key VA ranges. It converts RVA to raw-file offsets using section headers and range bounds; it does **not** reinterpret raw file RVAs as pointers. These read-only diagnostics run only on a DLL reporting `ERROR_BAD_EXE_FORMAT` and **do not change linker options, compilation, package dependencies or DLL contents**. A missing/invalid load-config in a CFG-marked PE would justify a precise subsequent GN/LLD investigation; it is not assumed in advance.

**Native output pending:** The exact LoadConfig and TLS values are unknown until the next Release probe run.


## Skia 144 installed-header integration (19 October 2026)

**Decision:** PDFium 2025.11.19 is pinned to precisely the Skia 144 revision recorded in PDFium DEPS (`a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0`). It consumes the independently installed BuildEngine package at `{InstallRoot}/packages/skia/144`. Skia 153 is not substituted, and no Skia implementation is copied or rebuilt under PDFium.

**Cause:** Original PDFium translation units use literal includes like `#include "third_party/skia/include/core/SkSurface.h"`, assuming a Chromium-style Skia source checkout under PDFium. The installed BuildEngine package already provides the same pinned Skia revision, but installs the versioned header tree under `{InstallRoot}/packages/skia/144/include/include/`, alongside `{InstallRoot}/packages/skia/144/include/modules/skcms/`. The GN include root is `{InstallRoot}/packages/skia/144/include`. Copying that directory into `{Workspace}/third_party/skia` is not a valid solution: it reconstructs the wrong nested layout, cannot satisfy the separate `modules/skcms` assertion, duplicates immutable package content, and was observed failing in `stage-installed-skia-headers`.

**Change:** A version-bound source patch is maintained at `admin/patches/pdfium/2025.11.19/skia144-installed-includes.patch`. It converts 53 Skia header includes in six pinned PDFium source files to names relative to the installed package include root (for example `include/core/SkSurface.h`), without altering Skia symbols or API signatures. The affected files are `core/fxge/skia/fx_skia_device.cpp`, `core/fxge/skia/fx_skia_device.h`, `core/fxge/skia/cfx_dibbase_skia.cpp`, `core/fxcodec/png/skia_png_decoder.cpp`, `core/fxge/skia/fx_skia_device_embeddertest.cpp`, and `testing/image_diff/image_diff_png_skia.cpp`. The patch runs once in PDFium Source via the declared XML `git apply --check`, `git apply`, reverse-check sequence, before parallel variant builds. No build-stage source mutation, header staging, additional Skia checkout, or external patch driver is introduced.

The XML `stage-installed-skia-headers` operation and its staged-path assertions were removed. The build now requires the real installed `include/include/core/SkSurface.h` and variant-specific `skia144.lib`; `CB_PDFIUM_SKIA_INCLUDE` points directly to the installed `include` directory and `CB_PDFIUM_SKIA_LIB` to the matching import library. The existing external `//skia` GN adapter remains responsible for propagating include paths, import symbols and compile defines.

**Evidence boundary:** The patch is grounded in the exact pinned upstream PDFium file contents. GitHub commits validate persistence of the patch and XML modifications, not successful BCC64X compilation. A full build must still verify the reachable include closure, including nested Skia-private headers, `skcms` availability, Skia-144-specific definitions and the generated GN graph. If private/package headers are absent, correct the Skia 144 producer's *declared public installation contract* or the consuming API boundary; do not recreate a private Skia tree in PDFium. Debug/Release GN generation, compile, link and smoke remain **[not verified]** at the time of this change.


## Skia 144 DLL link closure after installed-header migration (19 October 2026)

The uploaded native Release `04-execute.log` proves all 603 preliminary steps reached the final `BCC64X DLL pdfium.dll` link. There are two unresolved imports: `SkFontMgr_New_Custom_Empty()` referenced from `CFX_GlyphCache::GetDeviceCache()`, and `Op(SkPath const&, SkPath const&, SkPathOp)` referenced from `CFX_SkiaDeviceDriver::DrawPath()`. These are link-time failures, not include-resolution errors. The Debug FSM summary reports at least the same missing font manager symbol; the full Debug log was not supplied.

Upstream pinned Skia 144 defines both APIs. The owning fix is in the independently installed Skia producer: `admin/patches/skia/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0/bcc64x-pdfium-symbol-closure.patch` adds GN component dependencies for PathOps and the empty font manager. The Skia 144 XML GN args explicitly enable the latter and bind it to installed FreeType 2.14.3. This does not restore copied Skia headers, recompile Skia sources under PDFium, or change the PDFium pin. Native producer export and consumer linker verification remain **[not verified]**.

## PDFium native load failure after successful Skia 144 rebuild (19 October 2026)

New Windows evidence: Skia 144 Debug/Release producer and installs PASS; PDFium Debug/Release builds and installs PASS, but the installed PDFium external smoke fails in both configurations. The Release GN graph generated 494 targets, with BCC64X compilation and BCC64X-driver DLL linking validated by the GN-toolchain check. The actual Release Ninja run is `[1/1] BCC64X DLL ./pdfium.dll`, i.e. an incremental relink rather than an inexplicably short full compile. The linker driver is the configured Embarcadero `bcc64x`; its internal LLVM/LLD implementation must not be confused with invoking `ld.lld` directly.

The Release loader probe individually loaded 13 prerequisite DLLs and still returned `ERROR_BAD_EXE_FORMAT` (Win32 193) for `pdfium.dll`. That image is PE32+ x64 and passed `SEC_IMAGE_NO_EXECUTE` and `DONT_RESOLVE_DLL_REFERENCES` mapping. Its `DllCharacteristics=0x4160` carries CFG, with `LoadConfig.GuardFlags=0x00010500`; its TLS descriptor and ranges were recorded. Those facts alone do not establish a malformed CFG/TLS directory or which loader phase rejects the image. In particular, the prior prerequisite probe list omitted `skia144.dll`, despite the DLL appearing in PDFium's direct import table.

The Release and Debug XML `ADECC_RUNTIME_PROBE_DLLS` lists now explicitly probe `skia144.dll` *before* `pdfium.dll`. This distinguishes a Skia 144 load failure from one specific to PDFium. The new minimal native Skia-144 package consumer checks raster creation, empty FreeType font manager and PathOps union against the installed Debug and Release `skia144.lib` and loads `skia144.dll`. Both tests are XML-owned FSM smoke stages, and are **[not verified]** until run on Windows. If all dependencies pass including Skia and PDFium alone remains Win32 193, investigate the PDFium PE loader metadata and linking contract without reverting to a different compiler or unproven global linker flag changes.

## BCC64X Clang identity correction (19 October 2026)

The PDFium GN profile previously set `is_clang = false` both in the package-generated `args.gn` and in the dedicated `build/toolchain/bcc64x/BUILD.gn`. This misrepresented the Clang-derived Embarcadero BCC64X compiler, potentially selecting incompatible Chromium compiler configuration branches. Both flags are now `true`. The compiler/link driver remains `{Tool:bcc64x}` and archives continue to use the separately provisioned LLVM archiver. No MSVC compiler or direct LLVM linker is substituted.

The PDFium XML library contract timestamp has been advanced to `2026-10-19T01:10:00Z`, invalidating cached PDFium source/build evidence so that a fresh GN generation and compile can exercise the changed Clang configuration instead of directly reusing the earlier successful link. The existing explicitly guarded local Chromium Clang-revision handling remains in place.

**Validation boundary:** The source edits are committed but native GN generation, C++ compilation, linking, and loader smoke are **not yet verified**. Switching Chromium GN's `is_clang` may reveal Chromium-internal Clang assumptions, flags or tool availability requirements. Treat any new diagnostic as a concrete GN/flag incompatibility to resolve in the BCC64X toolchain rather than restoring `is_clang = false`.

## BCC64X Clang identity: Chromium compiler-rt builtins (19 October 2026)

After correctly setting `is_clang = true`, Ninja stopped before compilation because the generated Chromium configuration demanded `third_party/llvm-build/Release+Asserts/lib/clang/22/lib/windows/clang_rt.builtins-x86_64.lib`. That path belongs to Chromium's *private* Clang toolchain, not the installed BCC64X compiler/runtime. The pinned Chromium build configuration injects `//build/config/clang:compiler_builtins` for Clang. It is not a reason to misidentify BCC64X as non-Clang or to stage a fabricated library.

The PDFium source preparation step `adapt-bcc64x-chromium-compiler-builtins` now guards only that compiler-builtin config with `current_toolchain != \"//build/toolchain/bcc64x:bcc64x\"` in `build/config/compiler/BUILD.gn`. Other Clang branches continue to apply, the Embarcadero BCC64X compiler/link driver remains authoritative, and no Chromium clang_rt binary is copied. The XML library timestamp is advanced to `2026-10-19T01:45:00Z` to invalidate the affected FSM evidence. **GN regeneration/native build pending.**

## Chromium Clang/MSVC compile-flag adapter (19 October 2026)

With `is_clang = true` and the Chromium-private compiler-rt builtins dependency excluded, both variants now generate all 604 PDFium build commands, but the **argument adapter**, not the C++ frontend, rejects six Windows compiler switches. The Release log identifies `/Zc:sizedDealloc-`, `/Zc:twoPhase`, `/Brepro`, `/Z7`, `/guard:cf`, and `/W3` for multiple Abseil translation units. Therefore the build summary mentioning `strerror.cc` is a side effect of Ninja parallel execution, not evidence of an Abseil source defect.

The generic BuildEngine `admin/programs/bcc64x/gn_argument_adapter.py` now explicitly handles the observed switches: disable sized deallocation (`-fno-sized-deallocation`), use Clang two-phase name lookup, preserve existing reproducible-build flags instead of forwarding MSVC `/Brepro`, select CodeView (`-gcodeview`) for `/Z7`, request Clang CFG instrumentation (`-mguard=cf`) for `/guard:cf`, and map MSVC warning level 3 to `-Wall`. Unknown MSVC switches **continue to fail**. This is a compile-driver translation, not a conversion to `clang-cl`, MSVC or direct `ld.lld`. The configuration remains subject to native verification: especially `-mguard=cf` on BCC64X and its interaction with the linker guard tables, and whether `-Wall` introduces diagnostics with `-Werror`. Do not silently strip security options or warning diagnostics to pass the build. The PDFium timestamp advances to `2026-10-19T02:00:00Z` for fresh FSM evidence.

## BCC64X LLVM 20 versus Chromium-specific Clang flags (19 October 2026)

The subsequent native Release Ninja log passed argument translation and invoked Embarcadero C++ 7.80, but every visible compilation failed with two diagnostics: `Error E849: unknown warning option '-Wno-uninitialized-const-pointer'` and `Unknown command line argument '-pick-merged-source-locations'` from LLVM option parsing. These are exact-version incompatibilities between Chromium's preferred Clang options and the BCC64X backend; they are not Abseil source errors. The adapter now consumes only the *exact* unsupported warning suppression and the *exact* `-mllvm -pick-merged-source-locations` pair. All other `-mllvm` backend options remain forwarded, and unknown MSVC switches still fail. The suppressed warning option itself has no corresponding warning category in BCC64X; no real source warning is silenced. The backend flag is an optimization/debug-location heuristic, not a correctness or required runtime option. The library contract timestamp was advanced to `2026-10-19T02:15:00Z`. The native build is still **[not verified]** after this commit.

## PDFium GN /W4 warning mapping (19 October 2026)

The latest native Release Ninja log shows Abseil translation units progressing after the precise LLVM option correction, but the PDFium translation unit `core/fdrm/fx_crypt.cpp` stops in the argument adapter at `unsupported MSVC option for BCC64X: /W4`. This is not a source compilation error. The BCC64X adapter now maps GN `/W4` to Clang `-Wall -Wextra`, keeps `/WX` mapped to `-Werror`, and allows explicit multi-flag translations rather than accidentally nesting them as a Python tuple. Unknown MSVC switches remain errors. The PDFium library timestamp was advanced to `2026-10-19T02:30:00Z`. This mapping is not yet verified by a subsequent native run.

## Debug CFG variant and shared Skia include (19 October 2026)

The uploaded Debug Ninja log fails during argument translation on Chromium's `/guard:cf,nochecks` for Abseil compilation. The adapter now explicitly maps that exact option to `-mguard=cf`, deliberately **retaining** guard checks because BCC64X has no separately validated `nochecks` equivalent. The mapping must be validated on the actual compiler and PE loader before claiming equivalent CFG security behavior.

The simultaneously uploaded Release Ninja log successfully compiles numerous Abseil and PDFium translation units (at least through step 95/604) but fails when `core/fxge/dib/cfx_dibbase.h`, included by `cpdf_type3char.cpp`, still references `third_party/skia/include/core/SkRefCnt.h`. The preexisting versioned PDFium Skia include patch only covered six translation units; it overlooked this **shared header**. That exact upstream include is now covered in `skia144-installed-includes.patch` and resolves as `include/core/SkRefCnt.h` under the separately installed Skia 144 include root. No copy or staging is restored. The PDFium timestamp is `2026-10-19T02:45:00Z` for renewed evidence. **Native rebuild pending.** The complete source-wide include inventory remains to be verified rather than assumed from this one failure.
