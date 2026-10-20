# Integrated Third-Party Libraries

[TOC|Content]

This page is the public engineering overview of the third-party libraries currently integrated into BuildEngine for the Win64 Modern BCC64X toolchain. The authoritative executable contract remains `admin/build-libraries.xml`; this page explains the dependency graph, integration shape, deliberate deviations from upstream defaults, version-bound patches, packaging repairs, and important verification choices.

The corresponding internal engineering findings are maintained in `docs/bcc64x-library-integration-findings.md`. Both documents must be updated when a library is added, removed, upgraded, patched, or materially reconfigured.

Each logical library also carries a concise one-line purpose description in its contract metadata. The description identifies what the upstream library is for; build-specific details remain in the integration notes so that product metadata and integration evidence do not become mixed.

## Integration rules

The current integration follows a few project-wide rules:

- BCC64X is the compiler path under investigation. It is Clang/LLVM with Embarcadero simulation and GNU-compatible target/header conventions; MinGW compatibility macros do not make it a MinGW/GCC compiler. A library is not silently moved to MSVC, clang-cl, MinGW, or another compiler to make it pass.
- Versions and dependencies are explicit BuildEngine contract data rather than accidental host discovery.
- Shared libraries are the normal product shape unless a library is intrinsically header-only or there is a deliberate, documented reason for a static package.
- Library product form is separate from compiler-runtime linkage: every BCC64X Windows EXE, DLL and MODULE uses the dynamic BCC64X runtime (`-tR`). Static BCC64X runtime linkage is forbidden.
- Release and Debug use separate variant directories and are tested independently where the package contract requires both.
- Source repairs are version-bound and checked with `git apply --check` before application.
- Packaging repairs are preferred over source patches when the source itself is compatible but upstream install/export metadata is incomplete.
- Consumer smoke tests validate the installed package rather than only the build tree.
- BuildEngine dependencies remain visible in the DAG, package metadata, and SBOM. Bundled upstream components are tracked separately.
- A logical library boundary does not require an artificial physical output boundary when the upstream build system deliberately shares a producer tree. ACE and TAO are the current explicit example.

## License traffic light

The license traffic light is an engineering integration marker for proprietary product use:

- **🟢 Green** — permissive/public-domain style use; normally suitable for proprietary software when required notices, attribution and other ordinary license obligations are preserved.
- **🟡 Yellow** — product-specific review required; typical causes are weak copyleft, dual/multi licensing, mixed-license distributions, bundled components, or additional redistribution obligations.
- **🔴 Red** — do not plan as an in-process dependency of proprietary software; strong copyleft requires a different product/distribution architecture or another licensing path.

The marker does not replace the original upstream license text. See [Open-Source License Overview](licenses.md) for the general rules.

## Dependency overview

The diagram highlights current integration boundaries; it is intentionally
selective. The **complete, version-resolved dependency inventory** is in the
table below and the authoritative `admin/build-libraries.xml` contract.

```mermaid
flowchart LR
    zlib["zlib 1.3.2"] --> libarchive["libarchive 3.8.10"]
    xz["XZ 5.8.4"] --> libarchive
    bzip2["BZip2 1.0.8"] --> libarchive
    openssl["OpenSSL 3.5.9"] --> curl["curl 8.23.0"]
    openssl --> libpq["libpq 18.6"]
    libpq --> libpqxx["libpqxx 8.0.2 (package extension)"]
    openssl --> ace["ACE 8.0.6"]
    ace --> tao["TAO 4.0.6 (shared-source extension)"]
    harfbuzz["HarfBuzz 14.4.0"] --> freetype["FreeType 2.14.3"]
    freetype --> skia144["Skia 144 (pinned)"]
    skia144 --> pdfium["PDFium 2025.11.19"]
    abseil["Abseil 20260817.0 (external)"] --> pdfium
    icu["ICU 78.3"] --> pdfium
    lcms["LittleCMS 2.19.1"] --> pdfium
    openjpeg["OpenJPEG 2.5.4"] --> pdfium
    sdl2["SDL2 2.32.10"] --> viewerSDL["PDFium SDL2 GUI test"]
    pdfium --> viewerSDL
    pdfium --> viewerVCL["PDFium VCL GUI test"]
    skia153["Skia 153 (current general version)"]
```

