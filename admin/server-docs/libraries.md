# Integrated Third-Party Libraries

[TOC|Content]

This page is the public engineering overview of the third-party libraries currently integrated into BuildEngine for the Win64 Modern BCC64X toolchain. The authoritative executable contract remains `admin/build-libraries.xml`; this page explains the dependency graph, integration shape, deliberate deviations from upstream defaults, version-bound patches, packaging repairs, and important verification choices.

The corresponding internal engineering findings are maintained in `docs/bcc64x-library-integration-findings.md`. Both documents must be updated when a library is added, removed, upgraded, patched, or materially reconfigured.

Each logical library also carries a concise one-line purpose description in its contract metadata. The description identifies what the upstream library is for; build-specific details remain in the integration notes so that product metadata and integration evidence do not become mixed.

## Integration rules

The current integration follows a few project-wide rules:

- BCC64X is the compiler path under investigation. A library is not silently moved to MSVC, clang-cl, MinGW, or another compiler to make it pass.
- Versions and dependencies are explicit BuildEngine contract data rather than accidental host discovery.
- Shared libraries are the normal product shape unless a library is intrinsically header-only or there is a deliberate, documented reason for a static package.
- Release and Debug use separate variant directories and are tested independently where the package contract requires both.
- Source repairs are version-bound and checked with `git apply --check` before application.
- Packaging repairs are preferred over source patches when the source itself is compatible but upstream install/export metadata is incomplete.
- Consumer smoke tests validate the installed package rather than only the build tree.
- BuildEngine dependencies remain visible in the DAG, package metadata, and SBOM. Bundled upstream components are tracked separately.
- A logical library boundary does not require an artificial physical output boundary when the upstream build system deliberately shares a producer tree. ACE and TAO are the current explicit example.

## Dependency overview

```mermaid
flowchart LR
   zlib --> libzip
   xz --> libzip
   zstd --> libzip

   zlib --> libarchive
   xz --> libarchive
   zstd --> libarchive
   bzip2 --> libarchive
   openssl --> libarchive

   zlib --> openssl
   brotli --> openssl
   zstd --> openssl

   openssl --> curl
   zlib --> curl

   openssl --> boost
   zlib --> boost

   openssl --> ace[ACE]
   xerces[Xerces-C] --> ace
   zlib --> ace
   ace --> tao[TAO]

   opengl[OpenGL / Mesa] --> glew
   opengl --> raylib
   opengl --> soil2

   zlib --> libpng
   zlib --> libtiff
   jpeg[libjpeg-turbo] --> libtiff

   zlib --> freetype[FreeType]
   bzip2 --> freetype
   brotli --> freetype
   libpng --> freetype
   harfbuzz[HarfBuzz] --> freetype

   opengl --> skia
   zlib --> skia
   brotli --> skia
   jpeg --> skia
   libpng --> skia
   harfbuzz --> skia
   freetype --> skia

   zlib --> opencv[OpenCV]
```

HarfBuzz is deliberately built without FreeType. FreeType is then built with HarfBuzz, keeping that part of the graph acyclic.

ACE and TAO are separate logical libraries in the dependency graph. This does **not** mean that their upstream output is physically separated. TAO is built on the already prepared ACE `ACE_wrappers` tree; `TAO_ROOT` is below `ACE_ROOT`, and ACE and TAO both write libraries and tools into the shared `ACE_wrappers\lib` and `ACE_wrappers\bin` directories. The logical `tao -> ace` edge therefore describes scheduling, state, metadata and SBOM ownership while preserving the upstream physical layout.

## Current inventory

