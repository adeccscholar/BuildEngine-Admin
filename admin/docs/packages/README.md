# BuildEngine Package Migration and Consumer Documentation

[TOC|Content]

This catalog links **95 version-specific migration guides** from the public BuildEngine Administration repository. The complete library catalog and configuration are determined by the current `admin/build-libraries.xml`; a documentation page may describe a superseded version or a configuration that has not yet passed native BCC64X builds.

Each versioned guide documents the intended native C++Builder 13/BCC64X build profile, upstream integration, chosen dependencies, CMake or other build settings, patch decisions, installation artifacts, runtime boundaries and consumer evidence. **The contract describes what should happen, not proof that it happened successfully.**

## Versioned package guides

- [abseil 20260817.0](/package/abseil/20260817.0/migration)
- [ace 8.0.6](/package/ace/8.0.6/migration)
- [ace 8.0.8](/package/ace/8.0.8/migration)
- [agg 2.6](/package/agg/2.6/migration)
- [bitmapplusplus 1.1.1](/package/bitmapplusplus/1.1.1/migration)
- [boost 1.92.0](/package/boost/1.92.0/migration)
- [brotli 1.2.0](/package/brotli/1.2.0/migration)
- [brotli 1.2.1](/package/brotli/1.2.1/migration)
- [bzip2 1.0.8](/package/bzip2/1.0.8/migration)
- [catch2 3.16.0](/package/catch2/3.16.0/migration)
- [catch2 3.16.1](/package/catch2/3.16.1/migration)
- [cmark-gfm 0.29.0.gfm.13](/package/cmark-gfm/0.29.0.gfm.13/migration)
- [curl 8.21.0](/package/curl/8.21.0/migration)
- [curl 8.23.0](/package/curl/8.23.0/migration)
- [eigen 5.0.1](/package/eigen/5.0.1/migration)
- [expat 2.10.0](/package/expat/2.10.0/migration)
- [expat 2.8.4](/package/expat/2.8.4/migration)
- [expat 2.9.0](/package/expat/2.9.0/migration)
- [fast_float 8.2.3](/package/fast_float/8.2.3/migration)
- [fast_float 8.3.3](/package/fast_float/8.3.3/migration)
- [freetype 2.14.3](/package/freetype/2.14.3/migration)
- [freetype 2.15.0](/package/freetype/2.15.0/migration)
- [glew 2.3.1](/package/glew/2.3.1/migration)
- [google-benchmark 1.10.0](/package/google-benchmark/1.10.0/migration)
- [googletest 1.17.0](/package/googletest/1.17.0/migration)
- [googletest 1.18.0](/package/googletest/1.18.0/migration)
- [graphite2 1.3.15](/package/graphite2/1.3.15/migration)
- [harfbuzz 14.4.0](/package/harfbuzz/14.4.0/migration)
- [harfbuzz 15.1.0](/package/harfbuzz/15.1.0/migration)
- [icu 78.3](/package/icu/78.3/migration)
- [icu 79.1](/package/icu/79.1/migration)
- [lcms2 2.19.1](/package/lcms2/2.19.1/migration)
- [lcms2 2.20](/package/lcms2/2.20/migration)
- [leptonica 1.87.0](/package/leptonica/1.87.0/migration)
- [libarchive 3.8.10](/package/libarchive/3.8.10/migration)
- [libarchive 3.8.11](/package/libarchive/3.8.11/migration)
- [libarchive 3.8.9](/package/libarchive/3.8.9/migration)
- [libdeflate 1.26](/package/libdeflate/1.26/migration)
- [libjpeg-turbo 3.2.0](/package/libjpeg-turbo/3.2.0/migration)
- [libjpeg-turbo 3.2.1](/package/libjpeg-turbo/3.2.1/migration)
- [libpng 1.6.58](/package/libpng/1.6.58/migration)
- [libpng 1.6.59](/package/libpng/1.6.59/migration)
- [libpq 18.6](/package/libpq/18.6/migration)
- [libpqxx 8.0.2](/package/libpqxx/8.0.2/migration)
- [libtiff 4.7.2](/package/libtiff/4.7.2/migration)
- [libxml2 2.15.3](/package/libxml2/2.15.3/migration)
- [libzip 1.11.4](/package/libzip/1.11.4/migration)
- [libzip 1.12.1](/package/libzip/1.12.1/migration)
- [llvm-openmp 20.1.7](/package/llvm-openmp/20.1.7/migration)
- [lz4 1.10.0](/package/lz4/1.10.0/migration)
- [lz4 1.11.0](/package/lz4/1.11.0/migration)
- [mariadb-connector-c 3.4.11](/package/mariadb-connector-c/3.4.11/migration)
- [nlohmann-json 3.12.0](/package/nlohmann-json/3.12.0/migration)
- [onetbb 2023.1.0](/package/onetbb/2023.1.0/migration)
- [onetbb 2023.2.0](/package/onetbb/2023.2.0/migration)
- [opencv 5.0.0](/package/opencv/5.0.0/migration)
- [opencv 5.1.0](/package/opencv/5.1.0/migration)
- [opengl 26.2.1](/package/opengl/26.2.1/migration)
- [openjpeg 2.5.4](/package/openjpeg/2.5.4/migration)
- [openssl 3.5.8](/package/openssl/3.5.8/migration)
- [openssl 3.5.9](/package/openssl/3.5.9/migration)
- [openxlsx 0.5.1](/package/openxlsx/0.5.1/migration)
- [pdfium 2025.11.19](/package/pdfium/2025.11.19/migration)
- [podofo 1.1.1](/package/podofo/1.1.1/migration)
- [podofo 1.1.2](/package/podofo/1.1.2/migration)
- [poppler 26.09.0](/package/poppler/26.09.0/migration)
- [poppler 26.11.0](/package/poppler/26.11.0/migration)
- [pugixml 1.16](/package/pugixml/1.16/migration)
- [qpdf 12.4.1](/package/qpdf/12.4.1/migration)
- [qpdf 12.4.2](/package/qpdf/12.4.2/migration)
- [raylib 6.0](/package/raylib/6.0/migration)
- [raylib 6.2](/package/raylib/6.2/migration)
- [sdl2 2.32.10](/package/sdl2/2.32.10/migration)
- [simdutf 9.3.0](/package/simdutf/9.3.0/migration)
- [simdutf 9.3.3](/package/simdutf/9.3.3/migration)
- [skia 144](/package/skia/144/migration)
- [skia 153](/package/skia/153/migration)
- [soil2 1.3.0](/package/soil2/1.3.0/migration)
- [sqlite 3.53.4](/package/sqlite/3.53.4/migration)
- [sqlite 3.54.1](/package/sqlite/3.54.1/migration)
- [tao 4.0.6](/package/tao/4.0.6/migration)
- [tao 4.0.8](/package/tao/4.0.8/migration)
- [teckit 2.5.13](/package/teckit/2.5.13/migration)
- [tesseract 5.5.3](/package/tesseract/5.5.3/migration)
- [tesseract 5.5.4](/package/tesseract/5.5.4/migration)
- [vtk 9.6.2](/package/vtk/9.6.2/migration)
- [vtk 9.7.2](/package/vtk/9.7.2/migration)
- [wil 1.0.260126.7](/package/wil/1.0.260126.7/migration)
- [win-iconv 0.0.8](/package/win-iconv/0.0.8/migration)
- [xerces-c 3.3.0](/package/xerces-c/3.3.0/migration)
- [xz 5.8.3](/package/xz/5.8.3/migration)
- [xz 5.8.4](/package/xz/5.8.4/migration)
- [zlib 1.3.2](/package/zlib/1.3.2/migration)
- [zlib 1.4](/package/zlib/1.4/migration)
- [zstd 1.5.7](/package/zstd/1.5.7/migration)

## Legacy, non-versioned notes

- `mariadb-connector-c/migration.md`

These legacy notes are retained for provenance and are not a substitute for version-specific contracts.

## Evidence and language requirements

A declared XML setting is not a passing build. A patch is evidence of a deliberate adaptation, not proof that every downstream target succeeded. PASS claims must refer to the exact source version, BCC64X build variant, installation and matching FSM/consumer evidence. Where that evidence is missing, use **[not verified]**.

All public server documentation, package migrations, news items and integration instructions are written in English. Keep original technical flags, path names, symbols and version identifiers unchanged when editing prose.

[Library catalog](/manual/libraries.md) · [Development news](/news/) · [Project story](/manual/story.md)