Notable distinctions:

- The XML retains **six explicitly superseded versions** for reproducible
  consumers: XZ 5.8.3 → 5.8.4, libarchive 3.8.9 → 3.8.10,
  OpenSSL 3.5.8 → 3.5.9, curl 8.21.0 → 8.23.0,
  Skia 144 → 153 and Expat 2.8.4 → 2.9.0. Supersession
  does not silently rewrite pinned dependency edges.
- PDFium 2025.11.19 **still pins Skia 144**. Skia 153 is the later
  general-purpose version; it is not a substitute for the tested PDFium
  dependency without an explicit migration and rebuild.
- `tao@4.0.6` is a shared-source extension of `ace@8.0.6`,
  while `libpqxx@8.0.2` is a package extension of `libpq@18.6`.
  Both retain independent logical metadata and scheduling.
- HarfBuzz is built independently of FreeType; FreeType consumes HarfBuzz.
  PDFium's Abseil is the external `abseil@20260817.0` runtime, **not**
  the retired `third_party_abseil-cpp_absl.dll` Chromium component.

## Current inventory (21 October 2026)

The **Schema-17** XML declares **58 versioned library records with 52
distinct logical library IDs**. The difference is six retained predecessor
versions, not six failed or duplicate libraries. The catalog includes only
versioned `<library>` elements, not explanatory XML examples or historical
comments.

The table is a **declarative inventory**. A declared smoke test is *not*
a claim that it passed: `R/pub` = Release published-scope smoke,
`D/pkg` = Debug installed-package smoke, and `—` = no smoke defined.
The exact configurations and scope flags follow the live XML.