| Library | Version | Category | Description | BuildEngine dependencies | Integration notes |
| --- | --- | --- | --- | --- | --- |
| pugiXML | 1.16 | data | Lightweight C++ XML processing library with DOM-style parsing and XPath support. | — | Shared package, Release/Debug, installed package smoke. |
| zlib | 1.3.2 | compression | General-purpose DEFLATE compression library used as a foundational compression dependency. | — | Core compression dependency used by several packages. |
| Brotli | 1.2.0 | compression | Lossless compression library and codec optimized for web and general data compression. | — | Shared compression package; also consumed by OpenSSL, FreeType, and Skia. |
| Zstandard | 1.5.7 | compression | Fast lossless compression library offering a wide compression-speed trade-off. | — | Shared compression package used by libzip, libarchive, and OpenSSL. |
| XZ / liblzma | 5.8.3 | compression | LZMA/LZMA2 compression library and XZ container implementation. | — | Shared compression package used by libzip and libarchive. |
| libzip | 1.11.4 | archive | C library for reading, creating, and modifying ZIP archives. | zlib, XZ, Zstandard | Shared library plus zip tools; AES/XZ/Zstd functional checks; upstream documentation build is not part of the package build because it writes shared source-tree outputs. |
| libarchive | 3.8.9 | archive | Multi-format archive and compression library backing tar/cpio-style workflows. | zlib, XZ, Zstandard, bzip2, OpenSSL | Shared libarchive plus tar/cpio/cat/unzip; BZip2 enabled explicitly for in-process `.tar.bz2` extraction; BCC64X compatibility patch for legacy `__BORLANDC__` branches and Windows/Clang details. |
| OpenSSL | 3.5.8 | security | TLS/SSL and general-purpose cryptography toolkit and provider library. | zlib, Brotli, Zstandard | Native BCC64X Configure target, shared build, version-bound linker/Windows/PDB patches; no-asm baseline. |
| curl | 8.21.0 | network | Client-side URL transfer library with HTTP(S) and related protocol support. | OpenSSL, zlib | Shared libcurl baseline with OpenSSL + zlib; Schannel, Brotli, Zstd and several optional protocol dependencies deliberately disabled in the current proof. |
| Boost | 1.92.0 | foundation | Large collection of portable C++ libraries extending the standard library ecosystem. | OpenSSL, zlib | C++23 shared build from the official CMake tree; broad selected module set; MPI/Python excluded; BCC64X native-Clang preflight gates. |
| nlohmann-json | 3.12.0 | data | Header-only modern C++ JSON parser, serializer, and data model. | — | Header-only package. |
| ACE | 8.0.6 | middleware | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. | OpenSSL, Xerces-C, zlib | Native BCC64X MPC/BMake base; owns the shared `ACE_wrappers` producer tree used later by TAO. |
| TAO | 4.0.6 | middleware | CORBA object request broker and middleware services built on top of ACE. | ACE | Logical TAO package continues inside ACE's physical `ACE_wrappers` tree; shared `bin`/`lib`, `tao_idl`, Naming, COS Event and RT Event services. |
| bzip2 | 1.0.8 | compression | Lossless block-sorting compression library implementing the bzip2 format. | — | BuildEngine CMake wrapper creates the shared Win64 package and package metadata. |
| GLEW | 2.3.1 | graphics | OpenGL extension loading library exposing modern OpenGL entry points. | OpenGL | Shared package built against the managed OpenGL package. |
| OpenGL / Mesa | 26.2.1 | graphics | Mesa software OpenGL implementation providing the managed Windows desktop OpenGL runtime. | — | Mesa/Meson softpipe WGL profile; OpenGL enabled, LLVM/Vulkan/GLES/EGL/GLX disabled; BCC64X-derived import libraries generated from built DLLs. |
| raylib | 6.0 | graphics | Small C game-programming library for graphics, input, audio, and windowing. | OpenGL | BuildEngine CMake wrapper, shared Release/Debug package. |
| SDL2 | 2.32.10 | graphics | Cross-platform low-level multimedia library for windows, input, audio, and graphics integration. | — | Shared-only package; test targets and Direct3D renderer paths disabled in the current Windows proof. |
| SQLite | 3.53.4 | database | Embedded transactional SQL database engine delivered as a self-contained library. | — | Amalgamation-based shared package with BuildEngine CMake wrapper. |
| Xerces-C | 3.3.0 | data | C++ XML parser and validation library implementing DOM/SAX and XML Schema support. | — | Shared Windows package using Winsock, Windows transcoder and in-memory message loader; upstream API-documentation target retained. |
| SOIL2 | 1.3.0 | graphics | Small OpenGL texture-loading library for common image formats. | OpenGL | BuildEngine CMake wrapper and managed OpenGL package. |
| VTK | 9.6.2 | graphics | Visualization Toolkit for scientific data structures, processing, and visualization pipelines. | — | Deliberately narrow shared profile: CommonCore, CommonDataModel and FiltersCore; large module groups and language wrappers disabled. |
| OpenCV | 5.0.0 | graphics | Computer vision and image-processing library with broad algorithm and matrix support. | zlib | Reduced package profile with a version-bound BCC64X patch making MLAS optional. |
| cmark-gfm | 0.29.0.gfm.13 | documentation | GitHub-flavored CommonMark parser and renderer used for BuildEngine documentation. | — | GitHub-flavored CommonMark library used by BuildEngine documentation rendering. |
| GoogleTest | 1.17.0 | testing | C++ unit-testing and mocking framework from the GoogleTest project. | — | Managed test-framework package with CMake package consumption. |
| Catch2 | 3.16.0 | testing | Modern C++ unit-testing framework with self-contained test registration and assertions. | — | Deliberate static test-framework package; BCC64X-specific compile definitions/code-page setting, no source patch. |
| BitmapPlusPlus | 1.1.1 | graphics | Header-only C++ bitmap utility for reading, writing, and manipulating BMP images. | — | Header-only; no artificial binary build; BuildEngine supplies missing package-config metadata and Release/Debug consumer smokes. |
| libjpeg-turbo | 3.2.0 | image | High-performance JPEG codec using the libjpeg API and TurboJPEG interface. | — | Shared library, TurboJPEG enabled, SIMD disabled for the current baseline. |
| libpng | 1.6.58 | image | Reference PNG image encoding and decoding library. | zlib | Shared package using managed zlib; static library and upstream tests disabled in the current contract. |
| HarfBuzz | 14.4.0 | text | OpenType text shaping engine converting Unicode text into positioned glyphs. | — | Shared core shaping + subset package; optional frameworks and FreeType disabled to keep the graph acyclic; package/export repairs applied at install time. |
| FreeType | 2.14.3 | text | Font rasterization engine for loading and rendering scalable and bitmap fonts. | zlib, bzip2, Brotli, libpng, HarfBuzz | Shared package with all declared dependencies explicitly required; HarfBuzz integration is part of the consumer proof. |
| libtiff | 4.7.2 | image | TIFF image file reading, writing, and image metadata library. | zlib, libjpeg-turbo | Shared package; tools/contrib/docs/tests disabled in the current contract. |
| Skia | 153 | graphics | 2D graphics engine for raster, vector, text, image, and GPU-backed rendering. | OpenGL, zlib, Brotli, libjpeg-turbo, libpng, HarfBuzz, FreeType | Broad Windows desktop component build at pinned source commit `2eed75b956045eb8603d3690a1e84bc582a2135d`; extensive BCC64X GN/system-library/component repairs and bundled-component SBOM evidence. |

