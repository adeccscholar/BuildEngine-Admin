# BuildEngine Third-Party Libraries

[TOC|Content]

**Catalog snapshot, 10 November 2026:** Schema 17 contains 95 versioned library records across 62 IDs, derived from the current `admin/build-libraries.xml`.

Each version has a [server documentation page with configuration, porting notes and a complete transitive Mermaid dependency graph](/library-docs/). Registering a build does not imply that native binaries were successfully produced.

[News and history](/news/) · [Library status](/libraries/) · [Skia review](/manual/skia-externalization.md)

## Bibliotheksversionen

### archive (5)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `libarchive` | `3.8.10` (superseded) | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib@1.3.2, xz@5.8.4, zstd@1.5.7, bzip2@1.0.8, openssl@3.5.9 | yellow | [Documentation](/package/libarchive/3.8.10/migration) |
| `libarchive` | `3.8.11` | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib@1.4, xz@5.8.4, zstd@1.5.7, lz4@1.11.0, bzip2@1.0.8, openssl@3.5.9 | yellow | [Documentation](/package/libarchive/3.8.11/migration) |
| `libarchive` | `3.8.9` (superseded) | Multi-format archive and compression library for reading and writing tar, cpio, ZIP, and related formats. | zlib@1.3.2, xz@5.8.3, zstd@1.5.7, bzip2@1.0.8, openssl@3.5.9 | yellow | [Documentation](/package/libarchive/3.8.9/migration) |
| `libzip` | `1.11.4` (superseded) | C library for reading, creating, and modifying ZIP archives. | zlib@1.4, xz@5.8.4, zstd@1.5.7 | green | [Documentation](/package/libzip/1.11.4/migration) |
| `libzip` | `1.12.1` | C library for reading, creating, and modifying ZIP archives. | zlib@1.4, xz@5.8.4, zstd@1.5.7 | green | [Documentation](/package/libzip/1.12.1/migration) |

### compression (11)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `brotli` | `1.2.0` (superseded) | General-purpose lossless compression library providing Brotli encoder and decoder support. | — | green | [Documentation](/package/brotli/1.2.0/migration) |
| `brotli` | `1.2.1` | General-purpose lossless compression library providing Brotli encoder and decoder support. | — | green | [Documentation](/package/brotli/1.2.1/migration) |
| `bzip2` | `1.0.8` | Lossless block-sorting compression library implementing the bzip2 format. | — | green | [Documentation](/package/bzip2/1.0.8/migration) |
| `libdeflate` | `1.26` | Fast native DEFLATE, zlib and gzip compression/decompression; distinct from zlib streaming interfaces. | zlib@1.4 | green | [Documentation](/package/libdeflate/1.26/migration) |
| `lz4` | `1.10.0` (superseded) | Fast lossless LZ4 block and frame compression with BCC64X shared DLL. | — | green | [Documentation](/package/lz4/1.10.0/migration) |
| `lz4` | `1.11.0` | Fast lossless LZ4 block and frame compression with BCC64X shared DLL. | — | green | [Documentation](/package/lz4/1.11.0/migration) |
| `xz` | `5.8.3` (superseded) | Data compression library and tools providing the LZMA and LZMA2 algorithms through liblzma. | — | yellow | [Documentation](/package/xz/5.8.3/migration) |
| `xz` | `5.8.4` | Data compression library and tools providing the LZMA and LZMA2 algorithms through liblzma. | — | yellow | [Documentation](/package/xz/5.8.4/migration) |
| `zlib` | `1.3.2` (superseded) | General-purpose DEFLATE compression library used for gzip-compatible data and streams. | — | green | [Documentation](/package/zlib/1.3.2/migration) |
| `zlib` | `1.4` | General-purpose DEFLATE compression library used for gzip-compatible data and streams. | — | green | [Documentation](/package/zlib/1.4/migration) |
| `zstd` | `1.5.7` | High-performance lossless compression library implementing the Zstandard format. | — | green | [Documentation](/package/zstd/1.5.7/migration) |

