# podofo 1.1.1: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `podofo@1.1.1` · **Category:** documentation · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Build the PoDoFo PDF object/document APIs as a shared BCC64X Windows package using exact managed zlib, OpenSSL, FreeType, libxml2, JPEG, PNG and TIFF dependencies. Investigations proved the danger of allocating/deleting C++ objects across a DLL boundary with separate static runtimes; enforce -tR globally rather than suppressing crashing tests. Versioned source changes address genuine API/ABI/test constraints.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `openssl@3.5.9`
- `freetype@2.14.3`
- `libxml2@2.15.3`
- `libjpeg-turbo@3.2.0`
- `libpng@1.6.58`
- `libtiff@4.7.2`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `PODOFO_WITH_FONTMANAGER` | `ON` |
| `PODOFO_WITH_WIN32GDI_FONT_SEARCH` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `PODOFO_BUILD_STATIC` | `OFF` |
| `PODOFO_BUILD_EXAMPLES` | `OFF` |
| `PODOFO_BUILD_UNSUPPORTED_TOOLS` | `OFF` |
| `PODOFO_WITH_AFDKO` | `OFF` |
| `PODOFO_WITH_LCMS2` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `PODOFO_BUILD_TEST` | `{TestsEnabled}` |
| `PDF_TEST_RESOURCE_PATH` | `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\resources` |
| `PDF_TEST_OUTPUT_PATH` | `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\out` |
| `LIBXML2_LIBRARY` | `{InstallRoot}\packages\libxml2\2.15.3\lib\win64\Release\libxml2.lib` |
| `LIBXML2_LIBRARY` | `{InstallRoot}\packages\libxml2\2.15.3\lib\win64\Debug\libxml2d.lib` |
| `PoDoFo_DIR` | `{ConsumerRoot}\lib\win64\{Configuration}\cmake\podofo` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_CXX_FLAGS:STRING=-fborland-exec-code-page=65001`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{DependencyRoot:openssl};{InstallRoot}\packages\freetype\2.14.3;{InstallRoot}\packages\libxml2\2.15.3;{InstallRoot}\packages\libjpeg-turbo\3.2.0;{InstallRoot}\packages\libpng\1.6.58;{InstallRoot}\packages\libtiff\4.7.2`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Versioned source locations:**

- `https://github.com/podofo/podofo/archive/refs/tags/{LibraryVersion}.tar.gz`

**Declared extraction profiles:**

- `libarchive / podofo-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/podofo/1.1.1/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

Do not hide XMP lifetime/allocator faults by omitting upstream tests. Dynamic `-tR` across EXE/DLL/MODULE boundaries is a generic compiler runtime requirement, independently of the PoDoFo DLL patch series.

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/podofo/1.1.1/bcc64x-catch-isnan-route.patch` | Avoid a legacy Catch2/CRT isnan path incompatible with this toolchain. |
| `admin/patches/podofo/1.1.1/bcc64x-charconv-route.patch` | Select a compatible BCC64X character-conversion implementation in PoDoFo. |
| `admin/patches/podofo/1.1.1/bcc64x-cid-order.patch` | Make CID mapping/ordering deterministic for the tested compiler and fixtures. |
| `admin/patches/podofo/1.1.1/bcc64x-dll-export.patch` | Select DLL import/export declarations that are valid for native Windows BCC64X rather than GNU visibility attributes. |
| `admin/patches/podofo/1.1.1/bcc64x-test-output-isolation.patch` | Keep test output files isolated by configuration so parallel Debug/Release runs cannot interfere. |
| `admin/patches/podofo/1.1.1/bcc64x-test-resource-isolation.patch` | Isolate mutable test resources and cleanups across concurrently executed suites. |
| `admin/patches/podofo/1.1.1/bcc64x-xmp-dll-boundary.patch` | Correct exported XMP class/interface ownership across the PoDoFo DLL boundary. |
| `admin/patches/podofo/1.1.1/bcc64x-xmp-dll-link-fix.patch` | Provide the necessary exported XMP symbols for installed DLL consumers. |
| `admin/patches/podofo/1.1.1/bcc64x-xmp-schema-lifetime.patch` | Address XMP schema/object lifetime ownership and destruction order. |

### Declarative repairs and preparation actions

- `charconv-patch-check`
- `charconv-patch-apply`
- `charconv-patch-verify`
- `catch-isnan-patch-check`
- `catch-isnan-patch-apply`
- `catch-isnan-patch-verify`
- `cid-order-patch-check`
- `cid-order-patch-apply`
- `test-output-patch-check`
- `test-output-patch-apply`
- `xmp-dll-patch-check`
- `xmp-dll-patch-apply`
- `xmp-dll-patch-verify`
- `xmp-dll-link-patch-check`
- `xmp-dll-link-patch-apply`
- `xmp-dll-link-patch-verify`
- `dll-export-patch-check`
- `dll-export-patch-apply`
- `test-resource-patch-check`
- `test-resource-patch-apply`
- `test-resource-patch-verify`
- `xmp-lifetime-patch-check`
- `xmp-lifetime-patch-apply`
- `xmp-lifetime-patch-verify`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- In shared builds, XMP tests use the DLL without re-linking internal archives. The existing upstream tests retain their original private-library access.
- Repair the DLL-only test target without adding private static archives.
- PoDoFo 1.1.1 gitlink extern/resources -> podofo-resources@1e0740f2183e01d6f8516399031f6dbccbe8cd0f. Keep Git metadata outside the materialized PoDoFo source tree.
- Keep RELAX-NG schemas scoped to each validation, not DLL teardown.
- PoDoFo sources and upstream tests contain UTF-8 characters in ordinary narrow string literals. BCC64X 20.1.7 requires the Embarcadero execution-code-page switch for UTF-8 literal bytes. This was machine-verified previously for Catch2; generic Clang -finput-charset/-fexec-charset did not change BCC64X behaviour.
- Keep the tested runtime closure deterministic. In particular, Release libz.dll must come from our zlib 1.3.2 package.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\NOTICE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\NOTICE`

**Declared smoke tests:**

- `consumer (Release, scope package; source smokes\podofo\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL=1`
- `PODOFO_TEST_OUTPUT_PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\out`

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
