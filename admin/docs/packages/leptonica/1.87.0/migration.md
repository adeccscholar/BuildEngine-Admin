## Updated BuildEngine contract (10 November 2026)

**Declared configuration; no new test PASS.**

### Complete transitive dependencies

```mermaid
flowchart LR
   N0["leptonica 1.87.0"]
   N1["zlib 1.4"]
   N2["libpng 1.6.59"]
   N3["libjpeg-turbo 3.2.1"]
   N4["libtiff 4.7.2"]
   N0 --> N1
   N0 --> N2
   N0 --> N3
   N0 --> N4
   N2 --> N1
   N4 --> N1
   N4 --> N3
```

Direkt: `zlib@1.4`, `libpng@1.6.59`, `libjpeg-turbo@3.2.1`, `libtiff@4.7.2`

### Current configuration

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_PROG:BOOL=OFF`
- `-DSW_BUILD:BOOL=OFF`
- `-DENABLE_ZLIB:BOOL=ON`
- `-DENABLE_PNG:BOOL=ON`
- `-DENABLE_JPEG:BOOL=ON`
- `-DENABLE_TIFF:BOOL=ON`
- `-DENABLE_GIF:BOOL=OFF`
- `-DENABLE_WEBP:BOOL=OFF`
- `-DENABLE_OPENJPEG:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:PATH={DependencyRoot:zlib};{DependencyRoot:libpng};{DependencyRoot:libjpeg-turbo};{DependencyRoot:libtiff}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2 -DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DLeptonica_DIR:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\Release\cmake\leptonica`

---

## Historical integration notes

# Leptonica 1.87.0

[TOC|Content]

## Current BuildEngine contract (10 November 2026)

**Contract data; not a verified build execution.** Earlier tests belong to their exact versions and binaries; new executions remain **[not verified]**.

Image normalization, binarization, morphology and OCR preprocessing for Tesseract.

**Coordinate:** `leptonica@1.87.0` · **Category:** `image` · **License indicator:** `yellow`

### Declared dependencies (full transitive closure)

Edges follow exact XML pins. Dashed lines indicate extensions and solid lines indicate package dependencies. Bundled upstream components are not separate BuildEngine packages.

```mermaid
flowchart LR
   N0["leptonica 1.87.0"]
   N1["libjpeg-turbo 3.2.1"]
   N2["libpng 1.6.59"]
   N3["libtiff 4.7.2"]
   N4["zlib 1.4"]
   N0 --> N4
   N0 --> N2
   N0 --> N1
   N0 --> N3
   N2 --> N4
   N3 --> N4
   N3 --> N1
```

**Direct dependencies:** `zlib@1.4`, `libpng@1.6.59`, `libjpeg-turbo@3.2.1`, `libtiff@4.7.2`

### Configuration and integration notes

**CMake arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_PROG:BOOL=OFF`
- `-DSW_BUILD:BOOL=OFF`
- `-DENABLE_ZLIB:BOOL=ON`
- `-DENABLE_PNG:BOOL=ON`
- `-DENABLE_JPEG:BOOL=ON`
- `-DENABLE_TIFF:BOOL=ON`
- `-DENABLE_GIF:BOOL=OFF`
- `-DENABLE_WEBP:BOOL=OFF`
- `-DENABLE_OPENJPEG:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:PATH={DependencyRoot:zlib};{DependencyRoot:libpng};{DependencyRoot:libjpeg-turbo};{DependencyRoot:libtiff}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2 -DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Build environment:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Source-Downloads:**

- `https://github.com/DanBloomberg/leptonica/releases/download/{LibraryVersion}/leptonica-{LibraryVersion}.tar.gz` (SHA-256: `c73363397f96eb1295602bf44d708a994ad42046c791bf03ea0505d829bdb6a7`)

**Deklarierte Smoke-Tests:**

- `consumer` / `Release` / `package` / `smokes\leptonica\consumer`

**Explizit geforderte Installationsartefakte:**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\leptonica\LeptonicaConfig.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\leptonica\allheaders.h`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\leptonica-license.txt`

**Verbindliche Sources:** `admin/build-libraries.xml` for configuration and pins; FSM/build logs for actual execution. [Library catalog](/manual/libraries.md).