### concurrency (3)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `llvm-openmp` | `20.1.7` | Self-built dynamic LLVM libomp runtime, BCC64X compiler consumer validation. | — | green | [Documentation](/package/llvm-openmp/20.1.7/migration) |
| `onetbb` | `2023.1.0` (superseded) | Task-based C++ parallelism runtime, parallel algorithms and flow graphs. | — | green | [Documentation](/package/onetbb/2023.1.0/migration) |
| `onetbb` | `2023.2.0` | Task-based C++ parallelism runtime, parallel algorithms and flow graphs. | — | green | [Documentation](/package/onetbb/2023.2.0/migration) |

### core (1)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `abseil` | `20260817.0` | Google Abseil C++ utilities and containers | — | yellow | [Documentation](/package/abseil/20260817.0/migration) |

### data (10)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `expat` | `2.10.0` | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | green | [Documentation](/package/expat/2.10.0/migration) |
| `expat` | `2.8.4` (superseded) | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | green | [Documentation](/package/expat/2.8.4/migration) |
| `expat` | `2.9.0` (superseded) | Streaming XML parser used as a native dependency by TECkit SFconv and other XML-consuming components. | — | green | [Documentation](/package/expat/2.9.0/migration) |
| `fast_float` | `8.2.3` (superseded) | High-performance C++ floating-point parser | — | yellow | [Documentation](/package/fast_float/8.2.3/migration) |
| `fast_float` | `8.3.3` | High-performance C++ floating-point parser | — | yellow | [Documentation](/package/fast_float/8.3.3/migration) |
| `libxml2` | `2.15.3` | C XML toolkit providing parsing, XPath, XML Schema, Relax NG, XInclude and serialization support. | zlib@1.4 | green | [Documentation](/package/libxml2/2.15.3/migration) |
| `nlohmann-json` | `3.12.0` | Modern C++ JSON library offering intuitive STL-like parsing, serialization, and data access. | — | green | [Documentation](/package/nlohmann-json/3.12.0/migration) |
| `openxlsx` | `0.5.1` | C++ library for reading and writing Office Open XML XLSX workbooks. | pugixml@1.16, libzip@1.12.1, boost@1.92.0 | green | [Documentation](/package/openxlsx/0.5.1/migration) |
| `pugixml` | `1.16` | Lightweight C++ XML processing library with DOM-style parsing, modification, and XPath support. | — | green | [Documentation](/package/pugixml/1.16/migration) |
| `xerces-c` | `3.3.0` | Apache C++ XML parser supporting DOM, SAX, validation, and XML Schema. | — | green | [Documentation](/package/xerces-c/3.3.0/migration) |

### database (5)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `libpq` | `18.6` | Official PostgreSQL C client library and wire-protocol access API. | openssl@3.5.9, zlib@1.4 | green | [Documentation](/package/libpq/18.6/migration) |
| `libpqxx` | `8.0.2` | Modern C++ client API for PostgreSQL built on the official libpq C client library. | libpq@18.6 | green | [Documentation](/package/libpqxx/8.0.2/migration) |
| `mariadb-connector-c` | `3.4.11` | MariaDB native C client as a BCC64X-built shared DLL. | zlib@1.4, zstd@1.5.7 | green | [Documentation](/package/mariadb-connector-c/3.4.11/migration) |
| `sqlite` | `3.53.4` (superseded) | Embedded relational database engine providing a self-contained SQL database in a single library. | — | green | [Documentation](/package/sqlite/3.53.4/migration) |
| `sqlite` | `3.54.1` | Embedded relational database engine providing a self-contained SQL database in a single library. | — | green | [Documentation](/package/sqlite/3.54.1/migration) |

