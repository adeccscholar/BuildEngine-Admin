# spirv-cross vulkan-sdk-1.4.363.0: BuildEngine integration

[TOC|Content]

## Purpose and current integration

SPIR-V shader reflection and cross-compilation

**BuildEngine package:** `spirv-cross@vulkan-sdk-1.4.363.0`  
**Upstream source reference:** `vulkan-sdk-1.4.363.0`  
**Status:** Successful completion of the 109/109 BuildEngine integration run was confirmed on 15 November 2026. This is not a claim that Skia's separately bundled upstream copies have been replaced by the corresponding external packages.

## Direct and transitive dependencies

No direct BuildEngine package dependency is declared.

```mermaid
flowchart LR
   spirv_cross_vulkan_sdk_1_4_363_0["spirv-cross@vulkan-sdk-1.4.363.0"]
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
-DSPIRV_CROSS_STATIC:BOOL=OFF
-DSPIRV_CROSS_SHARED:BOOL=ON
-DSPIRV_CROSS_CLI:BOOL=OFF
-DSPIRV_CROSS_ENABLE_TESTS:BOOL=OFF
-DSPIRV_CROSS_SKIP_INSTALL:BOOL=OFF
-DCMAKE_BUILD_TYPE:STRING=Release
-DCMAKE_BUILD_TYPE:STRING=Debug
```

## Integration patches and special considerations

No version-specific source patch is declared in the source workflow.









## Verification and maintenance

The overall BuildEngine run was reported successful for 109 versioned library integrations. Check each package's installed artifacts and its package-specific logs for per-configuration and consumer-test evidence. The authoritative configuration is `admin/build-libraries.xml`; do not duplicate dependency paths or names as fixed values outside its variables.

[Library overview](/manual/libraries.md)
