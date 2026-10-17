# Graphite2 1.3.15: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `graphite2@1.3.15` · **Category:** text · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** Smart-font rendering engine for Graphite fonts and complex text shaping.

Retain Graphite2 upstream CMake and its real upstream test suite rather than disabling tests when BCC64X reveals discrepancies. Versioned repairs cover native test fixture encoding, CRLF expectations, reference output, managed FontTools/Python discovery and the Padauk case. The integration report records 91 of 91 configured tests passing for both variants.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

No direct BuildEngine library dependency is declared for this coordinate. This does **not** mean the generated binary uses no Windows system libraries or that its upstream bundled components are absent.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `ON` |

### Deliberately disabled switches

No switches of this category are explicitly selected in the parsed build arguments.

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `LIB_SUFFIX` | `/win64/{Configuration}` |
| `BUILD_TESTING` | `{TestsEnabled}` |
| `Python3_EXECUTABLE` | `{Tool:python}` |
| `GRAPHITE2_VM_TYPE` | `auto` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`

**Versioned source locations:**

- `https://github.com/silnrsi/graphite/releases/download/{LibraryVersion}/graphite2-{LibraryVersion}.tgz`

**Declared extraction profiles:**

- `libarchive / graphite2-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/graphite2/1.3.15/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/graphite2/1.3.15/add-managed-fonttools-path.patch` | Use the managed FontTools tooling rather than an arbitrary PATH discovery. |
| `admin/patches/graphite2/1.3.15/bcc64x-python-dll-search.patch` | Keep Python/test support DLL discovery scoped to the managed Windows runtime. |
| `admin/patches/graphite2/1.3.15/diagnose-fnttxtrender-mismatch.patch` | Add diagnostic evidence around the test-render mismatch to distinguish behavior from fixture mismatch. |
| `admin/patches/graphite2/1.3.15/fix-bcc64x-padauk3-reference.patch` | Repair the Padauk3 reference expectation for the BCC64X test case. |
| `admin/patches/graphite2/1.3.15/fix-fnttxtrender-advance.patch` | Correct the font-test advance/reference comparison without disabling the test. |
| `admin/patches/graphite2/1.3.15/fix-gr2fonttest-log-line-endings.patch` | Normalize platform line-ending handling for native Windows test logs. |
| `admin/patches/graphite2/1.3.15/fix-jsontest-log-line-endings.patch` | Normalize Windows CRLF differences in JSON test evidence. |
| `admin/patches/graphite2/1.3.15/fix-nametabletest-langtags.patch` | Update the upstream font language-tag test reference expectations for the pinned Windows test environment. |
| `admin/patches/graphite2/1.3.15/fix-nametabletest-utf8-literals.patch` | Ensure the font-name test compares intended UTF-8 literal values under BCC64X. |

### Declarative repairs and preparation actions

- `nametabletest-patch-check`
- `nametabletest-patch-apply`
- `nametabletest-patch-verify`
- `fnttxtrender-patch-check`
- `fnttxtrender-patch-apply`
- `fnttxtrender-patch-verify`
- `fnttxtrender-diagnostic-patch-check`
- `fnttxtrender-diagnostic-patch-apply`
- `fnttxtrender-diagnostic-patch-verify`
- `gr2fonttest-log-patch-check`
- `gr2fonttest-log-patch-apply`
- `gr2fonttest-log-patch-verify`
- `nametabletest-utf8-patch-check`
- `nametabletest-utf8-patch-apply`
- `nametabletest-utf8-patch-verify`
- `fonttools-path-patch-check`
- `fonttools-path-patch-apply`
- `fonttools-path-patch-verify`
- `jsontest-log-patch-check`
- `jsontest-log-patch-apply`
- `jsontest-log-patch-verify`
- `padauk3-reference-patch-check`
- `padauk3-reference-patch-apply`
- `padauk3-reference-patch-verify`
- `runtime-fix-check`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\lib\win64\{Configuration} -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\include -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{Workspace}\LICENSE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Declared smoke tests:**

- None declared for this category.

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `GRAPHITE2_FONTTOOLS_PATH={ToolDir:fonttools}`
- `GRAPHITE2_DLL_SEARCH_PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src;{ToolDir:bcc64x}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

## Verification and evidence boundaries

The contract includes one or more upstream tests, validation actions or consumer smokes. Their mere declaration does not establish a successful execution; use the FSM state evidence and process logs for the specific version and configuration.

- The maintained BCC64X integration findings record 91/91 configured tests passing in Release and Debug. These figures refer to that recorded run, not a new execution performed while writing this guide.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.