### documentation (8)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `cmark-gfm` | `0.29.0.gfm.13` | CommonMark parser with GitHub Flavored Markdown extensions. | — | green | [Documentation](/package/cmark-gfm/0.29.0.gfm.13/migration) |
| `pdfium` | `2025.11.19` | Pinned PDFium GN/BCC64X integration; validation pending | abseil@20260817.0, fast_float@8.3.3, skia@153, zlib@1.4, libjpeg-turbo@3.2.1, libpng@1.6.59, freetype@2.15.0, icu@79.1, lcms2@2.20, openjpeg@2.5.4 | yellow | [Documentation](/package/pdfium/2025.11.19/migration) |
| `podofo` | `1.1.1` (superseded) | C++ PDF parsing, creation and modification library with forms, annotations, signing and incremental-update support. | zlib@1.4, openssl@3.5.9, freetype@2.14.3, libxml2@2.15.3, libjpeg-turbo@3.2.0, libpng@1.6.59, libtiff@4.7.2 | yellow | [Documentation](/package/podofo/1.1.1/migration) |
| `podofo` | `1.1.2` | C++ PDF parsing, creation and modification library with forms, annotations, signing and incremental-update support. | zlib@1.4, openssl@3.5.9, freetype@2.15.0, libxml2@2.15.3, libjpeg-turbo@3.2.1, libpng@1.6.59, libtiff@4.7.2 | yellow | [Documentation](/package/podofo/1.1.2/migration) |
| `poppler` | `26.09.0` (superseded) | PDF rendering and parsing library derived from Xpdf and used by document-processing toolchains. | zlib@1.4, freetype@2.15.0, libjpeg-turbo@3.2.0, libpng@1.6.59, libtiff@4.7.2, win-iconv@0.0.8 | red | [Documentation](/package/poppler/26.09.0/migration) |
| `poppler` | `26.11.0` | PDF rendering and parsing library derived from Xpdf and used by document-processing toolchains. | zlib@1.4, freetype@2.15.0, harfbuzz@15.1.0, libjpeg-turbo@3.2.1, libpng@1.6.59, libtiff@4.7.2, win-iconv@0.0.8 | red | [Documentation](/package/poppler/26.11.0/migration) |
| `qpdf` | `12.4.1` (superseded) | PDF structural transformation library and command-line toolkit for inspecting, rewriting, encrypting and validating PDF files. | zlib@1.4, libjpeg-turbo@3.2.0 | green | [Documentation](/package/qpdf/12.4.1/migration) |
| `qpdf` | `12.4.2` | PDF structural transformation library and command-line toolkit for inspecting, rewriting, encrypting and validating PDF files. | zlib@1.4, libjpeg-turbo@3.2.1 | green | [Documentation](/package/qpdf/12.4.2/migration) |

### foundation (2)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `boost` | `1.92.0` | Comprehensive collection of portable C++ libraries covering containers, algorithms, networking, concurrency, and more. | openssl@3.5.9, zlib@1.4 | green | [Documentation](/package/boost/1.92.0/migration) |
| `wil` | `1.0.260126.7` | Microsoft MIT-licensed header-only Windows COM and RAII library. BCC64X compatibility requires a native consumer smoke before use by OpenCV. | — | yellow | [Documentation](/package/wil/1.0.260126.7/migration) |

