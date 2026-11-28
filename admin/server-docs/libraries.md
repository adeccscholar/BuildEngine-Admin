# BuildEngine Third-Party Libraries

[TOC|Content]

**Current library catalog (28 November 2026): 116 distinct libraries and 152 versioned integrations, including retained superseded versions.** Source: `admin/build-libraries.xml` (schema 17). The version totals cover all retained historical versions; a successor is not counted as a new distinct library.

The catalog begins with **2 packages on 24 August**, grows to **44 on 25 September**, and reaches **81 distinct library IDs across 116 version records on 17 November**. The original August compiler feasibility study preceded the first two-entry library catalog. Details appear in the [project story](/manual/story.md) and [dated news](/news/).

The catalog lists upstream versions built and managed with C++Builder 13 / BCC64X, their direct dependencies and extension relationships. The per-version guides contain build configuration, porting notes, migration information and transitive Mermaid diagrams. The native BCC64X build also maintains integrated library smokes and dedicated, larger consumer tests in BuildEngine-Tests.

[Library status](/libraries/) · [Version-specific documentation](/library-docs/) · [News](/news/) · [Skia dependencies](/manual/skia-externalization.md)

**New game-development packages (17 November 2026):** [SDL3 3.4.20](/package/sdl3/3.4.20/migration) and [Box2D 3.2.0](/package/box2d/3.2.0/migration), both registered for BCC64X Debug/Release DLL builds and pending native validation. SDL2 is unchanged.

**Earlier registrations:** `xmp-toolkit@7.0.0`, `dng-sdk@1.7.1.2802`, `libgav1@0.20.0`, `libwebp@1.6.0` and `piex@0.27`. Skia 153 declares PIE-X and libgav1 as managed dependencies. The external AVIF package consumes libgav1 0.20.0 as its decoder and libaom 3.15.3 as its encoder; libyuv r1924 remains a managed transitive dependency through AVIF_LIBYUV=SYSTEM. The Skia and libgav1 Debug/Release builds require BCC64X verification. SPIRV-Cross now builds a native C++ DLL instead of the C-only DLL, and Skia 153 uses that package for Direct3D HLSL translation. Build and ABI compatibility remain unverified until BCC64X tests. The prior 109/109 FSM result predates these additions.

## Native C/C++ UI and plotting additions (18 November 2026)

Four new versioned integrations are registered in `admin/build-libraries.xml`: [Dear ImGui 1.93.0](/package/imgui/1.93.0/migration) (shared GUI core), [ImPlot 1.1](/package/implot/1.1/migration) (shared plotting extension), [raygui 5.0](/package/raygui/5.0/migration) (header-only / interface extension of raylib 6.2) and [Matplot++ 1.2.2](/package/matplotplusplus/1.2.2/migration) (mathematical plotting).

**Current declared catalog: 85 unique library IDs / 120 versioned library records.** The new packages are **[not verified]** until native BCC64X compilation and consumer smokes complete. Extension relationships and the distinction between `depends-on` and `extends` appear in [the engineering news](/news/2026-11-18/) with two Mermaid graphs. A header-only interface package is installed and smoke-tested without producing a DLL.

## Microsoft graphics, Windows runtime and shader integration: 28 November 2026

The active XML defines **116 distinct library IDs / 152 retained version records**. The previous 17 November and 22 November totals below remain useful historical milestones, not the current top-line inventory. The project operator has reported successful completion on both the developer workstation and the reduced Windows Server 2025; this is operator-reported evidence. A reproducible unchanged second CURRENT run and GPU showcase acceptance are separate claims.

| Managed integration | Version | Primary use | Guide |
| --- | --- | --- | --- |
| DirectX-Headers | 1.619.1 | D3D12 and DXCore APIs | [Guide](/package/directx-headers/1.619.1/migration) |
| Detours | 4.0.1 | Opt-in native interception and diagnostics | [Guide](/package/detours/4.0.1/migration) |
| DirectXMath | 3.21 | SIMD geometry and matrix mathematics | [Guide](/package/directxmath/3.21/migration) |
| DirectXMesh | 1.7.2 | Mesh normals, adjacency and optimization | [Guide](/package/directxmesh/1.7.2/migration) |
| DirectXTex | 2.1.1 | Textures, format conversion and mipmaps | [Guide](/package/directxtex/2.1.1/migration) |
| WinMD Reader | 0f1eae3b | Windows Runtime metadata | [Guide](/package/winmd/0f1eae3b/migration) |
| C++/WinRT | 3.0.260818.1 | C++ metadata projections | [Guide](/package/cppwinrt/3.0.260818.1/migration) |
| DXC | 1.9.2609 | Native HLSL compiler, DXIL and SPIR-V tools | [Guide](/package/dxc/1.9.2609/migration) |
| DirectXTK12 | 1.6.6 | Direct3D 12 effects, geometry, input and audio | [Guide](/package/directxtk12/1.6.6/migration) |
| DirectStorage GDeflate reference | 9352da00 | GDeflate codec, **not** the proprietary DirectStorage runtime | [Guide](/package/directstorage-gdeflate/9352da00/migration) |
| NVIDIA libdeflate GDeflate fork | 1.8-8ba9502f | Native GDeflate codec dependency | [Guide](/package/libdeflate-gdeflate/1.8-8ba9502f/migration) |
| WIL | 1.0.260126.7 | Native Windows resource and COM lifetime | [Guide](/package/wil/1.0.260126.7/migration) |

The latest versioned inventory and direct dependencies remain authoritative in `admin/build-libraries.xml`. This summary does not replace the category tables below. OpenGL acceleration must be measured using the *actual* Mesa renderer/adapter rather than inferred from library installation. [Engineering retrospective](/news/2026-11-28/) · [Showcases](/showcases/).

