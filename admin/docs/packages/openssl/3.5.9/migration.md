# openssl 3.5.9: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `openssl@3.5.9` · **Category:** security · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

Earlier OpenSSL 3.5.8 engineering notes describe the same porting family; this file uses the current 3.5.9 contract for values.

## Purpose and integration strategy

**Package purpose:** Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives.

Use native OpenSSL Configure with an explicit BC-64X platform, BCC64X linker, LLVM archiver and managed compression dependencies. The package deliberately uses a controlled no-assembly shared baseline. Linker response files, QUIC WinSock declarations and PDB handling required source/build-system corrections, not a switch to MSVC.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`
- `brotli@1.2.0`
- `zstd@1.5.7`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

No switches of this category are explicitly selected in the parsed build arguments.

### Deliberately disabled switches

No switches of this category are explicitly selected in the parsed build arguments.

### Other selected build features and external settings

No switches of this category are explicitly selected in the parsed build arguments.


## Build architecture, variants and paths

**Selected build flow:** GN/Ninja and explicit native toolchain targets. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- None declared for this category.

**Versioned source locations:**

- `https://github.com/openssl/openssl/releases/download/openssl-{LibraryVersion}/openssl-{LibraryVersion}.tar.gz`

**Declared extraction profiles:**

- `libarchive / openssl-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/openssl/3.5.9/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/openssl/3.5.9/bcc64x-linker-external-libs.patch` | Preserve externally resolved compression and system libraries when writing OpenSSL linker response files. |
| `admin/patches/openssl/3.5.9/bcc64x-linker-response-paths.patch` | Normalize path quoting/response-file input for the BCC64X and LLD linker contract. |
| `admin/patches/openssl/3.5.9/bcc64x-memory-barrier.patch` | Provide the correct memory-barrier semantics/headers for the modern BCC64X Windows compiler rather than a legacy Borland branch. |
| `admin/patches/openssl/3.5.9/bcc64x-pdb-platform.patch` | Use the native PDB generation model, not the older C++Builder TDS debug-file convention. |
| `admin/patches/openssl/3.5.9/bcc64x-quic-winsock-inline.patch` | Handle Windows SDK inline declarations and compiler identity in OpenSSL's QUIC/WinSock path. |

### Declarative repairs and preparation actions

- `patch-check`
- `patch-apply`
- `response-patch-check`
- `response-patch-apply`
- `external-libs-response-patch-check`
- `external-libs-response-patch-apply`
- `quic-inline-patch-check`
- `quic-inline-patch-apply`
- `pdb-platform-patch-check`
- `pdb-platform-patch-apply`
- `verify-bcc64x-platform`
- `brotli-common-import-alias`
- `brotli-dec-import-alias`
- `brotli-enc-import-alias`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- BCC64X defines both __BORLANDC__ and __clang__. In the current Embarcadero Win64/COFF headers the WinSDK MemoryBarrier macro is not exposed. OpenSSL 3.5.8 uses it in both Windows thread implementations. The patch supplies the missing full fence locally for Clang-based Embarcadero compilers through __atomic_thread_fence with sequentially consistent ordering; upstream call sites remain unchanged.
- BCC64X uses GNU/Clang response-file tokenization. A backslash in OpenSSL's linker inline response file escapes the following character, so crypto\aes\foo.obj arrives as cryptoaesfoo.obj. The patch adds an opt-in target property that converts only paths written to linker response files to forward slashes. Make paths, prerequisites and compiler command lines remain unchanged.
- OpenSSL places Configure-supplied external compression libraries in LIB_EX_LIBS / DSO_EX_LIBS / BIN_EX_LIBS. These values are written into the same BCC64X/LLD response files, but they bypass the path lists normalized by bcc64x-linker-response-paths.patch. Keep the proven response-path patch unchanged and normalize only these response-file copies of external library arguments.
- The Microsoft SDK mstcpip.h and mswsockdef.h headers use GNU-style extern inline declarations for non-MSVC compilers. BCC64X uses Clang C inline semantics, which causes unused header helpers to become emitted definitions with unresolved SDK dependencies. OpenSSL's quic_reactor.c only needs SIO_UDP_CONNRESET and SIO_UDP_NETRESET from mstcpip.h/mswsock.h. The patch keeps winsock2.h and supplies thos
- OpenSSL's Windows::cppbuilder platform still models the legacy C++Builder debug sidecar as .tds. BCC64X is the modern COFF/LLD toolchain and uses PDB debug information instead. The patch adds a dedicated Windows::bcc64x platform that inherits the existing C++Builder behavior and overrides only the debug-file extension.
- Native BCC64X OpenSSL build. All OpenSSL target knowledge remains in this XML contract. The perl target materializes the OpenSSL Configure target file in the variant build tree.
- OpenSSL 3.5.8 expects unprefixed Brotli import-library filenames on Windows (brotlicommon.lib, brotlidec.lib, brotlienc.lib). The upstream Brotli/BCC64X producer correctly installs libbrotli*.lib. Keep the producer artifacts untouched and create private consumer-side aliases in the OpenSSL build tree. The COFF import descriptors still reference the original libbrotli*.dll names.
- First native proof keeps test/install in the existing perVariant sequence. Scheduler-level Release/Debug test parallelism remains a separate generic change.
- Generate BCC64X COFF import libraries directly from the produced native DLLs.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\lib -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\include -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{Workspace}\LICENSE.txt -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\OpenSSL-LICENSE.txt`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\openssl\consumer)`

**Selected environment inputs:**

- `HARNESS_JOBS={TestJobs}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

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
