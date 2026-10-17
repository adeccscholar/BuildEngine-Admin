# curl 8.21.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `curl@8.21.0` · **Category:** network · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

This is a maintained historical coordinate superseded by 8.23.0; do not silently replace it in explicitly pinned dependencies.

## Purpose and integration strategy

**Package purpose:** URL transfer library and command-line client supporting HTTP(S) and numerous network protocols.

Consume BuildEngine OpenSSL for TLS and the managed zlib package through upstream CMake. Keep optional protocol and resolver stacks outside this baseline, and do not silently select Windows Schannel or unrelated host OpenSSL installations. Newer curl has a dedicated BCC64X WinSock compatibility patch.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `openssl@3.5.8`
- `zlib@1.3.2`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `CURL_ENABLE_EXPORT_TARGET` | `ON` |
| `CURL_USE_OPENSSL` | `ON` |
| `CURL_ZLIB` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_STATIC_LIBS` | `OFF` |
| `BUILD_CURL_EXE` | `OFF` |
| `BUILD_EXAMPLES` | `OFF` |
| `BUILD_TESTING` | `OFF` |
| `BUILD_LIBCURL_DOCS` | `OFF` |
| `BUILD_MISC_DOCS` | `OFF` |
| `ENABLE_CURL_MANUAL` | `OFF` |
| `CURL_USE_SCHANNEL` | `OFF` |
| `ENABLE_ARES` | `OFF` |
| `USE_LIBIDN2` | `OFF` |
| `CURL_BROTLI` | `OFF` |
| `CURL_ZSTD` | `OFF` |
| `CURL_USE_PKGCONFIG` | `OFF` |
| `CURL_USE_CMAKECONFIG` | `OFF` |
| `OPENSSL_USE_STATIC_LIBS` | `OFF` |
| `ZLIB_USE_STATIC_LIBS` | `OFF` |
| `CURL_USE_LIBPSL` | `OFF` |
| `CURL_USE_LIBSSH2` | `OFF` |
| `CURL_USE_LIBSSH` | `OFF` |
| `USE_NGHTTP2` | `OFF` |
| `USE_NGTCP2` | `OFF` |
| `USE_QUICHE` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `OPENSSL_INCLUDE_DIR` | `{InstallRoot}\packages\openssl\3.5.8\include` |
| `ZLIB_INCLUDE_DIR` | `{InstallRoot}\packages\zlib\1.3.2\include` |
| `ZLIB_LIBRARY` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\Release\libz.lib` |
| `ZLIB_LIBRARY` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\Debug\libzd.lib` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_FIND_PACKAGE_PREFER_CONFIG:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Versioned source locations:**

- `https://curl.se/download/curl-{LibraryVersion}.tar.xz`

**Declared extraction profiles:**

- `libarchive / curl-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/curl/8.21.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Engineering constraints recorded in the executable contract

- First re-establish the historically verified curl baseline: TLS via OpenSSL plus zlib. Brotli/Zstd remain built and published packages, but are re-enabled for curl only after this baseline is green.
- Match the proven BCC64X baseline: module-based dependency lookup with exact OpenSSL/zlib artifacts supplied below.
- CMake 4.1.1 FindOpenSSL performs its own find_library() lookup on WIN32 and then rewrites OPENSSL_SSL_LIBRARY/OPENSSL_CRYPTO_LIBRARY. Do not set OPENSSL_ROOT_DIR here: that activates NO_DEFAULT_PATH inside FindOpenSSL and hides BuildEngine's configuration-specific lib/win64/Release directory. The generic producer integration supplies that exact directory through CMAKE_LIBRARY_PATH and the versione
- Same FindOpenSSL rule as Release; CMAKE_LIBRARY_PATH is variant-specific.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\COPYING -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\curl-COPYING.txt`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\curl\consumer)`

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