### graphics (17)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `agg` | `2.6` | AGG core rasterization without GPC | — | yellow | [Documentation](/package/agg/2.6/migration) |
| `bitmapplusplus` | `1.1.1` | Header-only C++ bitmap image library for loading, manipulating, and saving BMP data. | — | green | [Documentation](/package/bitmapplusplus/1.1.1/migration) |
| `glew` | `2.3.1` | OpenGL Extension Wrangler Library for discovering and loading OpenGL extensions and entry points. | opengl@26.2.1 | yellow | [Documentation](/package/glew/2.3.1/migration) |
| `lcms2` | `2.19.1` (superseded) | LittleCMS 2 ICC color-management engine (GPL plugins excluded) | — | yellow | [Documentation](/package/lcms2/2.19.1/migration) |
| `lcms2` | `2.20` | LittleCMS 2 ICC color-management engine (GPL plugins excluded) | — | yellow | [Documentation](/package/lcms2/2.20/migration) |
| `opencv` | `5.0.0` (superseded) | Computer vision and image-processing library with algorithms for vision, geometry, filtering, and machine learning, Windows DirectShow camera capture (MSMF COM compatibility pending), and image stitching. Native oneTBB 2023.2 parallel backend configured; Release consumer smoke regression pending; LLVM OpenMP 20.1.7 enabled as a build dependency. | onetbb@2023.2.0, llvm-openmp@20.1.7, zlib@1.4 | yellow | [Documentation](/package/opencv/5.0.0/migration) |
| `opencv` | `5.1.0` | Computer vision and image-processing library with algorithms for vision, geometry, filtering, and machine learning, Windows DirectShow camera capture (MSMF COM compatibility pending), and image stitching. Native oneTBB 2023.2 parallel backend configured; Release consumer smoke regression pending; LLVM OpenMP 20.1.7 enabled as a build dependency. | onetbb@2023.2.0, llvm-openmp@20.1.7, zlib@1.4 | yellow | [Documentation](/package/opencv/5.1.0/migration) |
| `opengl` | `26.2.1` | Mesa-based OpenGL runtime and development package for hardware-accelerated graphics on Windows. | — | yellow | [Documentation](/package/opengl/26.2.1/migration) |
| `openjpeg` | `2.5.4` | OpenJPEG JPEG 2000 encoder and decoder library | — | yellow | [Documentation](/package/openjpeg/2.5.4/migration) |
| `raylib` | `6.0` (superseded) | Simple cross-platform C library for graphics, audio, input, and game and application development. | opengl@26.2.1 | green | [Documentation](/package/raylib/6.0/migration) |
| `raylib` | `6.2` | Simple cross-platform C library for graphics, audio, input, and game and application development. | opengl@26.2.1 | green | [Documentation](/package/raylib/6.2/migration) |
| `sdl2` | `2.32.10` | Cross-platform multimedia library for windows, input, audio, rendering, and platform integration. | — | green | [Documentation](/package/sdl2/2.32.10/migration) |
| `skia` | `144` (superseded) | Pinned Skia 144 shared CPU/raster package for PDFium 2025.11.19. Superseded for general consumers by Skia 153. | zlib@1.4, libjpeg-turbo@3.2.0, libpng@1.6.59, freetype@2.14.3, opengl@26.2.1 | yellow | [Documentation](/package/skia/144/migration) |
| `skia` | `153` | 2D graphics engine providing raster, vector, text, image, and GPU-accelerated rendering capabilities. | opengl@26.2.1, zlib@1.4, brotli@1.2.1, libjpeg-turbo@3.2.1, libpng@1.6.59, harfbuzz@15.1.0, freetype@2.15.0 | yellow | [Documentation](/package/skia/153/migration) |
| `soil2` | `1.3.0` | Small image-loading helper library for OpenGL textures based on stb image decoders. | opengl@26.2.1 | green | [Documentation](/package/soil2/1.3.0/migration) |
| `vtk` | `9.6.2` | Visualization Toolkit for 3D computer graphics, image processing, scientific visualization, and data analysis. Native oneTBB 2023.2 and LLVM OpenMP 20.1.7 SMP backends configured; consumer regression pending. | onetbb@2023.2.0, llvm-openmp@20.1.7 | green | [Documentation](/package/vtk/9.6.2/migration) |
| `vtk` | `9.7.2` | Visualization Toolkit for 3D computer graphics, image processing, scientific visualization, and data analysis. VTK 9.7.2 core pipeline, CommonCache, oneTBB 2023.2.0 and LLVM OpenMP 20.1.7; BCC64X Release/Debug validation pending. | onetbb@2023.2.0, llvm-openmp@20.1.7 | green | [Documentation](/package/vtk/9.7.2/migration) |

