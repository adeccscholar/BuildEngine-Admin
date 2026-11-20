# Recast Navigation 1.6.0 – BCC64X migration

## Scope and status

Recast Navigation 1.6.0 is newly registered for a source-built, DLL-only BCC64X integration. Upstream Recast, Detour, DetourCrowd, DetourTileCache and DebugUtils are included. Native Debug/Release build and consumer acceptance are pending.

## Build contract

- Upstream `CMakeLists.txt` is used with `BUILD_SHARED_LIBS=ON`, `RECASTNAVIGATION_DEMO=OFF`, `RECASTNAVIGATION_TESTS=OFF`, `RECASTNAVIGATION_EXAMPLES=OFF`.
- The upstream export-all-symbols condition is MSVC-specific; explicitly set `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON` for the BCC64X Windows DLL target path. Verify actual exported symbols, rather than assuming this option is sufficient.
- Install separate DLL/import-library artifacts for each of the five upstream targets. Do not replace DLLs with archives.
- The consumer verifies Recast and Detour allocator/deallocator calls through their imported libraries and emits the BuildEngine five-field CHECK and four-field RESULT records.

## Acceptance and risk

Verify the actual upstream install layout, all five import-library names, Debug suffixes, header visibility, DLL runtime closure, source archive SHA-256 and ABI. The smoke is intentionally small; it does not assert successful generation or navigation of a complete mesh. Keep toolchain interpretation on the Clang branch even though compatibility macros may be present.