## Complete versioned library inventory (authoritative XML snapshot, 28 November 2026)

The 152 versioned `<library>` entries represent **116 unique library IDs**, excluding the separate unversioned `ace-tao` grouping contract. This list is derived from the current `admin/build-libraries.xml` (schema 17). Migration links are included only when a corresponding versioned guide exists. Build status and GPU performance are separate evidence.

### archive (5 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `libzip` | `1.11.4` (superseded by `1.12.1`) | C library for reading, creating, and modifying ZIP archives. | zlib@1.4, xz@5.8.4, zstd@1.5.7 | green | [Migration](/package/libzip/1.11.4/migration) |
| `libzip` | `1.12.1` | C library for reading, creating, and modifying ZIP archives. | zlib@1.4, xz@5.8.4, zstd@1.5.7 | green | [Migration](/package/libzip/1.12.1/migration) |
| `libarchive` | `3.8.9` (superseded by `3.8.10`) | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib@1.3.2, xz@5.8.3, zstd@1.5.7, bzip2@1.0.8, openssl@3.5.9 | yellow | [Migration](/package/libarchive/3.8.9/migration) |
| `libarchive` | `3.8.10` (superseded by `3.8.11`) | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib@1.3.2, xz@5.8.4, zstd@1.5.7, bzip2@1.0.8, openssl@3.5.9 | yellow | [Migration](/package/libarchive/3.8.10/migration) |
| `libarchive` | `3.8.11` | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib@1.4, xz@5.8.4, zstd@1.5.7, lz4@1.11.0, bzip2@1.0.8, openssl@3.5.9 | yellow | [Migration](/package/libarchive/3.8.11/migration) |

### compression (13 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `zlib` | `1.3.2` (superseded by `1.4`) | General-purpose DEFLATE compression library used for gzip-compatible data and streams. | — | green | [Migration](/package/zlib/1.3.2/migration) |
| `zlib` | `1.4` | General-purpose DEFLATE compression library used for gzip-compatible data and streams. | — | green | [Migration](/package/zlib/1.4/migration) |
| `libdeflate` | `1.26` | Fast native DEFLATE, zlib and gzip compression/decompression; distinct from zlib streaming interfaces. | zlib@1.4 | green | [Migration](/package/libdeflate/1.26/migration) |
| `lz4` | `1.10.0` (superseded by `1.11.0`) | Fast lossless LZ4 block and frame compression with BCC64X shared DLL. | — | green | [Migration](/package/lz4/1.10.0/migration) |
| `lz4` | `1.11.0` | Fast lossless LZ4 block and frame compression with BCC64X shared DLL. | — | green | [Migration](/package/lz4/1.11.0/migration) |
| `brotli` | `1.2.0` (superseded by `1.2.1`) | General-purpose lossless compression library providing Brotli encoder and decoder support. | — | green | [Migration](/package/brotli/1.2.0/migration) |
| `brotli` | `1.2.1` | General-purpose lossless compression library providing Brotli encoder and decoder support. | — | green | [Migration](/package/brotli/1.2.1/migration) |
| `zstd` | `1.5.7` | High-performance lossless compression library implementing the Zstandard format. | — | green | [Migration](/package/zstd/1.5.7/migration) |
| `xz` | `5.8.3` (superseded by `5.8.4`) | Data compression library and tools providing the LZMA and LZMA2 algorithms through liblzma. | — | yellow | [Migration](/package/xz/5.8.3/migration) |
| `xz` | `5.8.4` | Data compression library and tools providing the LZMA and LZMA2 algorithms through liblzma. | — | yellow | [Migration](/package/xz/5.8.4/migration) |
| `bzip2` | `1.0.8` | Lossless block-sorting compression library implementing the bzip2 format. | — | green | [Migration](/package/bzip2/1.0.8/migration) |
| `directstorage-gdeflate` | `9352da00` | Source-built GDeflate reference compressor and decompressor from Microsoft DirectStorage samples. This is not the closed DirectStorage SDK runtime. | libdeflate-gdeflate@1.8-8ba9502f | yellow | [Migration](/package/directstorage-gdeflate/9352da00/migration) |
| `libdeflate-gdeflate` | `1.8-8ba9502f` | GDeflate-specific libdeflate 1.8 fork from DirectStorage submodule, separate DLL and source ownership. | — | yellow | [Migration](/package/libdeflate-gdeflate/1.8-8ba9502f/migration) |

### concurrency (3 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `onetbb` | `2023.1.0` (superseded by `2023.2.0`) | Task-based C++ parallelism runtime, parallel algorithms and flow graphs. | — | green | [Migration](/package/onetbb/2023.1.0/migration) |
| `onetbb` | `2023.2.0` | Task-based C++ parallelism runtime, parallel algorithms and flow graphs. | — | green | [Migration](/package/onetbb/2023.2.0/migration) |
| `llvm-openmp` | `20.1.7` | Self-built dynamic LLVM libomp runtime, BCC64X compiler consumer validation. | — | green | [Migration](/package/llvm-openmp/20.1.7/migration) |

### core (1 version)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `abseil` | `20260817.0` | Google Abseil C++ utilities and containers | — | yellow | [Migration](/package/abseil/20260817.0/migration) |

