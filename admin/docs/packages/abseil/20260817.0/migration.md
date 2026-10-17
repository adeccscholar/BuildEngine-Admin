# abseil 20260817.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `abseil@20260817.0` · **Category:** core · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Use the upstream Abseil LTS CMake source with a controlled monolithic shared-library profile. Explicit Windows/Clang inline and import/export corrections are required; a duplicate kTenToNth symbol in the installed consumer was treated as a genuine ODR/export defect, not as an allowable duplicate.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

No direct BuildEngine library dependency is declared for this coordinate. This does **not** mean the generated binary uses no Windows system libraries or that its upstream bundled components are absent.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |
| `ABSL_ENABLE_INSTALL` | `ON` |
| `ABSL_PROPAGATE_CXX_STD` | `ON` |
| `ABSL_BUILD_MONOLITHIC_SHARED_LIBS` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_TESTING` | `OFF` |
| `ABSL_BUILD_TESTING` | `OFF` |
| `ABSL_BUILD_TEST_HELPERS` | `OFF` |
| `ABSL_USE_GOOGLETEST_HEAD` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `absl_DIR` | `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\Release\cmake\absl` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_CXX_STANDARD:STRING=20`
- `-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Versioned source locations:**

- `https://github.com/abseil/abseil-cpp/archive/refs/tags/20260817.0.tar.gz`

**Declared extraction profiles:**

- `libarchive / abseil-cpp-20260817.0`

**Package-path convention:** `{InstallRoot}/packages/abseil/20260817.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/abseil/20260817.0/bcc64x-charconv-digits10.patch` | A targeted version-specific compatibility change. Its exact motivation is [not verified] from the currently available engineering notes; inspect the unified diff before treating it as an established root cause. |
| `admin/patches/abseil/20260817.0/bcc64x-dll-export-all.patch` | A targeted version-specific compatibility change. Its exact motivation is [not verified] from the currently available engineering notes; inspect the unified diff before treating it as an established root cause. |
| `admin/patches/abseil/20260817.0/bcc64x-dll-import-export.patch` | A targeted version-specific compatibility change. Its exact motivation is [not verified] from the currently available engineering notes; inspect the unified diff before treating it as an established root cause. |
| `admin/patches/abseil/20260817.0/bcc64x-win32-cctz.patch` | A targeted version-specific compatibility change. Its exact motivation is [not verified] from the currently available engineering notes; inspect the unified diff before treating it as an established root cause. |

### Declarative repairs and preparation actions

- `patch-check`
- `patch-apply`
- `digits10-check`
- `dll-import-export-check`
- `dll-import-export-verify`
- `dll-export-all-check`
- `dll-export-all-verify`
- `digits10-verify`
- `patch-verify`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\LICENSE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses\abseil\LICENSE`

**Declared smoke tests:**

- `consumer (Release, scope package; source smokes\abseil\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`

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
