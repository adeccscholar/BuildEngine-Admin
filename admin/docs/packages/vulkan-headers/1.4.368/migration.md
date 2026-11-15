# vulkan-headers 1.4.368: BuildEngine integration

[TOC|Content]

## Purpose and current integration

Vulkan C and C++ API headers; no runtime DLL

**BuildEngine package:** `vulkan-headers@1.4.368`  
**Upstream source reference:** `v1.4.368`  
**Status:** Successful completion of the 109/109 BuildEngine integration run was confirmed on 15 November 2026. This is not a claim that Skia's separately bundled upstream copies have been replaced by the corresponding external packages.

## Direct and transitive dependencies

No direct BuildEngine package dependency is declared.

```mermaid
flowchart LR
   vulkan_headers_1_4_368["vulkan-headers@1.4.368"]
```

The graph follows the version-specific BuildEngine XML dependency declarations. Shared dependencies are shown once. It does not infer binary linkage from the presence of a package in the overall closure.

## Build configuration

**Toolchain:** Windows x64, RAD Studio C++Builder BCC64X, CMake/Ninja where configured. BuildEngine injects the paths and versions from its dependency and tool registries; no machine-specific absolute path is part of this guide.

This package is installed using the source/install workflow defined in the XML contract; no CMake `-D` switches are declared for this version.

## Integration patches and special considerations

No version-specific source patch is declared in the source workflow.







Vulkan-Headers is a headers-only package; no runtime DLL is expected.

## Verification and maintenance

The overall BuildEngine run was reported successful for 109 versioned library integrations. Check each package's installed artifacts and its package-specific logs for per-configuration and consumer-test evidence. The authoritative configuration is `admin/build-libraries.xml`; do not duplicate dependency paths or names as fixed values outside its variables.

[Library overview](/manual/libraries.md)