| Library ID | Version / supersession | License signal | Category | Purpose (contract metadata) | Direct dependencies / extension | Declared smokes | Detail |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `pugixml` | `1.16` | 🟢 | data | Lightweight C++ XML processing library with DOM-style parsing, modification, and XPath support. | — | R/pub | [Migration](/package/pugixml/1.16/migration) |
| `zlib` | `1.3.2` | 🟢 | compression | General-purpose DEFLATE compression library used for gzip-compatible data and streams. | — | R/pub | [Migration](/package/zlib/1.3.2/migration) |
| `brotli` | `1.2.0` | 🟢 | compression | General-purpose lossless compression library providing Brotli encoder and decoder support. | — | R/pub | [Migration](/package/brotli/1.2.0/migration) |
| `zstd` | `1.5.7` | 🟢 | compression | High-performance lossless compression library implementing the Zstandard format. | — | R/pub | [Migration](/package/zstd/1.5.7/migration) |
| `xz` | `5.8.3` → `5.8.4` (retained) | 🟡 | compression | Data compression library and tools providing the LZMA and LZMA2 algorithms through liblzma. | — | R/pub | [Migration](/package/xz/5.8.3/migration) |
| `xz` | `5.8.4` | 🟡 | compression | Data compression library and tools providing the LZMA and LZMA2 algorithms through liblzma. | — | R/pub | [Migration](/package/xz/5.8.4/migration) |
| `libzip` | `1.11.4` | 🟢 | archive | C library for reading, creating, and modifying ZIP archives. | zlib 1.3.2, xz 5.8.4, zstd 1.5.7 | R/pub | [Migration](/package/libzip/1.11.4/migration) |
| `libarchive` | `3.8.9` → `3.8.10` (retained) | 🟡 | archive | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib 1.3.2, xz 5.8.3, zstd 1.5.7, bzip2 1.0.8, openssl 3.5.9 | R/pub | [Migration](/package/libarchive/3.8.9/migration) |
| `libarchive` | `3.8.10` | 🟡 | archive | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib 1.3.2, xz 5.8.4, zstd 1.5.7, bzip2 1.0.8, openssl 3.5.9 | R/pub | [Migration](/package/libarchive/3.8.10/migration) |
| `openssl` | `3.5.8` → `3.5.9` (retained) | 🟢 | security | Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives. | zlib 1.3.2, brotli 1.2.0, zstd 1.5.7 | R/pub | [Migration](/package/openssl/3.5.8/migration) |
| `openssl` | `3.5.9` | 🟢 | security | Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives. | zlib 1.3.2, brotli 1.2.0, zstd 1.5.7 | R/pub | [Migration](/package/openssl/3.5.9/migration) |
| `curl` | `8.21.0` → `8.23.0` (retained) | 🟢 | network | URL transfer library and command-line client supporting HTTP(S) and numerous network protocols. | openssl 3.5.8, zlib 1.3.2 | R/pub | [Migration](/package/curl/8.21.0/migration) |
| `curl` | `8.23.0` | 🟢 | network | URL transfer library and command-line client supporting HTTP(S) and numerous network protocols. | openssl 3.5.9, zlib 1.3.2 | R/pub | [Migration](/package/curl/8.23.0/migration) |
| `boost` | `1.92.0` | 🟢 | foundation | Comprehensive collection of portable C++ libraries covering containers, algorithms, networking, concurrency, and more. | openssl 3.5.9, zlib 1.3.2 | R/pub, D/pkg | [Migration](/package/boost/1.92.0/migration) |
| `nlohmann-json` | `3.12.0` | 🟢 | data | Modern C++ JSON library offering intuitive STL-like parsing, serialization, and data access. | — | R/pub | [Migration](/package/nlohmann-json/3.12.0/migration) |
| `ace` | `8.0.6` | 🟢 | middleware | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. | openssl 3.5.9, xerces-c 3.3.0, zlib 1.3.2 | — | [Migration](/package/ace/8.0.6/migration) |
| `tao` | `4.0.6` | 🟢 | middleware | CORBA object request broker and middleware services built on top of ACE. | extension: ace 8.0.6 | R/pkg | [Migration](/package/tao/4.0.6/migration) |
| `bzip2` | `1.0.8` | 🟢 | compression | Lossless block-sorting compression library implementing the bzip2 format. | — | R/pub, D/pkg | [Migration](/package/bzip2/1.0.8/migration) |
| `glew` | `2.3.1` | 🟡 | graphics | OpenGL Extension Wrangler Library for discovering and loading OpenGL extensions and entry points. | opengl 26.2.1 | R/pub | [Migration](/package/glew/2.3.1/migration) |
| `opengl` | `26.2.1` | 🟡 | graphics | Mesa-based OpenGL runtime and development package for hardware-accelerated graphics on Windows. | — | R/pub | [Migration](/package/opengl/26.2.1/migration) |
| `raylib` | `6.0` | 🟢 | graphics | Simple cross-platform C library for graphics, audio, input, and game and application development. | opengl 26.2.1 | R/pub | [Migration](/package/raylib/6.0/migration) |
| `sdl2` | `2.32.10` | 🟢 | graphics | Cross-platform multimedia library for windows, input, audio, rendering, and platform integration. | — | R/pub | [Migration](/package/sdl2/2.32.10/migration) |
| `sqlite` | `3.53.4` | 🟢 | database | Embedded relational database engine providing a self-contained SQL database in a single library. | — | R/pub | [Migration](/package/sqlite/3.53.4/migration) |
| `libpq` | `18.6` | 🟢 | database | Official PostgreSQL C client library and wire-protocol access API. | openssl 3.5.9, zlib 1.3.2 | R/pub | [Migration](/package/libpq/18.6/migration) |
| `libpqxx` | `8.0.2` | 🟢 | database | Modern C++ client API for PostgreSQL built on the official libpq C client library. | extension: libpq 18.6 | R/pub | [Migration](/package/libpqxx/8.0.2/migration) |
| `xerces-c` | `3.3.0` | 🟢 | data | Apache C++ XML parser supporting DOM, SAX, validation, and XML Schema. | — | R/pub | [Migration](/package/xerces-c/3.3.0/migration) |
| `soil2` | `1.3.0` | 🟢 | graphics | Small image-loading helper library for OpenGL textures based on stb image decoders. | opengl 26.2.1 | R/pub | [Migration](/package/soil2/1.3.0/migration) |
| `vtk` | `9.6.2` | 🟢 | graphics | Visualization Toolkit for 3D computer graphics, image processing, scientific visualization, and data analysis. | — | R/pub | [Migration](/package/vtk/9.6.2/migration) |
| `opencv` | `5.0.0` | 🟡 | graphics | Computer vision and image-processing library with algorithms for vision, geometry, filtering, and machine learning. | zlib 1.3.2 | R/pub | [Migration](/package/opencv/5.0.0/migration) |
| `cmark-gfm` | `0.29.0.gfm.13` | 🟢 | documentation | CommonMark parser with GitHub Flavored Markdown extensions. | — | R/pkg | [Migration](/package/cmark-gfm/0.29.0.gfm.13/migration) |
| `googletest` | `1.17.0` | 🟢 | testing | C++ testing framework providing GoogleTest and GoogleMock for unit tests and mocks. | — | R/pkg | [Migration](/package/googletest/1.17.0/migration) |
| `catch2` | `3.16.0` | 🟢 | testing | Modern C++ testing framework for unit tests, assertions, and test discovery. | — | R/pkg, D/pkg | [Migration](/package/catch2/3.16.0/migration) |
| `bitmapplusplus` | `1.1.1` | 🟢 | graphics | Header-only C++ bitmap image library for loading, manipulating, and saving BMP data. | — | R/pkg, D/pkg | [Migration](/package/bitmapplusplus/1.1.1/migration) |
| `libjpeg-turbo` | `3.2.0` | 🟡 | image | High-performance JPEG codec using SIMD acceleration while remaining API-compatible with libjpeg. | — | R/pub | [Migration](/package/libjpeg-turbo/3.2.0/migration) |
| `libpng` | `1.6.58` | 🟢 | image | Reference PNG library for reading, writing, and manipulating Portable Network Graphics images. | zlib 1.3.2 | R/pub | [Migration](/package/libpng/1.6.58/migration) |
| `harfbuzz` | `14.4.0` | 🟡 | text | Text shaping engine that converts Unicode text into positioned glyph sequences for complex scripts. | — | R/pub, D/pkg | [Migration](/package/harfbuzz/14.4.0/migration) |
| `freetype` | `2.14.3` | 🟡 | text | Font rendering library for loading, rasterizing, and querying scalable and bitmap font formats. | zlib 1.3.2, bzip2 1.0.8, brotli 1.2.0, libpng 1.6.58, harfbuzz 14.4.0 | R/pub, D/pkg | [Migration](/package/freetype/2.14.3/migration) |
| `libtiff` | `4.7.2` | 🟢 | image | TIFF image library for reading, writing, and manipulating Tagged Image File Format images. | zlib 1.3.2, libjpeg-turbo 3.2.0 | R/pub | [Migration](/package/libtiff/4.7.2/migration) |
| `skia` | `144` → `153` (retained) | 🟡 | graphics | Pinned Skia 144 shared CPU/raster package for PDFium 2025.11.19. Superseded for general consumers by Skia 153. | zlib 1.3.2, libjpeg-turbo 3.2.0, libpng 1.6.58, freetype 2.14.3, opengl 26.2.1 | R/pkg, D/pkg | [Migration](/package/skia/144/migration) |
| `skia` | `153` | 🟡 | graphics | 2D graphics engine providing raster, vector, text, image, and GPU-accelerated rendering capabilities. | opengl 26.2.1, zlib 1.3.2, brotli 1.2.0, libjpeg-turbo 3.2.0, libpng 1.6.58, harfbuzz 14.4.0, freetype 2.14.3 | R/pkg, D/pkg | [Migration](/package/skia/153/migration) |
| `graphite2` | `1.3.15` | 🟡 | text | Smart-font rendering engine for Graphite fonts and complex text shaping. | — | — | [Migration](/package/graphite2/1.3.15/migration) |
| `expat` | `2.8.4` → `2.9.0` (retained) | 🟢 | data | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | R/pub | [Migration](/package/expat/2.8.4/migration) |
| `expat` | `2.9.0` | 🟢 | data | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | R/pub | [Migration](/package/expat/2.9.0/migration) |
| `teckit` | `2.5.13` | 🟡 | text | Text encoding conversion toolkit with runtime engine, mapping compiler, and Unicode conversion utilities. | zlib 1.3.2, expat 2.9.0 | — | [Migration](/package/teckit/2.5.13/migration) |
| `icu` | `78.3` | 🟡 | text | Unicode and globalization library providing text, locale, normalization, collation, and conversion services. | — | — | [Migration](/package/icu/78.3/migration) |
| `libxml2` | `2.15.3` | 🟢 | data | C XML toolkit providing parsing, XPath, XML Schema, Relax NG, XInclude and serialization support. | zlib 1.3.2 | R/pkg | [Migration](/package/libxml2/2.15.3/migration) |
| `qpdf` | `12.4.1` | 🟢 | documentation | PDF structural transformation library and command-line toolkit for inspecting, rewriting, encrypting and validating PDF files. | zlib 1.3.2, libjpeg-turbo 3.2.0 | R/pkg | [Migration](/package/qpdf/12.4.1/migration) |
| `podofo` | `1.1.1` | 🟡 | documentation | C++ PDF parsing, creation and modification library with forms, annotations, signing and incremental-update support. | zlib 1.3.2, openssl 3.5.9, freetype 2.14.3, libxml2 2.15.3, libjpeg-turbo 3.2.0, libpng 1.6.58, libtiff 4.7.2 | R/pkg | [Migration](/package/podofo/1.1.1/migration) |
| `win-iconv` | `0.0.8` | 🟢 | text | Small Windows iconv implementation using the Win32 character conversion APIs. | — | R/pkg | [Migration](/package/win-iconv/0.0.8/migration) |
| `poppler` | `26.09.0` | 🔴 | documentation | PDF rendering and parsing library derived from Xpdf and used by document-processing toolchains. | zlib 1.3.2, freetype 2.14.3, libjpeg-turbo 3.2.0, libpng 1.6.58, libtiff 4.7.2, win-iconv 0.0.8 | — | [Migration](/package/poppler/26.09.0/migration) |
| `openxlsx` | `0.5.1` | 🟢 | data | C++ library for reading and writing Office Open XML XLSX workbooks. | pugixml 1.16, libzip 1.11.4, boost 1.92.0 | R/pub | [Migration](/package/openxlsx/0.5.1/migration) |
| `fast_float` | `8.2.3` | 🟡 | data | High-performance C++ floating-point parser | — | R/pkg | [Migration](/package/fast_float/8.2.3/migration) |
| `abseil` | `20260817.0` | 🟡 | core | Google Abseil C++ utilities and containers | — | R/pkg | [Migration](/package/abseil/20260817.0/migration) |
| `lcms2` | `2.19.1` | 🟡 | graphics | LittleCMS 2 ICC color-management engine (GPL plugins excluded) | — | R/pkg | [Migration](/package/lcms2/2.19.1/migration) |
| `openjpeg` | `2.5.4` | 🟡 | graphics | OpenJPEG JPEG 2000 encoder and decoder library | — | R/pkg | [Migration](/package/openjpeg/2.5.4/migration) |
| `simdutf` | `9.3.0` | 🟡 | text | SIMD accelerated Unicode and Base64 processing | — | R/pkg | [Migration](/package/simdutf/9.3.0/migration) |
| `agg` | `2.6` | 🟡 | graphics | AGG core rasterization without GPC | — | R/pkg | [Migration](/package/agg/2.6/migration) |
| `pdfium` | `2025.11.19` | 🟡 | documentation | Pinned PDFium GN/BCC64X integration; validation pending | abseil 20260817.0, skia 144, zlib 1.3.2, libjpeg-turbo 3.2.0, libpng 1.6.58, freetype 2.14.3, icu 78.3, lcms2 2.19.1, openjpeg 2.5.4 | R/pkg, D/pkg | [Migration](/package/pdfium/2025.11.19/migration) |

