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

The XML explicitly supplies ZLIB_LIBRARY, ZLIB_LIBRARY_RELEASE and ZLIB_LIBRARY_DEBUG from the installed zlib package. Because Assimp installs separate CMake exports into `lib/win64/Release/cmake/assimp-6.0` and `lib/win64/Debug/cmake/assimp-6.0`, the consumer now resolves **only** the exact selected configuration. It requires matching `IMPORTED_CONFIGURATIONS`, `IMPORTED_LOCATION_<CONFIG>` and `IMPORTED_IMPLIB_<CONFIG>` values, and checks the imported DLL and library paths against the selected variant. CMake must not silently substitute a Debug import library for Release. The actual imported DLL is staged under its target-defined filename, and an x64 PE import audit checks the smoke executable's import of that exact DLL along with the zlib variant. Incorrect native links fail before the smoke executes; the opposite-variant DLL is never deployed as a workaround.

The version-bound Clang/BCC64X compatibility patch uses the existing idempotent reverse-checked driver. The real model import, upstream tests, compiler and feature set are retained.

## Runtime closure incident (2026-11-21)

The independent Release smoke log confirms that its executable imports `libassimp.dll`, and the actual Release library imports `libz.dll`. The common runtime stager nevertheless copied `libassimpd.dll` from the package's unversioned `bin/` directory. That unrelated Debug library imports `libzd.dll`, so the PE audit correctly rejected the extra DLL; it did not identify an error in the Release consumer's own native link.

The consumer's exact variant exclusion list now includes `libassimpd.dll` and `libzd.dll` for Release, and `libassimp.dll` and `libz.dll` for Debug. The shared staging implementation honors `EXCLUDE_DLLS` before copying and purges retired names from the consumer output. Installed package artifacts are not removed. This leaves the actual selected Assimp DLL and its matching zlib dependency intact, with the PE audit still enabled.

## Acceptance

After re-linking, Release must import its explicitly exported Release Assimp DLL and `libz.dll`; Debug must use the Debug Assimp DLL and `libzd.dll`. The PE audit must confirm that the consumer imports the selected Assimp target DLL and that no binary imports the opposite zlib variant. The OBJ scene, mesh and face CHECK records must then pass. The production BCC64X run is **[not verified]**.