### data (12 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `pugixml` | `1.16` | Lightweight C++ XML processing library with DOM-style parsing, modification, and XPath support. | — | green | [Migration](/package/pugixml/1.16/migration) |
| `nlohmann-json` | `3.12.0` | Modern C++ JSON library offering intuitive STL-like parsing, serialization, and data access. | — | green | [Migration](/package/nlohmann-json/3.12.0/migration) |
| `xerces-c` | `3.3.0` | Apache C++ XML parser supporting DOM, SAX, validation, and XML Schema. | — | green | [Migration](/package/xerces-c/3.3.0/migration) |
| `xmp-toolkit` | `7.0.0` | Adobe XMP Core 7.0 and XMP Files 7.0, preserved in the DNG SDK 1.7.1 Build 2802 source distribution; BCC64X dynamic producer with managed Expat and zlib. | expat@2.10.0, zlib@1.4 | yellow | [Migration](/package/xmp-toolkit/7.0.0/migration) |
| `expat` | `2.8.4` (superseded by `2.9.0`) | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | green | [Migration](/package/expat/2.8.4/migration) |
| `expat` | `2.9.0` (superseded by `2.10.0`) | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | green | [Migration](/package/expat/2.9.0/migration) |
| `expat` | `2.10.0` | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | green | [Migration](/package/expat/2.10.0/migration) |
| `libxml2` | `2.15.3` (superseded by `2.15.4`) | C XML toolkit providing parsing, XPath, XML Schema, Relax NG, XInclude and serialization support. | zlib@1.4 | green | [Migration](/package/libxml2/2.15.3/migration) |
| `libxml2` | `2.15.4` | C XML toolkit providing parsing, XPath, XML Schema, Relax NG, XInclude and serialization support. | zlib@1.4 | green | [Migration](/package/libxml2/2.15.4/migration) |
| `openxlsx` | `0.5.1` | C++ library for reading and writing Office Open XML XLSX workbooks. | pugixml@1.16, libzip@1.12.1, boost@1.92.0 | green | [Migration](/package/openxlsx/0.5.1/migration) |
| `fast_float` | `8.2.3` (superseded by `8.3.3`) | High-performance C++ floating-point parser | — | yellow | [Migration](/package/fast_float/8.2.3/migration) |
| `fast_float` | `8.3.3` | High-performance C++ floating-point parser | — | yellow | [Migration](/package/fast_float/8.3.3/migration) |

### database (5 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `sqlite` | `3.53.4` (superseded by `3.54.1`) | Embedded relational database engine providing a self-contained SQL database in a single library. | — | green | [Migration](/package/sqlite/3.53.4/migration) |
| `sqlite` | `3.54.1` | Embedded relational database engine providing a self-contained SQL database in a single library. | — | green | [Migration](/package/sqlite/3.54.1/migration) |
| `libpq` | `18.6` | Official PostgreSQL C client library and wire-protocol access API. | openssl@3.5.9, zlib@1.4 | green | [Migration](/package/libpq/18.6/migration) |
| `libpqxx` | `8.0.2` | Modern C++ client API for PostgreSQL built on the official libpq C client library. | libpq@18.6 (extension) | green | [Migration](/package/libpqxx/8.0.2/migration) |
| `mariadb-connector-c` | `3.4.11` | MariaDB native C client as a BCC64X-built shared DLL. | zlib@1.4, zstd@1.5.7 | green | [Migration](/package/mariadb-connector-c/3.4.11/migration) |

### documentation (8 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `cmark-gfm` | `0.29.0.gfm.13` | CommonMark parser with GitHub Flavored Markdown extensions. | — | green | [Migration](/package/cmark-gfm/0.29.0.gfm.13/migration) |
| `qpdf` | `12.4.1` (superseded by `12.4.2`) | PDF structural transformation library and command-line toolkit for inspecting, rewriting, encrypting and validating PDF files. | zlib@1.4, libjpeg-turbo@3.2.0 | green | [Migration](/package/qpdf/12.4.1/migration) |
| `qpdf` | `12.4.2` | PDF structural transformation library and command-line toolkit for inspecting, rewriting, encrypting and validating PDF files. | zlib@1.4, libjpeg-turbo@3.2.1 | green | [Migration](/package/qpdf/12.4.2/migration) |
| `podofo` | `1.1.1` (superseded by `1.1.2`) | C++ PDF parsing, creation and modification library with forms, annotations, signing and incremental-update support. | zlib@1.4, openssl@3.5.9, freetype@2.14.3, libxml2@2.15.3, libjpeg-turbo@3.2.0, libpng@1.6.59, libtiff@4.7.2 | yellow | [Migration](/package/podofo/1.1.1/migration) |
| `podofo` | `1.1.2` | C++ PDF parsing, creation and modification library with forms, annotations, signing and incremental-update support. | zlib@1.4, openssl@3.5.9, freetype@2.15.0, libxml2@2.15.4, libjpeg-turbo@3.2.1, libpng@1.6.59, libtiff@4.7.2 | yellow | [Migration](/package/podofo/1.1.2/migration) |
| `poppler` | `26.09.0` (superseded by `26.11.0`) | PDF rendering and parsing library derived from Xpdf and used by document-processing toolchains. | zlib@1.4, freetype@2.15.0, libjpeg-turbo@3.2.0, libpng@1.6.59, libtiff@4.7.2, win-iconv@0.0.8 | red | [Migration](/package/poppler/26.09.0/migration) |
| `poppler` | `26.11.0` | PDF rendering and parsing library derived from Xpdf and used by document-processing toolchains. | zlib@1.4, freetype@2.15.0, harfbuzz@15.1.0, libjpeg-turbo@3.2.1, libpng@1.6.59, libtiff@4.7.2, win-iconv@0.0.8 | red | [Migration](/package/poppler/26.11.0/migration) |
| `pdfium` | `2025.11.19` | Pinned PDFium GN/BCC64X integration; validation pending | abseil@20260817.0, fast_float@8.3.3, skia@153, zlib@1.4, libjpeg-turbo@3.2.1, libpng@1.6.59, freetype@2.15.0, icu@79.1, lcms2@2.20, openjpeg@2.5.4 | yellow | [Migration](/package/pdfium/2025.11.19/migration) |