## Build and acceptance evidence (not the inventory)

**Known native results reported for the recent PDFium consumer sequence:**
the PDFium loader consumers passed **Release and Debug**, and FreeType passed
both configurations after correcting the external-Abseil and shared-runtime
dependency closure. A preceding full-run summary reported **56/58** with
BZip2 Debug and HarfBuzz Debug consumer *CMake-configure* failures; their
package-scope configuration paths were subsequently corrected in the Admin
contract. A post-correction **58/58 run has not been supplied as evidence**
here. BuildEngine's current machine state and per-library logs take
precedence over all such historical snapshots.

**Not separately verified here:** the fresh installed package/SBOM refresh
after the retired Chromium Abseil DLL exclusion, and the standalone
PDFium+SDL2 and PDFium+VCL GUI applications. A successful PDFium loader
smoke does not establish that either interactive viewer passed.

**DLL naming and ownership:** the managed BZip2, HarfBuzz and FreeType
DLLs/import archives follow `lib` prefix plus `d` Debug postfix;
exceptions such as `pdfium.dll`, `skia144.dll` and
`libabseil_dll.dll` are deliberate named contracts, not aliases.
The SDL2 test excludes the retired
`third_party_abseil-cpp_absl.dll` during runtime staging.
The [PDFium migration dossier](/package/pdfium/2025.11.19/migration)
and [library contract](build-libraries.md) describe the gates in detail.