## Archive and compression stack

### zlib, Brotli, Zstandard, XZ and bzip2

These libraries form the reusable compression foundation. Their versions are explicit dependencies of downstream packages rather than libraries discovered from an arbitrary machine installation. This matters especially for OpenSSL, libzip, libarchive, FreeType and Skia, because package metadata and SBOM relationships must describe the exact producer that was used.

BZip2 1.0.8 is now also an explicit libarchive dependency. Release uses `bzip2.lib`, Debug uses `bzip2d.lib`. This is also the source for rebuilding BuildEngine's private libarchive runtime so `.tar.bz2`, `.tbz2` and `.tbz` can be handled in-process.

### libzip 1.11.4

libzip consumes the managed zlib, XZ and Zstandard packages. The package contains the shared library and the upstream command-line tools. BuildEngine runs functional checks for Zstandard, XZ and AES-256 archive operations.

Upstream installs some command-line tools to a literal `bin` location instead of respecting the configuration-specific BuildEngine bindir. BuildEngine keeps the upstream package metadata intact and additionally copies the actual tools into the variant-specific package layout.

The upstream documentation target is deliberately not part of the current build contract because it writes common generated files into the shared source tree and therefore conflicts with parallel Release/Debug builds. This is a build-system/output-layout constraint, not a reason to serialize the entire BuildEngine graph.

### libarchive 3.8.9

