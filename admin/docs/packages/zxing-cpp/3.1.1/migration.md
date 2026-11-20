# ZXing-C++ 3.1.1 – BCC64X migration

## Scope and status

The BuildEngine catalog now registers ZXing-C++ 3.1.1 from the upstream release source tarball as a **native reader-only DLL integration**. Native BCC64X Debug/Release compilation, DLL export validation, package installation and consumer execution are not yet reported as PASS.

## Build contract

- Toolchain: RAD Studio 13 BCC64X (Clang/EMBT), CMake, Ninja, dynamic runtime only.
- Upstream project: source archive root, upstream `CMakeLists.txt`.
- Shared library: `BUILD_SHARED_LIBS=ON`.
- Reader API: `ZXING_READERS=ON`, `ZXING_WRITERS=OFF`; C API, Python, tests, examples and Qt are disabled.
- Dependencies: `ZXING_DEPENDENCIES=LOCAL`; no on-demand GitHub fetches.
- Each configuration installs to `lib/win64/<Configuration>` and `bin/win64/<Configuration>`, with headers and license in the versioned package.
- A native consumer calls `ZXing::ReadBarcodes` with an empty luminance image and emits the required `SMOKE|CHECK|id|PASS/FAIL|detail` and `SMOKE|RESULT|PASS/FAIL|detail` records.

## Acceptance and risk

The release-asset archive bytes and SHA-256 have not been independently verified here. Check the source-root layout, generated resource compilation through the registered Windows RC tool, exported DLL/import library names and DLL load dependency closure. A passing empty-image smoke verifies linkage and runtime entry but does not establish accuracy on real QR/1D codes or adversarial image parsing. The consumer must not turn failures into empty successful checks.
