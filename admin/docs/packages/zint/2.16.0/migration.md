# Zint 2.16.0 — BCC64X native shared integration

## Purpose

[Zint 2.16.0](https://github.com/zint/zint/tree/2.16.0) is an independently maintained, installed **BSD-3-Clause** barcode generator. It supplies the new ZXing-C++ 3.1.1 writer backend via the upstream `zint::zint` CMake target. The native barcode reader remains ZXing-C++; generation and rasterization use the external Zint DLL. This increases the number of distinct managed libraries by one without bundling a duplicate copy in ZXing.

## Build configuration

- Source release: upstream GitHub tag `2.16.0`; directory `zint-2.16.0`.
- Toolchain: BCC64X Clang 20.1.7, Ninja, CMake 4.x; no MSVC/MinGW compiler.
- `ZINT_SHARED=ON`, `ZINT_STATIC=OFF`, `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON`.
- `ZINT_USE_PNG=OFF`, `ZINT_USE_GS1SE=OFF`, `ZINT_USE_QT=OFF`, `ZINT_FRONTEND=OFF`, `ZINT_TEST=OFF`.
- Variant separation: `lib/win64/Release`, `lib/win64/Debug`, matching `bin/win64/<Configuration>`. Inspect the actual DLL and import-library names after the first build.
- The upstream `zint-config.cmake` is placed adjacent to the installed `zint-targets.cmake` in `lib/win64/<Configuration>/cmake/zint`. This prevents a broken exported-target relative include when consuming the installed package.
- License copied to `licenses/zint/LICENSE`; `zint.h` and CMake exports are mandatory install evidence.

The `ZINT_USE_PNG=OFF` choice removes libpng/zlib from this new package's dependency chain; vector symbols, SVG-related capabilities and in-memory raster output remain in scope. The optional external GS1 Syntax Engine is not bundled or fetched. Support for particular GS1 validation modes must not be assumed.

## Acceptance requirements

1. Source integrity and release archive SHA-256 verification.
2. Native Release and Debug DLL/import-library build and exports.
3. Installed header, package CMake configuration and license/SBOM checks.
4. BuildEngine `zint` native smoke: QR encoding and vector output with valid CHECK/RESULT records.
5. ZXing external writer smoke: Zint DLL symbol, QR creation/rendering/decoding round-trip. Confirm the DLL runtime closure really loads the separately installed Zint artifact.
6. Barcode Signal Lab with five formats and independent CTest verification.

**[not verified]** The new source archive, native compiler execution, exact generated names, runtime closure and Windows host smoke results are not yet available in this conversation. Registration is not a PASS assertion.
