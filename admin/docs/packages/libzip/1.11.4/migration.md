# libzip 1.11.4: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `libzip@1.11.4` · **Category:** archive · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** C library for reading, creating, and modifying ZIP archives.

Build a shared ZIP library around the managed zlib, XZ and Zstandard dependency closure. Functional archive checks exercise XZ, Zstandard and AES features. Repair upstream tool placement during packaging instead of rewriting its consumer-facing CMake metadata.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `xz@5.8.4`
- `zstd@1.5.7`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `LIBZIP_DO_INSTALL` | `ON` |
| `SHARED_LIB_VERSIONNING` | `ON` |
| `ENABLE_LZMA` | `ON` |
| `ENABLE_ZSTD` | `ON` |
| `ENABLE_WINDOWS_CRYPTO` | `ON` |
| `ENABLE_FDOPEN` | `ON` |
| `BUILD_TOOLS` | `ON` |
| `BUILD_EXAMPLES` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `ENABLE_BZIP2` | `OFF` |
| `ENABLE_COMMONCRYPTO` | `OFF` |
| `ENABLE_OPENSSL` | `OFF` |
| `ENABLE_GNUTLS` | `OFF` |
| `ENABLE_MBEDTLS` | `OFF` |
| `BUILD_REGRESS` | `OFF` |
| `BUILD_OSSFUZZ` | `OFF` |
| `BUILD_DOC` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `ZLIB_INCLUDE_DIR` | `{InstallRoot}\packages\zlib\1.3.2\include` |
| `ZLIB_LIBRARY` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibImportLibrary}` |
| `LIBLZMA_INCLUDE_DIR` | `{DependencyRoot:xz}\include` |
| `LIBLZMA_LIBRARY` | `{DependencyRoot:xz}\lib\win64\{Configuration}\liblzma.lib` |
| `zstd_DIR` | `{InstallRoot}\packages\zstd\1.5.7\lib\win64\{Configuration}\cmake\zstd` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Versioned source locations:**

- `https://github.com/nih-at/libzip/releases/download/v{LibraryVersion}/libzip-{LibraryVersion}.tar.gz`

**Declared extraction profiles:**

- `libarchive / libzip-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/libzip/1.11.4/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Declarative repairs and preparation actions

- `remove-installed-bin`
- `remove-installed-lib`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Official release asset. Use tar.gz for bootstrap so libzip does not depend on the BuildEngine's not-yet-enabled XZ extraction path.
- Required deflate backend plus the two compression backends already produced by this BuildEngine contract.
- Native Windows AES implementation. Do not introduce an OpenSSL dependency when BCrypt is the upstream-preferred Windows backend.
- Build the upstream command-line tools. Documentation is disabled because libzip's documentation target copies generated HTML back into the shared source tree; parallel Release/Debug builds race on those files. Functional library gates remain enabled below.
- Release and Debug use separate directories. BCC64X/CMake emits libzip.dll and libzip.lib for the upstream target named zip.
- These tests run from the build tree before installation. The working directory contains zip.dll; dependency runtime directories are supplied generically by the BuildEngine.
- Upstream installs the three tools to literal bin rather than CMAKE_INSTALL_BINDIR. Keep that upstream location intact for libzip-bin-targets and additionally provide the configuration- specific package layout used by the BuildEngine.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src\zipcmp.exe -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\zipcmp.exe`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src\zipmerge.exe -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\zipmerge.exe`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src\ziptool.exe -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\ziptool.exe`
- `{Workspace}\LICENSE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\libzip\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`

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
