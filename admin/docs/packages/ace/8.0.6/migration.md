# ACE 8.0.6: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `ace@8.0.6` · **Category:** middleware · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction.

Build ACE natively with MPC/BMake and BCC64X rather than substituting GCC/MSVC. Keep one shared ACE_wrappers producer tree and patch Windows/Clang incompatibilities, Xerces compiler-identity assumptions, managed zlib import names and SSLIOP upstream integration. TAO consumes that same producer tree.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `openssl@3.5.9`
- `xerces-c@3.3.0`
- `zlib@1.3.2`

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

- `https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-8_0_6/ACE%2BTAO-{LibraryVersion}.zip`

**Declared extraction profiles:**

- `zip / ACE_wrappers`

**Package-path convention:** `{InstallRoot}/packages/ace/8.0.6/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/ace-tao/8.0.6/bcc64x-win64-upstream-backports.patch` | Port the necessary upstream Win64 fixes into the pinned ACE source version. |
| `admin/patches/ace-tao/8.0.6/bcc64x-xerces-modern-borland.patch` | Distinguish modern BCC64X Clang from legacy Borland macros used in Xerces integration. |
| `admin/patches/ace-tao/8.0.6/bcc64x-zlib-library-names.patch` | Resolve the managed zlib import-library names without renaming zlib's published output. |
| `admin/patches/ace-tao/8.0.6/upstream-ssliop-params-dup-cpp.patch` | Resolve a duplicated SSLIOP params producer/source definition in the pinned upstream tree. |

### Declarative repairs and preparation actions

- `win64-backports-check`
- `xerces-modern-borland-check`
- `zlib-library-names-check`
- `ssliop-backport-check`
- `copy-build-tree`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Keep the ACE half of the previously proven TAO_ACE.mwc contract. TAO is added later as an extension in this same variant producer tree.
- TAO has not overlaid the payload yet: this is the ACE baseline.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\Release\ACE_wrappers\COPYING -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`
- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\Release\ACE_wrappers\VERSION.txt -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\ACE_VERSION.txt`

**Declared smoke tests:**

- None declared for this category.

**Selected environment inputs:**

- `ACE_ROOT={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers`
- `SSL_ROOT={DependencyRoot:openssl}`
- `SSL_INCDIR={DependencyRoot:openssl}\include`
- `SSL_LIBDIR={DependencyRoot:openssl}\lib\win64\{Configuration}`
- `XERCESC_INCDIR={InstallRoot}\packages\xerces-c\3.3.0\include`
- `XERCESC_LIBDIR={InstallRoot}\packages\xerces-c\3.3.0\lib\win64\{Configuration}`
- `XERCESLIB={XercesLibrary}`
- `ZLIB_ROOT={InstallRoot}\packages\zlib\1.3.2`
- `ZLIB_INCDIR={InstallRoot}\packages\zlib\1.3.2\include`
- `ZLIB_LIBDIR={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}`
- `BCC32C=`
- `BCC64=`
- `BCC64X=1`
- `CODEGUARD=`
- `UNICODE=`
- `DEBUG={DebugFlag}`
- `RELEASE={ReleaseFlag}`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

**ACE/TAO shared producer:** The two logical library records intentionally refer to one ACE_wrappers output tree per variant. ACE_ROOT and TAO_ROOT must not be redirected to independent physical producers.

## Verification and evidence boundaries

No successful upstream-test run can be inferred from the declarative block. An intentionally excluded upstream suite is not a successful test suite.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.
