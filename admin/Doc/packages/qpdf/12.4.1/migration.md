# qpdf 12.4.1: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `qpdf@12.4.1` · **Category:** documentation · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Build QPDF shared against managed zlib/JPEG while preferring QPDF's native crypto path to unnecessary OpenSSL/Brotli/Zstd runtime closure. Keep native Windows qtest execution and explicit BuildEngine dependency resolution by version-bound patches.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `libjpeg-turbo@3.2.0`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `INSTALL_PKGCONFIG` | `ON` |
| `INSTALL_CMAKE_PACKAGE` | `ON` |
| `REQUIRE_CRYPTO_NATIVE` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_STATIC_LIBS` | `OFF` |
| `BUILD_DOC` | `OFF` |
| `INSTALL_MANUAL` | `OFF` |
| `INSTALL_EXAMPLES` | `OFF` |
| `MAINTAINER_MODE` | `OFF` |
| `CI_MODE` | `OFF` |
| `WERROR` | `OFF` |
| `GENERATE_AUTO_JOB` | `OFF` |
| `ENABLE_QTC` | `OFF` |
| `REQUIRE_SHELLS` | `OFF` |
| `USE_IMPLICIT_CRYPTO` | `OFF` |
| `REQUIRE_CRYPTO_OPENSSL` | `OFF` |
| `REQUIRE_CRYPTO_GNUTLS` | `OFF` |
| `USE_INSECURE_RANDOM` | `OFF` |
| `SKIP_OS_SECURE_RANDOM` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `ZLIB_H_PATH` | `{InstallRoot}\packages\zlib\1.3.2\include` |
| `ZLIB_LIB_PATH` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\Release\libz.lib` |
| `LIBJPEG_H_PATH` | `{InstallRoot}\packages\libjpeg-turbo\3.2.0\include` |
| `LIBJPEG_LIB_PATH` | `{InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\Release\libjpeg.lib` |
| `ZLIB_LIB_PATH` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\Debug\libzd.lib` |
| `LIBJPEG_LIB_PATH` | `{InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\Debug\libjpegd.lib` |
| `qpdf_DIR` | `{ConsumerRoot}\lib\win64\{Configuration}\cmake\qpdf` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{InstallRoot}\packages\libjpeg-turbo\3.2.0`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Versioned source locations:**

- `https://github.com/qpdf/qpdf/archive/refs/tags/v{LibraryVersion}.tar.gz`

**Declared extraction profiles:**

- `libarchive / qpdf-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/qpdf/12.4.1/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/qpdf/12.4.1/native-windows-qtest.patch` | Adapt QPDF's test launcher and shell assumptions to native Windows execution. |
| `admin/patches/qpdf/12.4.1/prefer-explicit-managed-dependencies.patch` | Resolve exact managed dependencies in QPDF's build rather than machine-local alternatives. |

### Declarative repairs and preparation actions

- `qtest-patch-check`
- `qtest-patch-apply`
- `qtest-patch-verify`
- `managed-deps-patch-check`
- `managed-deps-patch-apply`
- `managed-deps-patch-verify`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\LICENSE.txt -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.txt`
- `{Workspace}\NOTICE.md -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\NOTICE.md`

**Declared smoke tests:**

- `consumer (Release, scope package; source smokes\qpdf\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `QPDF_BCC64X=1`
- `QPDF_PERL={Tool:perl}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `QPDF_NATIVE_WINDOWS_PERL=1`

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