### foundation (7 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `boost` | `1.92.0` | Comprehensive collection of portable C++ libraries covering containers, algorithms, networking, concurrency, and more. | openssl@3.5.9, zlib@1.4 | green | [Migration](/package/boost/1.92.0/migration) |
| `wil` | `1.0.260126.7` | Microsoft MIT-licensed header-only Windows COM and RAII library. BCC64X compatibility requires a native consumer smoke before use by OpenCV. | — | yellow | [Migration](/package/wil/1.0.260126.7/migration) |
| `gflags` | `2.3.1` | Native BCC64X shared-library integration of gflags; independent DLL consumer validation required. | — | yellow | [Migration](/package/gflags/2.3.1/migration) |
| `glog` | `0.7.1` | Native BCC64X shared-library integration of glog; independent DLL consumer validation required. | gflags@2.3.1 | yellow | [Migration](/package/glog/0.7.1/migration) |
| `xtl` | `0.8.0` | Header-only C++ building block for the xtensor numerical stack. | doctest@2.4.12 | green | [Migration](/package/xtl/0.8.0/migration) |
| `winmd` | `0f1eae3b` | MIT licensed interface-only Windows metadata reader, pinned dependency for the Microsoft C++/WinRT projection generator. | — | yellow | [Migration](/package/winmd/0f1eae3b/migration) |
| `cppwinrt` | `3.0.260818.1` | Native WinRT source-projection generator with managed winmd metadata reader and original upstream test project. | winmd@0f1eae3b | yellow | [Migration](/package/cppwinrt/3.0.260818.1/migration) |

