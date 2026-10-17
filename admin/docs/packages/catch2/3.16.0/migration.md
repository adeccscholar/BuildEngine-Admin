# catch2 3.16.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `catch2@3.16.0` · **Category:** testing · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Produce Catch2 as an intentionally static test-framework package, but continue to use dynamic BCC64X runtime linkage for executables. Fix compiler detection by configuration (NO_POLYFILL_ISNAN) and use an explicit UTF-8 execution code page for upstream XML tests, without making those quirks global compiler settings.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

No direct BuildEngine library dependency is declared for this coordinate. This does **not** mean the generated binary uses no Windows system libraries or that its upstream bundled components are absent.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `CATCH_ENABLE_REPRODUCIBLE_BUILD` | `ON` |
| `CATCH_INSTALL_EXTRAS` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_SHARED_LIBS` | `OFF` |
| `CATCH_ENABLE_WERROR` | `OFF` |
| `CATCH_BUILD_EXAMPLES` | `OFF` |
| `CATCH_BUILD_EXTRA_TESTS` | `OFF` |
| `CATCH_BUILD_FUZZERS` | `OFF` |
| `CATCH_BUILD_BENCHMARKS` | `OFF` |
| `CATCH_ENABLE_CONFIGURE_TESTS` | `OFF` |
| `CATCH_ENABLE_CMAKE_HELPER_TESTS` | `OFF` |
| `CATCH_INSTALL_DOCS` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `CATCH_DEVELOPMENT_BUILD` | `{TestsEnabled}` |
| `CATCH_BUILD_TESTING` | `{TestsEnabled}` |
| `BUILD_TESTING` | `{TestsEnabled}` |
| `Python3_EXECUTABLE` | `{Tool:python}` |
| `Catch2_DIR` | `{ConsumerRoot}\lib\win64\{Configuration}\cmake\Catch2` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_CXX_FLAGS:STRING=-DCATCH_CONFIG_NO_POLYFILL_ISNAN`
- `-DCMAKE_CXX_STANDARD:STRING=23`
- `-DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON`
- `-DCMAKE_CXX_EXTENSIONS:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Versioned source locations:**

- `https://github.com/catchorg/Catch2/archive/refs/tags/v3.16.0.zip`

**Declared extraction profiles:**

- `zip / Catch2-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/catch2/3.16.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Engineering constraints recorded in the executable contract

- Catch2 3.16 identifies every compiler defining __BORLANDC__ as the historic Embarcadero compiler family and therefore enables CATCH_CONFIG_POLYFILL_ISNAN. That polyfill calls std::_isnan(), which is not available in the modern BCC64X 20.1.7 standard-library environment. Catch2 provides CATCH_CONFIG_NO_POLYFILL_ISNAN as the supported negative override, so no source patch is required. Catch2's UTF-8
- Catch2 installs its CMake package relative to CMAKE_INSTALL_LIBDIR. Our variant layout therefore places it below lib/win64/{Configuration}/cmake/Catch2. A package smoke uses the package root as CMAKE_PREFIX_PATH, so the variant-specific config location is supplied explicitly instead of relocating Catch2's generated CMake export files.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\LICENSE.txt -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses\catch2\LICENSE.txt`

**Declared smoke tests:**

- `consumer (Release, scope package; source smokes\catch2\consumer)`
- `consumer (Debug, scope package; source smokes\catch2\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
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