libarchive consumes zlib, XZ, Zstandard, bzip2 and OpenSSL and enables the native tar, cpio, cat and unzip tools. BZip2 is enabled explicitly and bound to the managed `bzip2 1.0.8` package rather than to an accidental host installation. CNG and Windows XMLLite are enabled; optional backends not represented by explicit BuildEngine dependencies are disabled.

The version-bound `bcc64x-modern-borland-compat.patch` separates modern Clang-based BCC64X behavior from legacy compiler branches guarded only by `__BORLANDC__`. It covers inline/integer/literal handling, Windows `lseek` and `mbstate_t` handling, open-mode signatures and related tests. It also incorporates the relevant upstream Windows/Clang `filter_fork_windows.c` correction and resolves a BCC64X `ftruncate` macro collision without weakening Debug `-Werror`.

## Security and network stack

### OpenSSL 3.5.8

OpenSSL uses a native BCC64X Configure platform named `BC-64X`. The tool contract supplies BCC64X, `llvm-ar`, `llvm-ranlib`, BCC64X windres and NASM. The current reproducible baseline is shared and uses `no-asm`; zlib, Brotli and Zstandard are explicit package dependencies.

The current version-bound repairs include:

- linker-response path normalization for BCC64X/LLD;
- normalization of external compression-library arguments written to response files;
- a QUIC/Windows header compatibility repair for the modern Clang-based Embarcadero compiler;
- a dedicated BCC64X OpenSSL platform using PDB rather than the legacy C++Builder TDS debug-sidecar model.

Brotli's BCC64X producer names its import libraries `libbrotli*.lib`, while OpenSSL's Windows integration expects unprefixed names. BuildEngine creates private consumer-side aliases inside the OpenSSL build tree; producer artifacts are not renamed.

### curl 8.21.0

The current curl contract intentionally re-establishes a narrow known baseline first: TLS through managed OpenSSL and compression through managed zlib. Schannel is disabled and Brotli/Zstd are currently not enabled for curl even though those packages exist independently. Optional c-ares, IDN2, libpsl, SSH and HTTP/3 dependency paths are also disabled.

CMake's Windows `FindOpenSSL` behavior is handled with explicit include/library search information for the variant-specific BuildEngine package. BuildEngine does not fall back to an unrelated OpenSSL installation on the host.

## Foundation and middleware

### Boost 1.92.0

Boost is built from the official CMake source distribution with C++23, shared libraries and a broad selected module closure. The current contract represents 148 selected source modules covering the verified logical library set; MPI- and Python-dependent ecosystems remain excluded.

OpenSSL and zlib are exact managed dependencies. The build includes early native-BCC64X/Boost.Config preflight compilations so a compiler-model regression fails before the expensive Boost graph is started. The optional Boost.Random `random_device` binary is disabled because the verified contract treats Boost.Random as header/interface use.

### ACE 8.0.6 and TAO 4.0.6

ACE and TAO are modeled as separate logical libraries because their dependency and product roles are different: ACE is the portable communication/concurrency foundation, while TAO is the CORBA implementation and service layer built on ACE. The direct BuildEngine edge is therefore `tao -> ace`; ACE itself depends on OpenSSL, Xerces-C and zlib. This also gives metadata and SBOM generation the correct direct/transitive relationship.

The logical split deliberately does **not** split the upstream producer tree. Both libraries come from the same ACE+TAO release and continue to use the same variant-specific `ACE_wrappers` tree. TAO builds with `ACE_ROOT` pointing at that existing ACE tree and `TAO_ROOT` at `ACE_ROOT\TAO`. In particular, TAO's generated libraries, DLLs and tools continue to appear in the same `ACE_wrappers\lib` and `ACE_wrappers\bin` directories used by ACE. BuildEngine must not invent a second physical TAO `bin` or `lib` tree merely because the scheduler has two logical library IDs.

The package view can still assign ownership deliberately: ACE publishes its base headers/runtime artifacts; TAO publishes TAO/orbsvcs headers, `tao_idl`, Naming, COS Event and RT Event services and the TAO-specific artifacts produced in the shared tree. The physical producer overlap is therefore explicit and intentional rather than hidden by filesystem discovery.

