# OpenEXR 3.5.4: BCC64X integration

Status: registered, native build and consumer smoke pending (21 November 2026).

External dependencies: Imath 3.2.3, OpenJPH 0.33.0, libdeflate 1.26. The initial build disables tools, examples, Python and upstream tests. OpenEXR's internal-dependency forcing options are disabled and explicit external package paths are supplied.

Warning: disabling FORCE_INTERNAL does not prevent an automatic vendored fallback when an external library is not discovered. A native configure audit must verify the resulting CMake targets. OpenEXR 3.5.4 has specific handling for OpenJPH 0.33.0 package-version and Threads behavior. Validate DLL imports, installed CMake consumption, HDR round-trip, adversarial image inputs and SBOM evidence.


## External OpenJPH cache correction (21 November 2026)

OpenEXR's upstream build caches `OPENEXR_USE_INTERNAL_OPENJPH` as an `INTERNAL` CMake variable. A previous vendored fallback may persist in a reused CMake cache even when an installed OpenJPH 0.33.0 is now discoverable. Configure with `cmake --fresh` to re-evaluate all managed external dependencies in both variants. The dependency audit remains mandatory: it requires `OPENEXR_USE_INTERNAL_OPENJPH:INTERNAL=0` and valid Imath, libdeflate, and OpenJPH package directories. This does not disable the JPEG 2000 codec or permit any vendored fallback. Release/Debug native and consumer validation remains pending.


## Mandatory external codec discovery (21 November 2026)

The native Release audit showed `openjph_DIR:PATH=openjph_DIR-NOTFOUND` and `OPENEXR_USE_INTERNAL_OPENJPH:INTERNAL=1` despite `OPENEXR_FORCE_INTERNAL_OPENJPH=OFF`. OpenJPH 0.33.0 exports a SameMinorVersion CMake configuration that can reject OpenEXR's initial 0.32.0 request. A version-bound patch now resolves the exact managed OpenJPH 0.33.0 with `find_package(... EXACT CONFIG REQUIRED)`. Missing external codecs must cause a configure error instead of silently selecting bundled codecs. The same rule requires external libdeflate and zstd 1.5.7. Zstd is now a declared dependency with a configuration root, strict audit, and a consumer-smoke package path. No OpenEXR image codec or processing feature is switched off. The installed target interfaces and HDR/JPEG 2000/Zstandard round trips still require native testing.