### image (6)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `leptonica` | `1.87.0` | Image normalization, binarization, morphology and OCR preprocessing for Tesseract. | zlib@1.4, libpng@1.6.59, libjpeg-turbo@3.2.1, libtiff@4.7.2 | yellow | [Documentation](/package/leptonica/1.87.0/migration) |
| `libjpeg-turbo` | `3.2.0` (superseded) | High-performance JPEG codec using SIMD acceleration while remaining API-compatible with libjpeg. | — | yellow | [Documentation](/package/libjpeg-turbo/3.2.0/migration) |
| `libjpeg-turbo` | `3.2.1` | High-performance JPEG codec using SIMD acceleration while remaining API-compatible with libjpeg. | — | yellow | [Documentation](/package/libjpeg-turbo/3.2.1/migration) |
| `libpng` | `1.6.58` (superseded) | Reference PNG library for reading, writing, and manipulating Portable Network Graphics images. | zlib@1.3.2 | green | [Documentation](/package/libpng/1.6.58/migration) |
| `libpng` | `1.6.59` | Reference PNG library for reading, writing, and manipulating Portable Network Graphics images. | zlib@1.4 | green | [Documentation](/package/libpng/1.6.59/migration) |
| `libtiff` | `4.7.2` | TIFF image library for reading, writing, and manipulating Tagged Image File Format images. | zlib@1.4, libjpeg-turbo@3.2.1 | green | [Documentation](/package/libtiff/4.7.2/migration) |

### math (1)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `eigen` | `5.0.1` | Header-only C++ matrix, geometry and sparse linear algebra templates. No DLL or import archive. | — | yellow | [Documentation](/package/eigen/5.0.1/migration) |

### middleware (4)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `ace` | `8.0.6` (superseded) | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. | openssl@3.5.9, xerces-c@3.3.0, zlib@1.4 | green | [Documentation](/package/ace/8.0.6/migration) |
| `ace` | `8.0.8` | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. | openssl@3.5.9, xerces-c@3.3.0, zlib@1.4 | green | [Documentation](/package/ace/8.0.8/migration) |
| `tao` | `4.0.6` (superseded) | CORBA object request broker and middleware services built on top of ACE. | ace@8.0.6 | green | [Documentation](/package/tao/4.0.6/migration) |
| `tao` | `4.0.8` | CORBA object request broker and middleware services built on top of ACE. | ace@8.0.8 | green | [Documentation](/package/tao/4.0.8/migration) |

### network (2)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `curl` | `8.21.0` (superseded) | URL transfer library and command-line client supporting HTTP(S) and numerous network protocols. | openssl@3.5.8, zlib@1.3.2 | green | [Documentation](/package/curl/8.21.0/migration) |
| `curl` | `8.23.0` | URL transfer library and command-line client supporting HTTP(S) and numerous network protocols. | openssl@3.5.9, zlib@1.4 | green | [Documentation](/package/curl/8.23.0/migration) |

### security (2)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `openssl` | `3.5.8` (superseded) | Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives. | zlib@1.3.2, brotli@1.2.0, zstd@1.5.7 | green | [Documentation](/package/openssl/3.5.8/migration) |
| `openssl` | `3.5.9` | Cryptography and TLS toolkit providing secure transport, certificates, and general cryptographic primitives. | zlib@1.4, brotli@1.2.1, zstd@1.5.7 | green | [Documentation](/package/openssl/3.5.9/migration) |

