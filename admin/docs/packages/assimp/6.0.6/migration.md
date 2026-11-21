# Assimp 6.0.6: managed zlib variants and PE import validation
[TOC|Content]

**Target:** C++Builder 13 / BCC64X 20.1.7 / Windows x64.
**Status:** integration correction declared; native end-to-end verification **[not verified]**.

## Dependency and variant contract

Assimp imports 3D models. This version uses a shared library and independently managed zlib 1.4; Assimp's bundled zlib remains disabled. Zstandard is not declared as an Assimp dependency.

~~~mermaid
flowchart LR
   A[Assimp 6.0.6] --> Z[zlib 1.4]
~~~

| Variant | zlib import library | Runtime DLL |
| --- | --- | --- |
| Release | `lib/win64/Release/libz.lib` | `bin/win64/Release/libz.dll` |
| Debug | `lib/win64/Debug/libzd.lib` | `bin/win64/Debug/libzd.dll` |

## Incident and correction

The Release consumer exited with 0xC0000135 before any smoke CHECK. A reported `libzd.dll` request suggests a variant mismatch, but the exit code alone cannot establish which binary imports it.

The XML explicitly supplies ZLIB_LIBRARY, ZLIB_LIBRARY_RELEASE and ZLIB_LIBRARY_DEBUG from the installed zlib package. The consumer stages the exact managed runtime closure. An x64 PE import audit runs before the OBJ smoke and names the importing DLL/EXE if an inappropriate zlib variant is referenced. An incorrect native link must be fixed rather than masked by deploying both DLL variants.

The version-bound Clang/BCC64X compatibility patch uses the existing idempotent reverse-checked driver. The real model import, upstream tests, compiler and feature set are retained.

## Acceptance

After re-linking, Release must import `libz.dll` and Debug must import `libzd.dll`. The PE audit and the OBJ scene, mesh and face CHECK records must pass. The production BCC64X run is **[not verified]**.
