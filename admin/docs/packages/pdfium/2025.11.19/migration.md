# PDFium 2025.11.19: BCC64X migration and integration record

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
