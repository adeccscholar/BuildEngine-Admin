# libaom 3.15.3: BuildEngine integration

[TOC|Content]

## Purpose and current integration

AV1 encoder and decoder for the independently managed libavif codec

**BuildEngine package:** `libaom@3.15.3`  
**Upstream source reference:** `v3.15.3`  
**Status:** Successful completion of the 109/109 BuildEngine integration run was confirmed on 15 November 2026. This is not a claim that Skia's separately bundled upstream copies have been replaced by the corresponding external packages.

## Direct and transitive dependencies

No direct BuildEngine package dependency is declared.

```mermaid
flowchart LR
   libaom_3_15_3["libaom@3.15.3"]
```

The graph follows the version-specific BuildEngine XML dependency declarations. Shared dependencies are shown once. It does not infer binary linkage from the presence of a package in the overall closure.

## Build configuration

**Toolchain:** Windows x64, RAD Studio C++Builder BCC64X, CMake/Ninja where configured. BuildEngine injects the paths and versions from its dependency and tool registries; no machine-specific absolute path is part of this guide.

```text
-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake
-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}
-DPERL_EXECUTABLE:FILEPATH={Tool:perl}
-DGIT_EXECUTABLE:FILEPATH={Tool:git}
-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}
-DCMAKE_INSTALL_INCLUDEDIR:STRING=include
-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/{Configuration}
-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/{Configuration}
-DAOM_TARGET_CPU:STRING=x86_64
-DENABLE_NASM:BOOL=ON
-DCONFIG_AV1_DECODER:STRING=1
-DCONFIG_AV1_ENCODER:STRING=1
-DCONFIG_MULTITHREAD:STRING=1
-DCONFIG_LIBYUV:STRING=0
-DENABLE_TESTS:BOOL=OFF
-DENABLE_EXAMPLES:BOOL=OFF
-DENABLE_DOCS:BOOL=OFF
-DENABLE_TOOLS:BOOL=OFF
-DBUILD_SHARED_LIBS:BOOL=ON
-DCMAKE_BUILD_TYPE:STRING=Release
-DCMAKE_BUILD_TYPE:STRING=Debug
```

## Integration patches and special considerations

- `{BuildFileDir}\patches\libaom\3.15.3\bcc64x-native-win32-threads.patch`









## Verification and maintenance

The overall BuildEngine run was reported successful for 109 versioned library integrations. Check each package's installed artifacts and its package-specific logs for per-configuration and consumer-test evidence. The authoritative configuration is `admin/build-libraries.xml`; do not duplicate dependency paths or names as fixed values outside its variables.

[Library overview](/manual/libraries.md)