## PostgreSQL client stack

The database branch now includes both embedded and client/server access:

- **SQLite 3.53.4** remains the embedded self-contained SQL engine.
- **libpq 18.6** is the official PostgreSQL C client package and consumes managed OpenSSL and zlib.
- **libpqxx 8.0.2** is the modern C++ layer and is modeled as a package extension of libpq rather than as a physical source/install extension.

The PostgreSQL work produced reusable BCC64X findings: Windows shared-library import/export logic must not assume `_MSC_VER`; `__MINGW64__` must not be treated as proof of a MinGW compiler; GNU hot/cold attributes may need BCC64X-specific declaration spelling; and installed consumer smokes are the authoritative package-acceptance evidence.

## PDF and XML processing stack

The document-processing branch now has four complementary layers:

- **libxml2 2.15.3** provides the XML parser, XPath, XML Schema and Relax NG foundation for extracted E-invoice payloads.
- **QPDF 12.4.1** provides low-level PDF object/structure inspection and deterministic rewrite capabilities.
- **PoDoFo 1.1.1** provides the higher-level C++ PDF modification layer, including forms, annotations, signing and incremental updates.
- **Poppler 26.09.0** provides PDF reading/rendering and its C++ API is enabled.

The current contracts are intentionally separate. No single PDF library is treated as a universal abstraction. The first BuildEngine goal is to prove each installed package independently with BCC64X, then use BuildEngine-Tests to determine the practical responsibility boundary through real E-invoice, form-discovery and fill/save/reopen scenarios.