### testing (5)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `catch2` | `3.16.0` (superseded) | Modern C++ testing framework for unit tests, assertions, and test discovery. | — | green | [Documentation](/package/catch2/3.16.0/migration) |
| `catch2` | `3.16.1` | Modern C++ testing framework for unit tests, assertions, and test discovery. | — | green | [Documentation](/package/catch2/3.16.1/migration) |
| `google-benchmark` | `1.10.0` | C++ microbenchmark framework for reproducible performance measurements and JSON reports in BuildEngine-Tests. | — | green | [Documentation](/package/google-benchmark/1.10.0/migration) |
| `googletest` | `1.17.0` (superseded) | C++ testing framework providing GoogleTest and GoogleMock for unit tests and mocks. | — | green | [Documentation](/package/googletest/1.17.0/migration) |
| `googletest` | `1.18.0` | C++ testing framework providing GoogleTest and GoogleMock for unit tests and mocks. | — | green | [Documentation](/package/googletest/1.18.0/migration) |

### text (13)

| Library | Version | Purpose | Direct dependencies | License signal | Details |
| --- | --- | --- | --- | --- | --- |
| `freetype` | `2.14.3` (superseded) | Font rendering library for loading, rasterizing, and querying scalable and bitmap font formats. | zlib@1.4, bzip2@1.0.8, brotli@1.2.1, libpng@1.6.59, harfbuzz@14.4.0 | yellow | [Documentation](/package/freetype/2.14.3/migration) |
| `freetype` | `2.15.0` | Font rendering library for loading, rasterizing, and querying scalable and bitmap font formats. | zlib@1.4, bzip2@1.0.8, brotli@1.2.1, libpng@1.6.59, harfbuzz@15.1.0 | yellow | [Documentation](/package/freetype/2.15.0/migration) |
| `graphite2` | `1.3.15` | Smart-font rendering engine for Graphite fonts and complex text shaping. | — | yellow | [Documentation](/package/graphite2/1.3.15/migration) |
| `harfbuzz` | `14.4.0` (superseded) | Text shaping engine that converts Unicode text into positioned glyph sequences for complex scripts. | — | yellow | [Documentation](/package/harfbuzz/14.4.0/migration) |
| `harfbuzz` | `15.1.0` | Text shaping engine that converts Unicode text into positioned glyph sequences for complex scripts. | — | yellow | [Documentation](/package/harfbuzz/15.1.0/migration) |
| `icu` | `78.3` (superseded) | Unicode and globalization library providing text, locale, normalization, collation, and conversion services. | — | yellow | [Documentation](/package/icu/78.3/migration) |
| `icu` | `79.1` | Unicode and globalization library providing text, locale, normalization, collation, and conversion services. | — | yellow | [Documentation](/package/icu/79.1/migration) |
| `simdutf` | `9.3.0` (superseded) | SIMD accelerated Unicode and Base64 processing | — | yellow | [Documentation](/package/simdutf/9.3.0/migration) |
| `simdutf` | `9.3.3` | SIMD accelerated Unicode and Base64 processing | — | yellow | [Documentation](/package/simdutf/9.3.3/migration) |
| `teckit` | `2.5.13` | Text encoding conversion toolkit with runtime engine, mapping compiler, and Unicode conversion utilities. | zlib@1.4, expat@2.10.0 | yellow | [Documentation](/package/teckit/2.5.13/migration) |
| `tesseract` | `5.5.3` (superseded) | Dynamically linked C++ OCR engine; language traineddata is provisioned and licensed separately. | leptonica@1.87.0 | yellow | [Documentation](/package/tesseract/5.5.3/migration) |
| `tesseract` | `5.5.4` | Dynamically linked C++ OCR engine; language traineddata is provisioned and licensed separately. | leptonica@1.87.0 | yellow | [Documentation](/package/tesseract/5.5.4/migration) |
| `win-iconv` | `0.0.8` | Small Windows iconv implementation using the Win32 character conversion APIs. | — | green | [Documentation](/package/win-iconv/0.0.8/migration) |

## Dependency graphs

Library pages derive dependencies solely from versioned `<dependency>` and `<extension>` coordinates. The transitive closure is traversed completely. Internally bundled upstream libraries, such as those in Skia, must be distinguished from separately managed dependencies.

## Nachweisgrenze

Declared configuration values and registered patches do not prove successful tests. PASS claims require corresponding FSM logs and the exact resulting artifacts.
