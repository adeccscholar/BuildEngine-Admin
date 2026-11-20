# ZXing-C++ 3.1.1 — External Zint writer migration

## Change

The existing ZXing-C++ 3.1.1 registration now declares a direct BuildEngine dependency on **Zint 2.16.0**. The previous reader-only DLL becomes a reader/writer DLL, using upstream `ZXING_WRITERS=NEW`, `ZXING_READERS=ON`, `ZXING_USE_BUNDLED_ZINT=OFF` and `ZXING_DEPENDENCIES=LOCAL`. CMake is pointed to the exact configuration-specific installed `zint-config.cmake` directory. No upstream Zint clone, embedded Zint C sources or alternative toolchain is permitted.

## Consumer and observable behavior

The native consumer uses `CreateBarcodeFromText`, `WriteBarcodeToImage` and `ReadBarcodes` to assert a real QR encoding/decoding round-trip, plus checks for the separately linked Zint API. Failures must emit the canonical BuildEngine `SMOKE|CHECK|id|FAIL|detail` and `SMOKE|RESULT|FAIL|detail` records.

The associated [Barcode Signal Lab](/showcase/graphics/barcode-signal-lab/) is extended to generate QR Code, Data Matrix, Code 128, EAN-13 and PDF417 from configurable payloads, display their actual image pixels and verify their detection using the installed reader DLL. Its manifest declares both ZXing-C++ and Zint as direct dependencies.

## Acceptance caveats

- The upstream new writer's format capabilities depend on the selected Zint configuration; not every possible barcode symbology is guaranteed.
- CMake's imported `zint::zint` target and the real exported DLL symbols must be validated with the native BCC64X build, not inferred solely from the upstream CMake.
- An existing successfully built reader-only ZXing DLL does not establish that the modified reader/writer artifact has passed.
- Use only `main`. Record validated Debug/Release, checksum, SBOM and dependency edges on the server after the real run.

**[not verified]** The updated ZXing writer integration awaits a Windows BCC64X build and full smoke validation.