win-iconv and Poppler now have target-machine build/package evidence, including Poppler with ENABLE_CPP=ON. libxml2 and QPDF have progressed through their active integration/debugging paths; their exact final test status remains tied to current contract evidence. PoDoFo reached build/test/install/metadata/publish/doxygen PASS before the final runtime-policy correction; its authoritative final gate is the clean rerun with dynamic-only BCC64X runtime plus the installed XMP DLL consumer.

Licensing is also part of the architecture. QPDF is Apache-2.0. PoDoFo's library offers MPL-2.0 or LGPL-2.0-or-later, while its command-line tools are GPL and are therefore disabled in the first library-focused profile. Poppler is GPL and must not become an accidental mandatory proprietary in-process dependency without a deliberate distribution decision. See [Open-Source License Overview](licenses.md) and [PDF and E-Invoice Processing Roadmap](pdf-processing.md).

## Archive and compression stack

### zlib, Brotli, Zstandard, XZ and bzip2

These libraries form the reusable compression foundation. Their versions are explicit dependencies of downstream packages rather than libraries discovered from an arbitrary machine installation. This matters especially for OpenSSL, libzip, libarchive, FreeType and Skia, because package metadata and SBOM relationships must describe the exact producer that was used.

BZip2 1.0.8 is now also an explicit libarchive dependency. Release uses `bzip2.lib`, Debug uses `bzip2d.lib`. This is also the source for rebuilding BuildEngine's private libarchive runtime so `.tar.bz2`, `.tbz2` and `.tbz` can be handled in-process.

### libzip 1.11.4

libzip consumes the managed zlib, XZ and Zstandard packages. The package contains the shared library and the upstream command-line tools. BuildEngine runs functional checks for Zstandard, XZ and AES-256 archive operations.

