# libpq 18.6: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `libpq@18.6` · **Category:** database · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Build PostgreSQL's client library with Meson/Ninja/BCC64X and managed OpenSSL/zlib, not the full PostgreSQL server. Correct Windows SDK/NT security header collisions and WinSock linkage. The present contract explicitly disables certain optional integrations including GSSAPI; it must not be described as a Kerberos-enabled product.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `openssl@3.5.9`
- `zlib@1.3.2`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `zlib` | `enabled` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `readline` | `disabled` |
| `icu` | `disabled` |
| `ldap` | `disabled` |
| `gssapi` | `disabled` |
| `libcurl` | `disabled` |
| `nls` | `disabled` |
| `docs` | `disabled` |
| `docs_pdf` | `disabled` |
| `tap_tests` | `disabled` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `ssl` | `openssl` |
| `PERL` | `{Tool:perl}` |
| `extra_include_dirs` | `{DependencyRoot:openssl}\include,{InstallRoot}\packages\zlib\1.3.2\include` |
| `extra_lib_dirs` | `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\compat-lib,{DependencyRoot:openssl}\lib\win64\{Configuration},{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}` |
| `PostgreSQL_INCLUDE_DIR` | `{ConsumerRoot}\include` |
| `PostgreSQL_LIBRARY` | `{ConsumerRoot}\lib\libpq.lib` |


## Build architecture, variants and paths

**Selected build flow:** GN/Ninja and explicit native toolchain targets. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- None declared for this category.

**Versioned source locations:**

- `https://ftp.postgresql.org/pub/source/v{LibraryVersion}/postgresql-{LibraryVersion}.tar.bz2`

**Declared extraction profiles:**

- `libarchive / postgresql-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/libpq/18.6/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/libpq/18.6/bcc64x-ntsecapi-conflict.patch` | Resolve the Windows NT security header collision exposed by native BCC64X includes. |
| `admin/patches/libpq/18.6/bcc64x-winsock-link.patch` | Link PostgreSQL's libpq Windows socket dependencies under the actual BCC64X output model. |

### Declarative repairs and preparation actions

- `ntsecapi-patch-check`
- `ntsecapi-patch-apply`
- `winsock-patch-check`
- `winsock-patch-apply`
- `create-zlib-compat-dir`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Reuse the proven generic Meson bootstrap already used by the native BCC64X Mesa build.
- Native BCC64X PostgreSQL client build. No MSVC, clang-cl or MinGW fallback.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src\interfaces\libpq\libpq.dll -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\libpq.dll`
- `{Workspace}\src\interfaces\libpq\libpq-fe.h -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\libpq-fe.h`
- `{Workspace}\src\interfaces\libpq\libpq-events.h -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\libpq-events.h`
- `{Workspace}\src\include\postgres_ext.h -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\postgres_ext.h`
- `{Workspace}\COPYRIGHT -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYRIGHT`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\libpq\consumer)`

**Selected environment inputs:**

- `CC={Tool:bcc64x}`
- `CFLAGS=-DWIN32`
- `LDFLAGS=-L{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\compat-lib`
- `CXX={Tool:bcc64x}`
- `AR={Tool:llvm-ar}`
- `RANLIB={Tool:llvm-ranlib}`
- `WINDRES={Tool:bcc64x-windres}`
- `PKG_CONFIG={Tool:pkg-config}`
- `PKG_CONFIG_PATH={DependencyRoot:openssl}\lib\win64\{Configuration}\pkgconfig;{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\pkgconfig`
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