### graphics (48 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `glew` | `2.3.1` | OpenGL Extension Wrangler Library for discovering and loading OpenGL extensions and entry points. | opengl@26.2.1 | yellow | [Migration](/package/glew/2.3.1/migration) |
| `opengl` | `26.2.1` | Mesa-based OpenGL runtime and development package for hardware-accelerated graphics on Windows. | directx-headers@1.619.1 | yellow | [Migration](/package/opengl/26.2.1/migration) |
| `raylib` | `6.0` (superseded by `6.2`) | Simple cross-platform C library for graphics, audio, input, and game and application development. | opengl@26.2.1 | green | [Migration](/package/raylib/6.0/migration) |
| `raylib` | `6.2` | Simple cross-platform C library for graphics, audio, input, and game and application development. | opengl@26.2.1 | green | [Migration](/package/raylib/6.2/migration) |
| `sdl2` | `2.32.10` | Cross-platform multimedia library for windows, input, audio, rendering, and platform integration. | — | green | [Migration](/package/sdl2/2.32.10/migration) |
| `soil2` | `1.3.0` | Small image-loading helper library for OpenGL textures based on stb image decoders. | opengl@26.2.1 | green | [Migration](/package/soil2/1.3.0/migration) |
| `vtk` | `9.6.2` (superseded by `9.7.2`) | Visualization Toolkit for 3D computer graphics, image processing, scientific visualization, and data analysis. Native oneTBB 2023.2 and LLVM OpenMP 20.1.7 SMP backends configured; consumer regression pending. | onetbb@2023.2.0, llvm-openmp@20.1.7 | green | [Migration](/package/vtk/9.6.2/migration) |
| `vtk` | `9.7.2` | Visualization Toolkit for 3D computer graphics, image processing, scientific visualization, and data analysis. VTK 9.7.2 core, OpenGL2/Win32-WGL graphics and GPU volume rendering; managed OpenGL 26.2.1 imports, oneTBB and OpenMP; BCC64X validation pending. | onetbb@2023.2.0, llvm-openmp@20.1.7, opengl@26.2.1 | green | [Migration](/package/vtk/9.7.2/migration) |
| `opencv` | `5.0.0` (superseded by `5.1.0`) | Computer vision and image-processing library with algorithms for vision, geometry, filtering, and machine learning, Windows DirectShow camera capture (MSMF COM compatibility pending), and image stitching. Native oneTBB 2023.2 parallel backend configured; Release consumer smoke regression pending; LLVM OpenMP 20.1.7 enabled as a build dependency. | onetbb@2023.2.0, llvm-openmp@20.1.7, zlib@1.4 | yellow | [Migration](/package/opencv/5.0.0/migration) |
| `opencv` | `5.1.0` | Computer vision and image-processing library with algorithms for vision, geometry, filtering, and machine learning, Windows DirectShow camera capture (MSMF COM compatibility pending), image stitching, and selected Contrib algorithms (img_hash, optflow, quality, ximgproc). Native oneTBB 2023.2 parallel backend configured; Release consumer smoke regression pending; LLVM OpenMP 20.1.7 enabled as a build dependency. | onetbb@2023.2.0, llvm-openmp@20.1.7, zlib@1.4 | yellow | [Migration](/package/opencv/5.1.0/migration) |
| `bitmapplusplus` | `1.1.1` | Header-only C++ bitmap image library for loading, manipulating, and saving BMP data. | — | green | [Migration](/package/bitmapplusplus/1.1.1/migration) |
| `skia` | `144` (superseded by `153`) | Pinned Skia 144 shared CPU/raster package for PDFium 2025.11.19. Superseded for general consumers by Skia 153. | zlib@1.4, libjpeg-turbo@3.2.0, libpng@1.6.59, freetype@2.14.3, opengl@26.2.1 | yellow | [Migration](/package/skia/144/migration) |
| `piex` | `0.27` | Preview Image Extractor for camera RAW image metadata and embedded thumbnails, built from upstream GYP sources using the BuildEngine CMake adapter. | — | green | [Migration](/package/piex/0.27/migration) |
| `libgav1` | `0.20.0` | Independent AV1 decoder package built from upstream libgav1 0.20.0 with BCC64X and dynamic runtime. | — | green | [Migration](/package/libgav1/0.20.0/migration) |
| `libwebp` | `1.6.0` | WebP image encoding, decoding and muxing with a native BCC64X shared-library build. | — | green | [Migration](/package/libwebp/1.6.0/migration) |
| `dng-sdk` | `1.7.1.2802` | Independent native DNG SDK 1.7.1 Build 2802 producer, sourced from the revision-preserving DNG SDK mirror; shared BCC64X DLL and import library for Skia RAW decoding. | zlib@1.4, libjpeg-turbo@3.2.1, libjxl@0.13.0, xmp-toolkit@7.0.0, expat@2.10.0 | yellow | [Migration](/package/dng-sdk/1.7.1.2802/migration) |
| `skia` | `153` | 2D graphics engine providing raster, vector, text, image, and GPU-accelerated rendering capabilities. | opengl@26.2.1, zlib@1.4, brotli@1.2.1, libjpeg-turbo@3.2.1, libpng@1.6.59, harfbuzz@15.1.0, freetype@2.15.0, icu@79.1, expat@2.10.0, libavif@1.4.2, libgav1@0.20.0, libjxl@0.13.0, dng-sdk@1.7.1.2802, libwebp@1.6.0, piex@0.27, spirv-cross@vulkan-sdk-1.4.363.0, spirv-headers@v1.202609.0, spirv-tools@2026.4, glslang@16.6.0, vulkan-memory-allocator@3.4.0, wuffs@0.3 | yellow | [Migration](/package/skia/153/migration) |
| `lcms2` | `2.19.1` (superseded by `2.20`) | LittleCMS 2 ICC color-management engine (GPL plugins excluded) | — | yellow | [Migration](/package/lcms2/2.19.1/migration) |
| `lcms2` | `2.20` | LittleCMS 2 ICC color-management engine (GPL plugins excluded) | — | yellow | [Migration](/package/lcms2/2.20/migration) |
| `openjpeg` | `2.5.4` | OpenJPEG JPEG 2000 encoder and decoder library | — | yellow | [Migration](/package/openjpeg/2.5.4/migration) |
| `agg` | `2.6` | AGG core rasterization without GPC | — | yellow | [Migration](/package/agg/2.6/migration) |
| `vulkan-headers` | `1.4.368` | Vulkan C and C++ API headers; no runtime DLL | — | yellow | [Migration](/package/vulkan-headers/1.4.368/migration) |
| `vulkan-memory-allocator` | `3.4.0` | Single-header Vulkan GPU memory allocator; downstream code provides implementation | vulkan-headers@1.4.368 | yellow | [Migration](/package/vulkan-memory-allocator/3.4.0/migration) |
| `wuffs` | `0.3` | Stable 0.3 single-file C image and data decoding library; source/header distribution, no DLL | — | green | [Migration](/package/wuffs/0.3/migration) |
| `spirv-headers` | `v1.202609.0` | Pinned SPIR-V grammar and C/C++ headers for shader tools; source snapshot compatible with Skia 153 | — | green | [Migration](/package/spirv-headers/v1.202609.0/migration) |
| `spirv-cross` | `vulkan-sdk-1.4.363.0` | SPIR-V shader reflection and cross-compilation | — | green | [Migration](/package/spirv-cross/vulkan-sdk-1.4.363.0/migration) |
| `glslang` | `16.6.0` | GLSL/HLSL to SPIR-V compiler, validator and libraries | spirv-tools@2026.4 | green | [Migration](/package/glslang/16.6.0/migration) |
| `spirv-tools` | `2026.4` | SPIR-V assembler, validator, optimizer and associated libraries | spirv-headers@v1.202609.0 | green | [Migration](/package/spirv-tools/2026.4/migration) |
| `libjxl` | `0.13.0` | JPEG XL image encoder and decoder with managed Highway, Brotli and LCMS2 dependencies | highway@1.4.0, brotli@1.2.1, lcms2@2.20, libpng@1.6.59 | yellow | [Migration](/package/libjxl/0.13.0/migration) |
| `libyuv` | `r1924` | YUV scaling and color conversion; Chromium M149 pinned revision, JPEG add-on disabled | — | green | [Migration](/package/libyuv/r1924/migration) |
| `libaom` | `3.15.1` (superseded by `3.15.3`) | AV1 encoder and decoder for the independently managed libavif codec | — | green | [Migration](/package/libaom/3.15.1/migration) |
| `libaom` | `3.15.3` | AV1 encoder and decoder for the independently managed libavif codec | — | green | [Migration](/package/libaom/3.15.3/migration) |
| `libavif` | `1.4.2` | AVIF encoding through external libaom, decoding through external libgav1, and conversion through managed libyuv | libaom@3.15.3, libyuv@r1924, libgav1@0.20.0 | green | [Migration](/package/libavif/1.4.2/migration) |
| `sdl3` | `3.4.20` | SDL3 multimedia framework for windows, controller input, audio, GPU, and rendering. | — | green | [Migration](/package/sdl3/3.4.20/migration) |
| `box2d` | `3.2.0` | Deterministic 2D rigid body dynamics, collision detection, and game physics. | — | green | [Migration](/package/box2d/3.2.0/migration) |
| `imgui` | `1.93.0` | Native Dear ImGui core and official SDL2/OpenGL3 platform/renderer backend as separate managed shared libraries. | sdl2@2.32.10, glew@2.3.1, opengl@26.2.1 | green | [Migration](/package/imgui/1.93.0/migration) |
| `implot` | `1.1` | Immediate-mode plotting extension for Dear ImGui, with native 2D graphs, histograms, heatmaps and interactive axes. | imgui@1.93.0 (extension) | green | [Migration](/package/implot/1.1/migration) |
| `raygui` | `5.0` | Header-only C immediate-mode GUI controls extending raylib, with a single application-side implementation translation unit and no raygui DLL. | raylib@6.2 (extension) | green | [Migration](/package/raygui/5.0/migration) |
| `miniaudio` | `0.11.26` | Single-header C audio engine with explicit sound fading; optional DeckKernel accept/reject feedback, compiled inside the consuming project. | — | green | [Migration](/package/miniaudio/0.11.26/migration) |
| `zxing-cpp` | `3.1.1` | Native ZXing-C++ barcode reader and writer DLL with externally managed Zint encoder backend. | zint@2.16.0, libpng@1.6.59, zlib@1.4 | green | [Migration](/package/zxing-cpp/3.1.1/migration) |
| `recastnavigation` | `1.6.0` | Recast and Detour native navigation and pathfinding libraries, with demos and tests disabled. | — | green | [Migration](/package/recastnavigation/1.6.0/migration) |
| `jolt-physics` | `5.7.0` | Jolt CPU physics engine built as native BCC64X shared library with optional backends and samples disabled. | — | green | [Migration](/package/jolt-physics/5.7.0/migration) |
| `assimp` | `6.0.6` | C++ 3D model import library using independently managed zlib, subject to component audit. | zlib@1.4 | yellow | [Migration](/package/assimp/6.0.6/migration) |
| `openal-soft` | `1.25.2` | Windows OpenAL 3D audio engine, native DLL, LGPL-compliant distribution obligations. | — | yellow | [Migration](/package/openal-soft/1.25.2/migration) |
| `directx-headers` | `1.619.1` | Pinned D3D12 API headers, supporting helper libraries and native BCC64X upstream tests; required by Mesa Gallium D3D12. | googletest@1.18.0 | green | [Migration](/package/directx-headers/1.619.1/migration) |
| `directxmesh` | `1.7.2` | Native D3D12 mesh processing DLL, adjacency, normals, optimization and meshconvert. | directxmath@3.21, directx-headers@1.619.1 | yellow | [Migration](/package/directxmesh/1.7.2/migration) |
| `dxc` | `1.9.2609` | Full native HLSL LLVM-based compiler, DXIL toolchain and Windows compiler runtime targeted at BCC64X. | spirv-headers@v1.202609.0, spirv-tools@2026.4 | yellow | [Migration](/package/dxc/1.9.2609/migration) |
| `directxtk12` | `1.6.6` | Native Direct3D 12 rendering utilities, effects, shaders and input support, with managed DXC and DirectX dependencies. | directxmath@3.21, directx-headers@1.619.1, dxc@1.9.2609, spirv-tools@2026.4 | yellow | [Migration](/package/directxtk12/1.6.6/migration) |