Upstream installs some command-line tools to a literal `bin` location instead of respecting the configuration-specific BuildEngine bindir. BuildEngine keeps the upstream package metadata intact and additionally copies the actual tools into the variant-specific package layout.

The upstream documentation target is deliberately not part of the current build contract because it writes common generated files into the shared source tree and therefore conflicts with parallel Release/Debug builds. This is a build-system/output-layout constraint, not a reason to serialize the entire BuildEngine graph.

### libarchive 3.8.10 (3.8.9 retained)

libarchive consumes zlib, XZ, Zstandard, bzip2 and OpenSSL and enables the native tar, cpio, cat and unzip tools. BZip2 is enabled explicitly and bound to the managed `bzip2 1.0.8` package rather than to an accidental host installation. CNG and Windows XMLLite are enabled; optional backends not represented by explicit BuildEngine dependencies are disabled.

The version-bound `bcc64x-modern-borland-compat.patch` separates modern Clang-based BCC64X behavior from legacy compiler branches guarded only by `__BORLANDC__`. It covers inline/integer/literal handling, Windows `lseek` and `mbstate_t` handling, open-mode signatures and related tests. It also incorporates the relevant upstream Windows/Clang `filter_fork_windows.c` correction and resolves a BCC64X `ftruncate` macro collision without weakening Debug `-Werror`.

## Security and network stack

### OpenSSL 3.5.9 (3.5.8 retained)

OpenSSL uses a native BCC64X Configure platform named `BC-64X`. The tool contract supplies BCC64X, `llvm-ar`, `llvm-ranlib`, BCC64X windres and NASM. The current reproducible baseline is shared and uses `no-asm`; zlib, Brotli and Zstandard are explicit package dependencies.

The current version-bound repairs include:

- linker-response path normalization for BCC64X/LLD;
- normalization of external compression-library arguments written to response files;
- a QUIC/Windows header compatibility repair for the modern Clang-based Embarcadero compiler;
- a dedicated BCC64X OpenSSL platform using PDB rather than the legacy C++Builder TDS debug-sidecar model.

Brotli's BCC64X producer names its import libraries `libbrotli*.lib`, while OpenSSL's Windows integration expects unprefixed names. BuildEngine creates private consumer-side aliases inside the OpenSSL build tree; producer artifacts are not renamed.

### curl 8.23.0 (8.21.0 retained)

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

## Text, Unicode and document-processing stack

### Graphite2 1.3.15

Graphite2 remains an upstream-CMake build. The important integration work is concentrated in the upstream test path rather than in a replacement build system. The current exact-version patch set covers language-tag/reference expectations, text-render advance diagnostics, Windows line endings, UTF-8 literals, managed FontTools discovery and the BCC64X Padauk3 reference case.

These patches are intentionally version-bound under `admin/patches/graphite2/1.3.15`. They are not described as generic Graphite2 fixes.

The resulting current evidence is strong: both Release and Debug completed the complete configured suite with 91/91 tests. This is a useful example where preserving the tests was more valuable than disabling inconvenient cases.

### Expat 2.9.0 (2.8.4 retained)

Expat is classified as **data**, because its primary role is XML parsing. Its use by TECkit/SFconv is a dependency relationship and does not make Expat a text-processing library.

Expat uses its upstream CMake build, builds a shared parser package plus `xmlwf`, runs upstream CTest tests and installs its own CMake package metadata.

One upstream test-system assumption required correction: Expat treated every non-MSVC compiler as a POSIX-shell case and routed tests through `bash run.sh`. BCC64X is a native Windows Clang toolchain. The version-bound `native-windows-tests.patch` changes this decision to Windows-vs-non-Windows, so native Windows test executables run directly.

This is a build/test-system patch, not an Expat parser-source compatibility patch.

### TECkit 2.5.13

TECkit is a deliberate exception to the default preference for using the upstream build system directly.

The maintained upstream build contract is Autotools and the upstream Windows release script cross-builds with MinGW. Neither path is the evidence target of this project. The TECkit source itself has a small, explicit target inventory in its `Makefile.am` files, so BuildEngine-Admin provides a native CMake adapter that expresses the same core products for CMake/Ninja/BCC64X:

- `TECkit`;
- `TECkit_Compiler`;
- `teckit_compile`;
- `txtconv`;
- `sfconv`.

The adapter is therefore a **downstream build description**, not a claim that TECkit upstream provides CMake support.

zlib 1.3.2 and Expat 2.8.4 are normal BuildEngine package dependencies. The FSM releases TECkit Build only after both dependency Install states have completed; TECkit then consumes their installed package metadata from `InstallRoot`.

The unchanged upstream `test/dotests.pl` regression suite is retained and executed with managed Strawberry Perl. This preserves upstream behavioral evidence despite the different native build description.

### ICU4C 78.3

ICU demonstrates a different integration class. The Windows VCXPROJ files are not executed with MSBuild and do not select MSVC. They are used read-only as source/resource inventories by BuildEngine's ProjectImport layer.

The generated neutral CMake project is then built by Ninja/BCC64X. The current bootstrap deliberately covers:

```text
stubdata -> common -> i18n
```

This was chosen over introducing NMAKE, MSYS, `runConfigureICU`, `cygpath` or Visual Studio Build Tools into the BCC64X proof. The first bootstrap stage does not yet claim full ICU data-generator/test coverage; that boundary remains explicit.

### Poppler 26.09.0

Poppler returns to the ordinary upstream-CMake path but uses an intentionally constrained package profile. Managed zlib, FreeType, libjpeg-turbo, libpng and libtiff are exact package dependencies.

The Win32 backend uses Microsoft-style Windows SDK headers. BCC64X identifies the x64 target differently from the assumptions in those headers, so the current contract supplies a local `_AMD64_` bridge and `NOMINMAX` through Poppler-specific flags instead of changing the generic BCC64X toolchain.

Several optional frontends/backends and utilities are disabled in this proof. Upstream release tarballs also do not contain the separate `poppler/test-data` repository, so the upstream test-data integration is explicitly deferred until that repository is pinned as its own reproducible participant. Tests are not claimed as passed merely because the core package builds.

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

Every non-default integration decision must remain visible here or in the matching detailed findings section. The documentation classifies it as one of: upstream build unchanged, downstream build adapter, exact-version source/build-system patch, packaging/export repair, library-local compiler policy, or generic toolchain/runtime correction.

Whenever `admin/build-libraries.xml` changes materially, maintainers must update the applicable documentation in the same work unit:

- add/remove/upgrade a library -> update the inventory and dependency information here;
- add/change a library purpose description -> update `metadata/@description` and the inventory description together;
- add/remove a patch -> update the relevant integration notes and identify whether it is source, packaging, compiler-policy, or generic-toolchain work;
- change important feature/test/build settings -> update the library's note;
- change dependency edges -> update both the inventory and dependency overview;
- change an intentional shared producer layout such as ACE/TAO -> document both the logical ownership and the physical output relationship;
- record deeper diagnostic evidence -> update the internal findings document under `docs/` as well.

The XML contract remains authoritative when documentation and executable configuration ever disagree; such a disagreement is a documentation defect to be corrected, not a second source of truth.

### BCC64X dynamic-runtime rule

The PoDoFo investigation established a project-wide invariant: **all BCC64X Windows executables, DLLs and modules use the dynamic runtime (`-tR`)**. Static BCC64X runtime linkage is forbidden.

This is separate from whether a third-party library is packaged as a static or shared library. Public C++ APIs may move strings, exceptions, STL containers or owning pointers across a DLL boundary. Separate static C++ runtime/allocator domains can then corrupt ownership or fail during destruction.

The generic CMake rules now encode and assert the dynamic-only policy so it cannot silently regress.

## Per-package migration and consumer records

Each logical library version has an English integration dossier in `admin/docs/packages/<library>/<version>/migration.md`, visible through the package's `/package/<library>/<version>/migration` route. The files explain the selected product scope, explicitly disabled features, source/build approach, managed dependencies, version-bound patch reasons, packaging repairs, consumer ABI/runtime requirements and test evidence. The XML remains the executable authority; the dossier is the architectural rationale. An unverified build outcome must not be presented as a passed test.