Both continue to use their native MPC/BMake path with BCC64X. The integration remains a direct BCC64X ecosystem proof; alternative compiler paths are not substituted for failed pieces.

## Graphics stack

### OpenGL / Mesa 26.2.1

The OpenGL package is produced from Mesa with Meson/Ninja and the BCC64X toolchain. The current profile targets Windows with Gallium softpipe/WGL and desktop OpenGL. GLES, EGL, GLX, GBM, Vulkan, LLVM and unrelated tools are disabled.

Import libraries are generated from the actual BCC64X-built DLL exports using `tdump` and `ld.lld`; they are not borrowed from another compiler installation. A small BuildEngine CMake package describes the resulting OpenGL package to consumers.

### GLEW, raylib and SOIL2

These packages consume the managed OpenGL package explicitly. Where upstream does not provide an installation shape suitable for BuildEngine, a small Admin-side CMake wrapper provides the build/install/package contract without changing the library's public source API.

### SDL2 2.32.10

SDL2 is a shared-only package in the current contract. The first BCC64X profile deliberately disables SDL tests and Direct3D renderer paths. Those are explicit current settings, not a general statement that those features can never be supported.

### VTK 9.6.2

The VTK proof is intentionally a small, controlled subset: `CommonCore`, `CommonDataModel` and `FiltersCore`. StandAlone/Rendering/Web/MPI groups and Python/Java wrappers are disabled. This establishes native library/package compatibility before expanding the very large optional VTK graph.

### OpenCV 5.0.0

OpenCV consumes managed zlib. The version-bound `bcc64x-optional-mlas.patch` makes the MLAS path optional for the BCC64X package profile rather than forcing an unrelated compiler/runtime dependency into the proof.

### BitmapPlusPlus 1.1.1

BitmapPlusPlus is header-only. Upstream defines an INTERFACE target but no install/package-config contract, so BuildEngine does not invent a binary build. It installs the original header and license and supplies only the missing CMake package discovery metadata. Release and Debug consumer smokes exercise real BMP write/read behavior.

## Image and text stack

### libjpeg-turbo 3.2.0

The package is shared, with TurboJPEG enabled and static output disabled. SIMD remains disabled for the current controlled baseline. Skia consumes this BuildEngine package rather than building another libjpeg-turbo copy.

### libpng 1.6.58

libpng is shared and consumes the exact zlib package. `PNG_TESTS` is currently disabled. The package is also an explicit input to FreeType and Skia.

For Skia's GN/BCC64X integration, the absolute import-library path must remain a raw linker flag rather than being converted into GN's normal `-l...` library form.

### HarfBuzz 14.4.0

HarfBuzz is built without FreeType and without optional Cairo, Graphite2, GLib, ICU, GObject, Uniscribe, GDI or DirectWrite integration. Core shaping and subset support are enabled; raster/vector/GPU/utilities are not part of the package.

Two upstream packaging gaps are repaired without patching the HarfBuzz source:

1. the generated CMake export can reference `Threads::Threads` without resolving the Threads dependency, so BuildEngine installs a small package wrapper around the original generated targets;
2. the CMake install path omits `hb-subset-depend.h` although the installed public subset header requires it, so BuildEngine copies that public header explicitly.

### FreeType 2.14.3

FreeType requires managed zlib, bzip2, Brotli, libpng and HarfBuzz. The direction HarfBuzz -> FreeType is deliberate: HarfBuzz itself is built without FreeType, then FreeType is built with HarfBuzz. The resulting package therefore has a complete but acyclic dependency graph.

Package smokes must resolve the transitive runtime DLL closure, not only direct dependencies. FreeType and its bzip2 path were one of the cases that made this requirement visible.

### libtiff 4.7.2

libtiff uses managed zlib and libjpeg-turbo. The package is shared; optional tools, contrib code, documentation and upstream tests are disabled in the current contract. Public headers and consumer use remain explicit package gates.

## Documentation and testing libraries

### cmark-gfm 0.29.0.gfm.13

cmark-gfm provides the CommonMark/GFM parsing layer used by BuildEngine's live Markdown documentation. It is managed like the other third-party packages rather than being taken from the host system.

### GoogleTest 1.17.0

GoogleTest is a managed testing dependency and is consumed through its installed CMake package.