### image (11 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `leptonica` | `1.87.0` | Image normalization, binarization, morphology and OCR preprocessing for Tesseract. | zlib@1.4, libpng@1.6.59, libjpeg-turbo@3.2.1, libtiff@4.7.2 | yellow | [Migration](/package/leptonica/1.87.0/migration) |
| `libjpeg-turbo` | `3.2.0` (superseded by `3.2.1`) | High-performance JPEG codec using SIMD acceleration while remaining API-compatible with libjpeg. | — | yellow | [Migration](/package/libjpeg-turbo/3.2.0/migration) |
| `libjpeg-turbo` | `3.2.1` | High-performance JPEG codec using SIMD acceleration while remaining API-compatible with libjpeg. | — | yellow | [Migration](/package/libjpeg-turbo/3.2.1/migration) |
| `libpng` | `1.6.58` (superseded by `1.6.59`) | Reference PNG library for reading, writing, and manipulating Portable Network Graphics images. | zlib@1.3.2 | green | [Migration](/package/libpng/1.6.58/migration) |
| `libpng` | `1.6.59` | Reference PNG library for reading, writing, and manipulating Portable Network Graphics images. | zlib@1.4 | green | [Migration](/package/libpng/1.6.59/migration) |
| `libtiff` | `4.7.2` | TIFF image library for reading, writing, and manipulating Tagged Image File Format images. | zlib@1.4, libjpeg-turbo@3.2.1 | green | [Migration](/package/libtiff/4.7.2/migration) |
| `zint` | `2.16.0` | Native shared barcode generation library and external ZXing-C++ writer backend. | libpng@1.6.59, zlib@1.4 | green | [Migration](/package/zint/2.16.0/migration) |
| `imath` | `3.2.3` | External shared Imath numerical types and math for OpenEXR. | — | yellow | [Migration](/package/imath/3.2.3/migration) |
| `openjph` | `0.33.0` | Open-source high-throughput JPEG 2000 codec independently installed for OpenEXR. | — | yellow | [Migration](/package/openjph/0.33.0/migration) |
| `openexr` | `3.5.4` | Native dynamic HDR image format libraries with external Imath, OpenJPH and libdeflate; no upstream tools or examples. | imath@3.2.3, openjph@0.33.0, libdeflate@1.26, zstd@1.5.7 | yellow | [Migration](/package/openexr/3.5.4/migration) |
| `directxtex` | `2.1.1` | Native DirectX texture DLL, all upstream tools and samples, D3D11/12, codecs, mipmaps, and OpenMP. | directxmath@3.21, directx-headers@1.619.1, openexr@3.5.4, imath@3.2.3, libdeflate@1.26, openjph@0.33.0, zstd@1.5.7, libjpeg-turbo@3.2.1, libpng@1.6.59, zlib@1.4, llvm-openmp@20.1.7 | yellow | [Migration](/package/directxtex/2.1.1/migration) |

