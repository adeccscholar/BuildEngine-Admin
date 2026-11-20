# Zint 2.16.0 — BCC64X native shared integration

## Purpose

[Zint 2.16.0](https://github.com/zint/zint/tree/2.16.0) is an independently maintained, installed **BSD-3-Clause** barcode generator. It supplies the new ZXing-C++ 3.1.1 writer backend via the upstream `zint::zint` CMake target. The native barcode reader remains ZXing-C++; generation and rasterization use the external Zint DLL. This increases the number of distinct managed libraries by one without bundling a duplicate copy in ZXing.

## Build configuration

- Source release: upstream GitHub tag `2.16.0`; directory `zint-2.16.0`.
- Toolchain: BCC64X Clang 20.1.7, Ninja, CMake 4.x; no MSVC/MinGW compiler.
- `ZINT_SHARED=ON`, `ZINT_STATIC=OFF`, `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON`.
- `ZINT_USE_PNG=ON` with mandatory managed **libpng 1.6.59**, which itself depends on zlib. `ZINT_FRONTEND=ON` retains the upstream command-line frontend; `ZINT_TEST=ON` retains upstream test targets.
- `ZINT_USE_GS1SE=ON` preserves upstream GS1 Syntax Engine discovery. GS1 Syntax Engine is not yet a registered BuildEngine package; do not report its strict validation as available without build evidence. `ZINT_USE_QT=OFF` is an explicit project decision: no Qt interface is needed in this integration.
- Variant separation: `lib/win64/Release`, `lib/win64/Debug`, matching `bin/win64/<Configuration>`. Inspect the actual DLL and import-library names after the first build.
- The upstream install already places `zint-config.cmake` alongside `zint-targets.cmake` in `lib/win64/<Configuration>/cmake/zint`. No file relocation is needed; ZXing uses this installed location via `zint_DIR`.
- License copied to `licenses/zint/LICENSE`; `zint.h` and CMake exports are mandatory install evidence.

The managed libpng dependency enables actual PNG output. The native smoke verifies a generated PNG signature in addition to QR modules and vector output. GS1 Syntax Engine remains an optional, not-yet-fulfilled capability; Qt support is intentionally excluded from this build.

## Acceptance requirements

1. Source integrity and release archive SHA-256 verification.
2. Native Release and Debug DLL/import-library build and exports.
3. Installed header, package CMake configuration and license/SBOM checks.
4. BuildEngine `zint` native smoke: QR encoding, vector output and libpng-backed PNG file output with valid CHECK/RESULT records.
5. ZXing external writer smoke: Zint DLL symbol, QR creation/rendering/decoding round-trip. Confirm the DLL runtime closure really loads the separately installed Zint artifact.
6. Barcode Signal Lab with five formats and independent CTest verification.

**[not verified]** The new source archive, native compiler execution, exact generated names, runtime closure and Windows host smoke results are not yet available in this conversation. Registration is not a PASS assertion.