### Catch2 3.16.0

Catch2 is deliberately static in this project because it is a test framework. Its current BCC64X integration requires two targeted settings rather than a source patch:

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
-fborland-exec-code-page=65001
```

The first avoids Catch2's legacy `__BORLANDC__` path that expects `std::_isnan()`. The second makes ordinary narrow string literals use UTF-8 for the upstream XML encoding test. The setting remains local to Catch2; it is not silently made a global BCC64X policy.

## Skia 153

Skia is the broadest integration in the current contract. The logical package version is `153`; the reproducible source pin is commit `2eed75b956045eb8603d3690a1e84bc582a2135d`.

BuildEngine supplies OpenGL, zlib, Brotli, libjpeg-turbo, libpng, HarfBuzz and FreeType as external packages. Those exact package versions therefore remain visible in the dependency graph and SBOM instead of being downloaded again inside Skia.

Other components remain intentionally bundled at Skia's pinned revisions, including ICU, WebP, libavif/libgav1, JPEG XL, Wuffs, DNG SDK, Expat, SPIR-V/Vulkan support components and the Vulkan/D3D memory allocators. Bundled components are recorded separately from BuildEngine package dependencies.

The Windows desktop profile includes CPU raster/skcms, DirectWrite/GDI, FreeType, ICU/HarfBuzz shaping, PNG/JPEG/WebP/Wuffs, AVIF/JPEG XL/DNG, SVG, PDF/XPS, Skottie, Ganesh, Graphite, OpenGL, Direct3D 12 and Vulkan. Web, mobile, Dawn/ANGLE, Rust codecs, FFmpeg/Lua and developer/fuzzer integrations are excluded from this package profile.

The version-bound BCC64X integration includes, among other repairs:

- GN toolchain support for BCC64X;
- propagation of raw system linker flags through GN target chains;
- Windows desktop/BCC64X compatibility changes;
- correction of the Debug SPIR-V validator condition;
- `SkArenaAlloc` export across component boundaries;
- zlib and other BuildEngine system-library routing;
- separate system integrations for Brotli, libjpeg-turbo, libpng, HarfBuzz and FreeType;
- targeted component/export handling rather than blanket `--export-all-symbols`.

A generic BCC64X UCRT runtime defect discovered while testing Skia is not hidden in a Skia source patch. BuildEngine creates the `bcc64x-ucrt-compat` archive from the installed Embarcadero runtime objects and places it before the normal runtime during linking. No Embarcadero object files are distributed in the repository.

## Package repairs versus source patches

The distinction is intentional:

| Kind | Examples | Project handling |
| --- | --- | --- |
| Source/build-system compatibility | libarchive legacy Borland branches, OpenSSL response files, Skia GN/toolchain/component integration, OpenCV MLAS | Exact-version patch, checked before application. |
| Packaging/export repair | HarfBuzz CMake wrapper and missing public subset header, BitmapPlusPlus package config | Admin-side install/package metadata; upstream source remains unchanged. |
| Compiler-policy setting | Catch2 `NO_POLYFILL_ISNAN`, BCC64X execution code page | Local library build contract unless proven generic. |
| Generic toolchain/runtime defect | BCC64X UCRT compatibility archive | BuildEngine/tool layer, not a library source patch. |

## Maintenance contract

This page and `docs/bcc64x-library-integration-findings.md` are documentation outputs of the library integration work, not optional historical notes.

Whenever `admin/build-libraries.xml` changes materially, maintainers must update the applicable documentation in the same work unit:

- add/remove/upgrade a library -> update the inventory and dependency information here;
- add/change a library purpose description -> update `metadata/@description` and the inventory description together;
- add/remove a patch -> update the relevant integration notes and identify whether it is source, packaging, compiler-policy, or generic-toolchain work;
- change important feature/test/build settings -> update the library's note;
- change dependency edges -> update both the inventory and dependency overview;
- change an intentional shared producer layout such as ACE/TAO -> document both the logical ownership and the physical output relationship;
- record deeper diagnostic evidence -> update the internal findings document under `docs/` as well.

The XML contract remains authoritative when documentation and executable configuration ever disagree; such a disagreement is a documentation defect to be corrected, not a second source of truth.