### math (11 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `eigen` | `5.0.1` | Header-only C++ matrix, geometry and sparse linear algebra templates. No DLL or import archive. | — | yellow | [Migration](/package/eigen/5.0.1/migration) |
| `highway` | `1.4.0` | Portable cross-platform SIMD operations for C++ | — | green | [Migration](/package/highway/1.4.0/migration) |
| `matplotplusplus` | `1.2.2` | Mathematical native C++ plotting library for 2D and 3D graphs, export and numerical visualization, with an independently verified graphics backend. | — | green | [Migration](/package/matplotplusplus/1.2.2/migration) |
| `hnswlib` | `0.9.0` | Header-only hierarchical navigable small-world nearest-neighbor index; optional DeckKernel artwork feature-vector retrieval. | — | green | [Migration](/package/hnswlib/0.9.0/migration) |
| `libigl` | `2.6.4` | Header-oriented geometry processing and mesh algorithms, with Eigen as a managed direct dependency; copyleft modules require individual license review. | eigen@5.0.1 | yellow | [Migration](/package/libigl/2.6.4/migration) |
| `ceres-solver` | `2.2.0` (superseded by `2.3.0`) | Native C++ nonlinear least-squares optimization library with independently managed dependencies and version-specific solver backends. | eigen@5.0.1, glog@0.7.1, gflags@2.3.1 | yellow | [Migration](/package/ceres-solver/2.2.0/migration) |
| `ceres-solver` | `2.3.0` | Native C++ nonlinear least-squares optimization library with independently managed dependencies and version-specific solver backends. | eigen@5.0.1, abseil@20260817.0 | yellow | [Migration](/package/ceres-solver/2.3.0/migration) |
| `xsimd` | `13.2.0` | Header-only C++ building block for the xtensor numerical stack. | doctest@2.4.12 | green | [Migration](/package/xsimd/13.2.0/migration) |
| `xtensor` | `0.27.1` | Multidimensional arrays, broadcasting and SIMD using independently managed xtl and xsimd. | doctest@2.4.12, nlohmann-json@3.12.0, xtl@0.8.0, xsimd@13.2.0 | green | [Migration](/package/xtensor/0.27.1/migration) |
| `cgal` | `6.2.1` | CGAL geometry kernel evaluation only; GPL algorithms require separate commercial or GPL terms. | boost@1.92.0 | yellow | [Migration](/package/cgal/6.2.1/migration) |
| `directxmath` | `3.21` | Header-only SIMD math and DirectXCollision geometry from the original Microsoft June 2026 source. | — | yellow | [Migration](/package/directxmath/3.21/migration) |

### middleware (4 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `ace` | `8.0.6` (superseded by `8.0.8`) | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. | openssl@3.5.9, xerces-c@3.3.0, zlib@1.4 | green | [Migration](/package/ace/8.0.6/migration) |
| `ace` | `8.0.8` | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. | openssl@3.5.9, xerces-c@3.3.0, zlib@1.4 | green | [Migration](/package/ace/8.0.8/migration) |
| `tao` | `4.0.6` (superseded by `4.0.8`) | CORBA object request broker and middleware services built on top of ACE. | ace@8.0.6 (extension) | green | [Migration](/package/tao/4.0.6/migration) |
| `tao` | `4.0.8` | CORBA object request broker and middleware services built on top of ACE. | ace@8.0.8 (extension) | green | [Migration](/package/tao/4.0.8/migration) |

### network (2 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `curl` | `8.21.0` (superseded by `8.23.0`) | URL transfer library and command-line client supporting HTTP(S) and numerous network protocols. | openssl@3.5.8, zlib@1.3.2 | green | [Migration](/package/curl/8.21.0/migration) |
| `curl` | `8.23.0` | URL transfer library and command-line client supporting HTTP(S) and numerous network protocols. | openssl@3.5.9, zlib@1.4 | green | [Migration](/package/curl/8.23.0/migration) |

### security (2 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `openssl` | `3.5.8` (superseded by `3.5.9`) | Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives. | zlib@1.3.2, brotli@1.2.0, zstd@1.5.7 | green | [Migration](/package/openssl/3.5.8/migration) |
| `openssl` | `3.5.9` | Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives. | zlib@1.4, brotli@1.2.1, zstd@1.5.7 | green | [Migration](/package/openssl/3.5.9/migration) |

### systems (1 version)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `detours` | `4.0.1` | Windows API interception and instrumentation, reusable BCC64X DLL and Vulkan Loader test source. | — | yellow | [Migration](/package/detours/4.0.1/migration) |

### testing (6 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `googletest` | `1.17.0` (superseded by `1.18.0`) | C++ testing framework providing GoogleTest and GoogleMock for unit tests and mocks. | — | green | [Migration](/package/googletest/1.17.0/migration) |
| `googletest` | `1.18.0` | C++ testing framework providing GoogleTest and GoogleMock for unit tests and mocks. | — | green | [Migration](/package/googletest/1.18.0/migration) |
| `google-benchmark` | `1.10.0` | C++ microbenchmark framework for reproducible performance measurements and JSON reports in BuildEngine-Tests. | — | green | [Migration](/package/google-benchmark/1.10.0/migration) |
| `catch2` | `3.16.0` (superseded by `3.16.1`) | Modern C++ testing framework for unit tests, assertions, and test discovery. | — | green | [Migration](/package/catch2/3.16.0/migration) |
| `catch2` | `3.16.1` | Modern C++ testing framework for unit tests, assertions, and test discovery. | — | green | [Migration](/package/catch2/3.16.1/migration) |
| `doctest` | `2.4.12` | Independent MIT-licensed C++ unit test framework for xtl, xsimd and xtensor upstream suites. | — | green | [Migration](/package/doctest/2.4.12/migration) |

