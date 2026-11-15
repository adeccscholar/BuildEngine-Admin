# libavif 1.4.2: BuildEngine integration

[TOC|Content]

## Purpose and current integration

AVIF encoder using managed libaom; AVIF decoder using managed libgav1; libyuv conversion is managed separately

**BuildEngine package:** `libavif@1.4.2`  
**Upstream source reference:** `v1.4.2`  
**Status:** Updated to an external libgav1 decoding backend on 15 November 2026; the revised libavif and Skia build, link, runtime loader and consumer smoke remain **unverified**. The historical 109/109 build result predates this change.

## Direct and transitive dependencies

- `libaom@3.15.3`
- `libyuv@r1924`
- `libgav1@0.20.0`

```mermaid
flowchart LR
   libavif_1_4_2["libavif@1.4.2"]
   libaom_3_15_3["libaom@3.15.3"]
   libyuv_r1924["libyuv@r1924"]
   libgav1_0_20_0["libgav1@0.20.0"]
   libavif_1_4_2 --> libaom_3_15_3
   libavif_1_4_2 --> libyuv_r1924
   libavif_1_4_2 --> libgav1_0_20_0
```

The graph follows the version-specific BuildEngine XML dependency declarations. Shared dependencies are shown once. It does not infer binary linkage from the presence of a package in the overall closure.

## Build configuration

**Toolchain:** Windows x64, RAD Studio C++Builder BCC64X, CMake/Ninja where configured. BuildEngine injects the paths and versions from its dependency and tool registries; no machine-specific absolute path is part of this guide.

```text
-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake
-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}
-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}
-DCMAKE_INSTALL_INCLUDEDIR:STRING=include
-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/{Configuration}
-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/{Configuration}
-DCMAKE_PREFIX_PATH:PATH={DependencyRoot:libaom};{DependencyRoot:libyuv};{DependencyRoot:libgav1}
-DAOM_LIBRARY:FILEPATH={DependencyRoot:libaom}\lib\win64\{Configuration}\libaom_dll.lib
-DBUILD_SHARED_LIBS:BOOL=ON
-DAVIF_CODEC_AOM:STRING=SYSTEM
-DAVIF_CODEC_AOM_DECODE:BOOL=OFF
-DAVIF_CODEC_AOM_ENCODE:BOOL=ON
-DAVIF_CODEC_LIBGAV1:STRING=SYSTEM
-DLIBGAV1_INCLUDE_DIR:PATH={DependencyRoot:libgav1}\include
-DLIBGAV1_LIBRARY:FILEPATH={DependencyRoot:libgav1}\lib\win64\{Configuration}\libgav1_shared.lib
-DAVIF_CODEC_DAV1D:STRING=OFF
-DAVIF_CODEC_RAV1E:STRING=OFF
-DAVIF_CODEC_SVT:STRING=OFF
-DAVIF_LIBYUV:STRING=SYSTEM
-DAVIF_JPEG:STRING=OFF
-DAVIF_ZLIBPNG:STRING=OFF
-DAVIF_BUILD_APPS:BOOL=OFF
-DAVIF_BUILD_TESTS:BOOL=OFF
-DAVIF_BUILD_EXAMPLES:BOOL=OFF
-DAVIF_ENABLE_WERROR:BOOL=OFF
-DCMAKE_BUILD_TYPE:STRING=Release
-DCMAKE_BUILD_TYPE:STRING=Debug
```

## Integration patches and special considerations

No version-specific libavif source patch is declared in the source workflow. The external libgav1 producer carries its own version-pinned Windows install and import-library naming patch; libavif continues to consume its exact variant-specific `libgav1_shared.lib` coordinate. In the 16 November FSM run both libgav1 native builds passed, but its install failed before libavif could build. A producer contract correction has been committed; the subsequent Windows import-library install and libavif consumer remain **[not verified]**.



AV1 encoding, AV1 decoding and libyuv conversion use independently managed BuildEngine packages. Upstream libavif 1.4.2 supports `AVIF_CODEC_LIBGAV1=SYSTEM`; `LIBGAV1_LIBRARY` selects the import library from the exact BCC64X package configuration.





## Verification and maintenance

The earlier BuildEngine run was reported successful for 109 versioned library integrations, before libgav1 decoding was introduced. Check each package's installed artifacts and its package-specific logs for per-configuration and consumer-test evidence. The authoritative configuration is `admin/build-libraries.xml`; do not duplicate dependency paths or names as fixed values outside its variables.

[Library overview](/manual/libraries.md)
