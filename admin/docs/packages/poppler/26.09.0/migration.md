# Poppler 26.09.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `poppler@26.09.0` · **Category:** documentation · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** PDF rendering and parsing library derived from Xpdf and used by document-processing toolchains.

Use upstream CMake for Poppler with managed compression/image/font packages and the C++ API enabled. Add _AMD64_ and NOMINMAX only for this Windows SDK consumer, not globally. Optional frontends and the separately distributed upstream test-data repository are outside the present evidence boundary; GPL implications remain a product decision.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `freetype@2.14.3`
- `libjpeg-turbo@3.2.0`
- `libpng@1.6.58`
- `libtiff@4.7.2`
- `win-iconv@0.0.8`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `ENABLE_CPP` | `ON` |
| `ENABLE_UNSTABLE_API_ABI_HEADERS` | `ON` |
| `ENABLE_LIBJPEG` | `ON` |
| `ENABLE_LIBTIFF` | `ON` |
| `ENABLE_RELOCATABLE` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `ENABLE_UTILS` | `OFF` |
| `ENABLE_GLIB` | `OFF` |
| `ENABLE_QT5` | `OFF` |
| `ENABLE_QT6` | `OFF` |
| `ENABLE_BOOST` | `OFF` |
| `ENABLE_BROTLI` | `OFF` |
| `ENABLE_LIBOPENJPEG` | `OFF` |
| `ENABLE_LCMS` | `OFF` |
| `ENABLE_LIBCURL` | `OFF` |
| `ENABLE_NSS3` | `OFF` |
| `ENABLE_GPGME` | `OFF` |
| `BUILD_GTK_TESTS` | `OFF` |
| `BUILD_QT5_TESTS` | `OFF` |
| `BUILD_QT6_TESTS` | `OFF` |
| `BUILD_CPP_TESTS` | `OFF` |
| `BUILD_MANUAL_TESTS` | `OFF` |
| `RUN_GPERF_IF_PRESENT` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `FONT_CONFIGURATION` | `win32` |
| `Iconv_INCLUDE_DIR` | `{InstallRoot}\packages\win-iconv\0.0.8\include` |
| `Iconv_LIBRARY` | `{InstallRoot}\packages\win-iconv\0.0.8\lib\win64\Release\libiconv.lib` |
| `Iconv_LIBRARY` | `{InstallRoot}\packages\win-iconv\0.0.8\lib\win64\Debug\libiconv.lib` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin`
- `-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`

**Versioned source locations:**

- `https://poppler.freedesktop.org/poppler-{LibraryVersion}.tar.xz`

**Declared extraction profiles:**

- `libarchive / poppler-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/poppler/26.09.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Engineering constraints recorded in the executable contract

- Poppler's Win32 backend includes the Microsoft-style Windows SDK headers. BCC64X identifies the target with _WIN64/__MINGW64__, while winnt.h requires the SDK architecture selector _AMD64_ on x64. Keep this bridge local to Poppler; the proven generic BCC64X toolchain remains unchanged.
- Upstream release tarballs do not contain poppler/test-data. Test integration stays explicit and will be added when that separate upstream data repository becomes a pinned BuildEngine participant.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\include -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{Workspace}\COPYING -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`

**Declared smoke tests:**

- None declared for this category.

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CXXFLAGS=-D_AMD64_ -DNOMINMAX`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

**Legal constraint:** Poppler GPL licensing requires independent product-level review before proprietary deployment. Successful compilation does not grant a license exception.

## Verification and evidence boundaries

No successful upstream-test run can be inferred from the declarative block. An intentionally excluded upstream suite is not a successful test suite.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.
