# brotli 1.2.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `brotli@1.2.0` · **Category:** compression · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

**Supersession:** This historical coordinate is superseded by `brotli@1.2.1`. It remains available for explicitly pinned consumers; no dependency is automatically migrated. The replacement is a security release addressing CVE-2026-111497.

## Purpose and integration strategy

**Package purpose:** General-purpose lossless compression library providing Brotli encoder and decoder support.

Produce managed shared Brotli compression and decompression libraries and command-line tools. Explicitly control bundled/package mode and test execution, while preserving the exact downstream dependency identity for OpenSSL and Skia.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

No direct BuildEngine library dependency is declared for this coordinate. This does **not** mean the generated binary uses no Windows system libraries or that its upstream bundled components are absent.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `BROTLI_BUILD_TOOLS` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BROTLI_BUILD_FOR_PACKAGE` | `OFF` |
| `BROTLI_BUNDLED_MODE` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `BROTLI_DISABLE_TESTS` | `{TestsDisabled}` |


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

- `https://github.com/google/brotli/archive/refs/tags/v{LibraryVersion}.tar.gz`

**Declared extraction profiles:**

- `libarchive / brotli-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/brotli/1.2.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Declarative repairs and preparation actions

- `remove-installed-bin`
- `remove-installed-lib`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Stock Brotli CMake build: no BCC64X source patch or replacement build file.
- Shared-only product; upstream tests and command line tool remain enabled.
- Release and Debug are separated by directory. Keep the artifact names emitted by the stock Brotli CMake project with the active BCC64X/CMake toolchain; do not normalize them for a downstream consumer.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\LICENSE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\brotli\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
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
