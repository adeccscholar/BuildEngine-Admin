# opengl 26.2.1: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `opengl@26.2.1` · **Category:** graphics · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Use Mesa with Meson/Ninja and BCC64X as the actual compiler; produce Windows WGL/desktop OpenGL using Gallium softpipe. Explicitly omit Vulkan/GLES/EGL/GLX/GBM/LLVM infrastructure. Extract the actual DLL exports with tdump and create matching BCC64X import libraries using lld rather than copying foreign compiler artifacts.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

No direct BuildEngine library dependency is declared for this coordinate. This does **not** mean the generated binary uses no Windows system libraries or that its upstream bundled components are absent.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `opengl` | `true` |
| `build-tests` | `true` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `gles1` | `disabled` |
| `gles2` | `disabled` |
| `egl` | `disabled` |
| `glx` | `disabled` |
| `gbm` | `disabled` |
| `llvm` | `disabled` |
| `zlib` | `disabled` |
| `zstd` | `disabled` |
| `xmlconfig` | `disabled` |
| `gallium-rusticl` | `false` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `platforms` | `windows` |
| `gallium-drivers` | `softpipe` |
| `gallium-wgl-dll-name` | `libgallium_wgl` |


## Build architecture, variants and paths

**Selected build flow:** GN/Ninja and explicit native toolchain targets. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- None declared for this category.

**Versioned source locations:**

- `https://archive.mesa3d.org/mesa-{LibraryVersion}.tar.xz`
- `https://files.pythonhosted.org/packages/source/m/meson/meson-1.12.0.tar.gz`
- `https://files.pythonhosted.org/packages/7d/fa/3944b40b07da9ce895c0e6303a5ab7d53da063554f534556b134a54d6093/packaging-26.3.tar.gz`
- `https://files.pythonhosted.org/packages/7e/99/7690b6d4034fffd95959cbe0c02de8deb3098cc577c67bb6a24fe5d7caa7/markupsafe-3.0.3.tar.gz`
- `https://files.pythonhosted.org/packages/2a/12/b5fa2353e2754cd67fb9f83793fa48ff42c213a5da7e719869d2301f6ab8/mako-1.4.1.tar.gz`
- `https://files.pythonhosted.org/packages/05/8e/961c0007c59b8dd7729d542c61a4d537767a59645b82a0b521206e1e25c2/pyyaml-6.0.3.tar.gz`

**Declared extraction profiles:**

- `libarchive / mesa-{LibraryVersion}`
- `libarchive / meson-1.12.0`
- `libarchive / packaging-26.3`
- `libarchive / markupsafe-3.0.3`
- `libarchive / mako-1.4.1`
- `libarchive / pyyaml-6.0.3`

**Package-path convention:** `{InstallRoot}/packages/opengl/26.2.1/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Engineering constraints recorded in the executable contract

- Official Mesa release archive; checksum is published in the Mesa 26.2.1 release notes.
- Meson is part of the reproducibility contract, not a host prerequisite.
- Mesa mandatory Python configure-time modules. Keep them pinned and source-local so the build does not depend on global pip/site-packages.
- Native BCC64X Mesa build. No MSVC, clang-cl or MinGW fallback is permitted.
- Import libraries are derived only from the BCC64X-built Mesa DLLs.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin\win64\{Configuration} -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{BuildFileDir}\cmake\opengl\OpenGLConfig.cmake -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\OpenGL\OpenGLConfig.cmake`
- `{Workspace}\include\GL -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL`
- `{Workspace}\include\KHR -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\KHR`
- `{Workspace}\docs\license.rst -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE-MESA.rst`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\opengl\consumer)`

**Selected environment inputs:**

- `CC={Tool:bcc64x}`
- `CXX={Tool:bcc64x}`
- `AR={Tool:llvm-ar}`
- `WINDRES={Tool:bcc64x-windres}`
- `PYTHONPATH={Workspace}\packaging-26.3\src;{Workspace}\markupsafe-3.0.3\src;{Workspace}\mako-1.4.1;{Workspace}\pyyaml-6.0.3\lib`
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