### text (13 versions)

| Library | Version | Purpose (XML metadata) | Direct dependencies / extension | License signal | Integration guide |
| --- | --- | --- | --- | --- | --- |
| `tesseract` | `5.5.3` (superseded by `5.5.4`) | Dynamically linked C++ OCR engine; language traineddata is provisioned and licensed separately. | leptonica@1.87.0 | yellow | [Migration](/package/tesseract/5.5.3/migration) |
| `tesseract` | `5.5.4` | Dynamically linked C++ OCR engine; language traineddata is provisioned and licensed separately. | leptonica@1.87.0 | yellow | [Migration](/package/tesseract/5.5.4/migration) |
| `harfbuzz` | `14.4.0` (superseded by `15.1.0`) | Text shaping engine that converts Unicode text into positioned glyph sequences for complex scripts. | — | yellow | [Migration](/package/harfbuzz/14.4.0/migration) |
| `harfbuzz` | `15.1.0` | Text shaping engine that converts Unicode text into positioned glyph sequences for complex scripts. | — | yellow | [Migration](/package/harfbuzz/15.1.0/migration) |
| `freetype` | `2.14.3` (superseded by `2.15.0`) | Font rendering library for loading, rasterizing, and querying scalable and bitmap font formats. | zlib@1.4, bzip2@1.0.8, brotli@1.2.1, libpng@1.6.59, harfbuzz@14.4.0 | yellow | [Migration](/package/freetype/2.14.3/migration) |
| `freetype` | `2.15.0` | Font rendering library for loading, rasterizing, and querying scalable and bitmap font formats. | zlib@1.4, bzip2@1.0.8, brotli@1.2.1, libpng@1.6.59, harfbuzz@15.1.0 | yellow | [Migration](/package/freetype/2.15.0/migration) |
| `graphite2` | `1.3.15` | Smart-font rendering engine for Graphite fonts and complex text shaping. | — | yellow | [Migration](/package/graphite2/1.3.15/migration) |
| `teckit` | `2.5.13` | Text encoding conversion toolkit with runtime engine, mapping compiler, and Unicode conversion utilities. | zlib@1.4, expat@2.10.0 | yellow | [Migration](/package/teckit/2.5.13/migration) |
| `icu` | `78.3` (superseded by `79.1`) | Unicode and globalization library providing text, locale, normalization, collation, and conversion services. | — | yellow | [Migration](/package/icu/78.3/migration) |
| `icu` | `79.1` | Unicode and globalization library providing text, locale, normalization, collation, and conversion services. | — | yellow | [Migration](/package/icu/79.1/migration) |
| `win-iconv` | `0.0.8` | Small Windows iconv implementation using the Win32 character conversion APIs. | — | green | [Migration](/package/win-iconv/0.0.8/migration) |
| `simdutf` | `9.3.0` (superseded by `9.3.3`) | SIMD accelerated Unicode and Base64 processing | — | yellow | [Migration](/package/simdutf/9.3.0/migration) |
| `simdutf` | `9.3.3` | SIMD accelerated Unicode and Base64 processing | — | yellow | [Migration](/package/simdutf/9.3.3/migration) |

## Dependency graph semantics

Direct references above come from the versioned `<dependency>` and `<extension>` elements, not from a manually maintained dependency list. Each integration guide provides the complete transitive relationship graph. Bundled upstream components are tracked separately from externally managed BuildEngine library coordinates.

## Library counts and build outcomes

The two catalog counts measure **registered unique library IDs** and **versioned integrations**, respectively. Technical build outcomes are recorded by the running FSM and its installed-consumer checks. Large application-level test programs are maintained separately in BuildEngine-Tests. The operator confirmed a complete 109/109 library FSM run with zero failures and zero blocked libraries on 15 November 2026. Historical failures should remain in dated reports, not as the current status.



## Graphics and shader libraries added in November 2026

The following versions are maintained by BuildEngine and have dedicated English migration guides with versioned build switches and Mermaid dependency graphs. The operational XML contract is authoritative for dependency roots, library names and toolchain paths.

| Package | Version | Guide |
| --- | --- | --- |
| Highway | 1.4.0 | [Configuration](/package/highway/1.4.0/migration) |
| Wuffs | 0.3 | [Configuration](/package/wuffs/0.3/migration) |
| SPIRV-Headers | v1.202609.0 | [Configuration](/package/spirv-headers/v1.202609.0/migration) |
| SPIRV-Tools | 2026.4 | [Configuration](/package/spirv-tools/2026.4/migration) |
| SPIRV-Cross | vulkan-sdk-1.4.363.0 | [Configuration](/package/spirv-cross/vulkan-sdk-1.4.363.0/migration) |
| glslang | 16.6.0 | [Configuration](/package/glslang/16.6.0/migration) |
| libjxl | 0.13.0 | [Configuration](/package/libjxl/0.13.0/migration) |
| libyuv | r1924 | [Configuration](/package/libyuv/r1924/migration) |
| libaom | 3.15.3 | [Configuration](/package/libaom/3.15.3/migration) |
| libavif | 1.4.2 | [Configuration](/package/libavif/1.4.2/migration) |
| Vulkan-Headers | 1.4.368 | [Configuration](/package/vulkan-headers/1.4.368/migration) |
| Vulkan Memory Allocator | 3.4.0 | [Configuration](/package/vulkan-memory-allocator/3.4.0/migration) |
| Skia | 153 | [Complete dependency and GN guide](/package/skia/153/migration) |

**Historical build summary (15 November 2026):** 109 / 109 finished, 0 failed and 0 blocked. The 28 November 116/152 catalog is a separate later baseline, reported complete by the operator. A package dependency does not itself prove that upstream vendor sources have been removed from Skia's GN graph.
