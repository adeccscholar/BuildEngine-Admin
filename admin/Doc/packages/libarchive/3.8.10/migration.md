# libarchive 3.8.10: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `libarchive@3.8.10` · **Category:** archive · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats.

Preserve libarchive's upstream CMake project and native tar/cpio/cat/unzip utilities. Bind every enabled compression/crypto backend to managed package roots, notably bzip2. Correct old Borland compiler assumptions in specific version-bound patches without weakening Debug warnings.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `xz@5.8.4`
- `zstd@1.5.7`
- `bzip2@1.0.8`
- `openssl@3.5.9`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `ENABLE_INSTALL` | `ON` |
| `ENABLE_TAR` | `ON` |
| `ENABLE_TAR_SHARED` | `ON` |
| `ENABLE_CPIO` | `ON` |
| `ENABLE_CPIO_SHARED` | `ON` |
| `ENABLE_CAT` | `ON` |
| `ENABLE_CAT_SHARED` | `ON` |
| `ENABLE_UNZIP` | `ON` |
| `ENABLE_UNZIP_SHARED` | `ON` |
| `ENABLE_ZLIB` | `ON` |
| `ENABLE_CNG` | `ON` |
| `ENABLE_WIN32_XMLLITE` | `ON` |
| `ENABLE_OPENSSL` | `ON` |
| `ENABLE_LZMA` | `ON` |
| `ENABLE_ZSTD` | `ON` |
| `ENABLE_BZip2` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `OPENSSL_USE_STATIC_LIBS` | `FALSE` |
| `ENABLE_MBEDTLS` | `OFF` |
| `ENABLE_NETTLE` | `OFF` |
| `ENABLE_LIBB2` | `OFF` |
| `ENABLE_LZ4` | `OFF` |
| `ENABLE_LZO` | `OFF` |
| `ENABLE_LIBXML2` | `OFF` |
| `ENABLE_EXPAT` | `OFF` |
| `ENABLE_PCREPOSIX` | `OFF` |
| `ENABLE_PCRE2POSIX` | `OFF` |
| `ENABLE_LIBGCC` | `OFF` |
| `ENABLE_ICONV` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `ENABLE_TEST` | `{TestsEnabled}` |
| `ZLIB_INCLUDE_DIR` | `{InstallRoot}\packages\zlib\1.3.2\include` |
| `ZLIB_LIBRARY` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibLibraryName}.lib` |
| `OPENSSL_ROOT_DIR` | `{DependencyRoot:openssl}` |
| `OPENSSL_INCLUDE_DIR` | `{DependencyRoot:openssl}\include` |
| `OPENSSL_SSL_LIBRARY` | `{DependencyRoot:openssl}\lib\win64\{Configuration}\libssl.lib` |
| `OPENSSL_CRYPTO_LIBRARY` | `{DependencyRoot:openssl}\lib\win64\{Configuration}\libcrypto.lib` |
| `LIBLZMA_INCLUDE_DIR` | `{DependencyRoot:xz}\include` |
| `LIBLZMA_LIBRARY` | `{DependencyRoot:xz}\lib\win64\{Configuration}\liblzma.lib` |
| `ZSTD_INCLUDE_DIR` | `{InstallRoot}\packages\zstd\1.5.7\include` |
| `ZSTD_LIBRARY` | `{InstallRoot}\packages\zstd\1.5.7\lib\win64\{Configuration}\libzstd.lib` |
| `BZIP2_INCLUDE_DIR` | `{InstallRoot}\packages\bzip2\1.0.8\include` |
| `BZIP2_LIBRARIES` | `{InstallRoot}\packages\bzip2\1.0.8\lib\win64\{Configuration}\{Bzip2LibraryName}.lib` |
| `POSIX_REGEX_LIB` | `NONE` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`

**Versioned source locations:**

- `https://github.com/libarchive/libarchive/releases/download/v{LibraryVersion}/libarchive-{LibraryVersion}.zip`

**Declared extraction profiles:**

- `zip / libarchive-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/libarchive/3.8.10/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/libarchive/3.8.10/bcc64x-modern-borland-compat.patch` | Replace legacy Borland-only assumptions where modern Clang-based BCC64X must take the native Windows/compiler branch (including test cases). |
| `admin/patches/libarchive/3.8.10/managed-bzip2-contract.patch` | Stop upstream discovery from silently substituting a host bzip2 and use the exact managed library names/paths. |

### Declarative repairs and preparation actions

- `patch-check`
- `patch-apply`
- `patch-verify`
- `bzip2-patch-check`
- `bzip2-patch-apply`
- `bzip2-patch-verify`
- `remove-installed-bin`
- `remove-installed-lib`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- BCC64X compatibility backpatch for legacy __BORLANDC__ compiler branches. BCC64X also defines __clang__ and must therefore use the modern Clang-compatible paths for inline handling, integer types and literals, Windows lseek/mbstate_t handling, open mode arguments, and the corresponding tests. The patch also includes the upstream Windows/Clang fix from commit f75ed3a20526ef4ec2f46ee94216b6efd01eab0
- CMake resolves the resource compiler through its BCC64X platform integration.
- Complete native command line toolset. The product package contains the shared runtime and import library; libarchive's internal static target remains only because upstream tests link against it.
- Declared compression/Windows backends; no accidental host-library pickup.
- BCC64X on Windows provides no libc POSIX regex implementation. With AUTO, libarchive otherwise enters its PCRE/libgcc fallback even though both optional regex backends are disabled below.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\lib\{ArchiveName}.lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\{ArchiveName}.lib`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\lib\pkgconfig -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\pkgconfig`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\include -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\share -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\share`
- `{Workspace}\COPYING -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\libarchive\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CTEST_OUTPUT_ON_FAILURE=1`
- `CTEST_PARALLEL_LEVEL={TestJobs}`

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
