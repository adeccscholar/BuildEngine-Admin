# OpenEXR 3.5.4: BCC64X integration

Status: registered, native build and consumer smoke pending (21 November 2026).

External dependencies: Imath 3.2.3, OpenJPH 0.33.0, libdeflate 1.26. The initial build disables tools, examples, Python and upstream tests. OpenEXR's internal-dependency forcing options are disabled and explicit external package paths are supplied.

Warning: disabling FORCE_INTERNAL does not prevent an automatic vendored fallback when an external library is not discovered. A native configure audit must verify the resulting CMake targets. OpenEXR 3.5.4 has specific handling for OpenJPH 0.33.0 package-version and Threads behavior. Validate DLL imports, installed CMake consumption, HDR round-trip, adversarial image inputs and SBOM evidence.


## External OpenJPH cache correction (21 November 2026)

OpenEXR's upstream build caches `OPENEXR_USE_INTERNAL_OPENJPH` as an `INTERNAL` CMake variable. A previous vendored fallback may persist in a reused CMake cache even when an installed OpenJPH 0.33.0 is now discoverable. Configure with `cmake --fresh` to re-evaluate all managed external dependencies in both variants. The dependency audit remains mandatory: it requires `OPENEXR_USE_INTERNAL_OPENJPH:INTERNAL=0` and valid Imath, libdeflate, and OpenJPH package directories. This does not disable the JPEG 2000 codec or permit any vendored fallback. Release/Debug native and consumer validation remains pending.


## Mandatory external codec discovery (21 November 2026)

The native Release audit showed `openjph_DIR:PATH=openjph_DIR-NOTFOUND` and `OPENEXR_USE_INTERNAL_OPENJPH:INTERNAL=1` despite `OPENEXR_FORCE_INTERNAL_OPENJPH=OFF`. OpenJPH 0.33.0 exports a SameMinorVersion CMake configuration that can reject OpenEXR's initial 0.32.0 request. A version-bound patch now resolves the exact managed OpenJPH 0.33.0 with `find_package(... EXACT CONFIG REQUIRED)`. Missing external codecs must cause a configure error instead of silently selecting bundled codecs. The same rule requires external libdeflate and zstd 1.5.7. Zstd is now a declared dependency with a configuration root, strict audit, and a consumer-smoke package path. No OpenEXR image codec or processing feature is switched off. The installed target interfaces and HDR/JPEG 2000/Zstandard round trips still require native testing.


## BCC64X runtime CPU detection correction (21 November 2026)

After the external Imath, OpenJPH, libdeflate and zstd dependency audit succeeds, the native C99 build of `src/lib/OpenEXRCore/internal_dwa.c` includes `internal_cpuid.h`. That upstream header currently chooses MSVC's `<intrin.h>` and `__cpuidex` solely because `_WIN32` is defined. BCC64X is Clang targeting Windows GNU and reports an undeclared `__cpuidex` function. The version-bound patch selects the existing `<cpuid.h>` and `__get_cpuid` path for GNU-compatible compilers, including this Clang target, while preserving the original MSVC path and runtime SSE2/AVX/F16C checks. No SIMD or codec capability is disabled. The independent OpenEXR header consumer now emits required SMOKE checks and result records. Native DLL and smoke verification are pending.


## Installed CMake consumer location correction (21 November 2026)

Native Release and Debug builds, installs, publication and documentation now pass. The independent consumer configures `OpenEXR_DIR` explicitly to the actual managed `lib/win64/Release/cmake/OpenEXR` installation and checks that `OpenEXRConfig.cmake` exists there before calling `find_package(OpenEXR CONFIG REQUIRED)`. The smoke continues to load external Imath, OpenJPH, libdeflate and zstd package targets, and retains the native header assertion and SMOKE protocol. The provided FSM summary does not include the full `find_package` diagnostic at smoke `CMakeLists.txt:16`, so this directory correction is targeted but still requires native verification.


## Smoke-only recovery (21 November 2026)

Restore the last successfully built library timestamp (2026-11-21T05:00:00Z). The current failure was confined to the independent OpenEXR consumer's CMake configure, so a source/build/install invalidation was not appropriate. The consumer now supplies the explicitly installed OpenEXR config directory; native verification is still required. Both prior OpenEXR source patches use a new idempotent script that recognizes previously applied patches and does not fail simply because a source workspace is reused. Use `BuildEngine --make --refresh=smoke --lib=openexr --libversion=3.5.4` for smoke-only retry when upstream scope evidence remains current.
