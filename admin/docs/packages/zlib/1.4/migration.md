# zlib 1.4: BCC64X migration

[TOC|Content]

**Version:** `zlib@1.4` · **Target:** native BCC64X Windows x64 · **Product:** shared DLL only

Retain zlib 1.3.2 for reproducibility and supersede it with zlib 1.4.
Official upstream zip: `zlib14.zip`; SHA-256:
`38bc7ed1c8ac3c870a159623deacf74a46c393ae1ad23f1dca1ed8e272deb058`.

- The upstream `win32/zlib-cmake.rc` includes `../zlib.h`; that triggers `zconf.h` including C `stddef.h`. The actual BCC64X BuildEngine log reports `RC1015: cannot open include file 'stddef.h'` at `zconf.h(268)`. The SDK resource compiler `rc.exe` and the SDK include directories are already correctly selected. The version-scoped, checked `bcc64x-rc-version-only.patch` replaces that one C header include with `ZLIB_VERSION`, `ZLIB_VER_MAJOR`, `ZLIB_VER_MINOR`, and `ZLIB_VER_REVISION` as defined in upstream zlib 1.4. No global BuildEngine changes or resource-compiler substitutions are necessary.
- Use `ZLIB_BUILD_SHARED=ON`, `ZLIB_BUILD_STATIC=OFF`, `ZLIB_PREFIX=OFF`.
- Keep the exact BCC64X product contract: Release `libz.dll/libz.lib` and Debug `libzd.dll/libzd.lib`. Verify real outputs in installed-artifact gates.
- The CTest runtime DLL `PATH` remains configured by the versioned BuildEngine test action. Two nested Upstream CMake tests (`zlib_add_subdirectory_configure` and `zlib_add_subdirectory_exclude_configure`) invoke CMake again, but do not forward `CMAKE_RC_FLAGS`. In the supplied Release CTest log, these configure successfully and then fail at `win32/zlib-cmake.rc` with `RC1015: cannot open include file 'winver.h'` because the nested `rc.exe` commands lack the SDK `/I` arguments. The new checked `bcc64x-nested-ctest-rc-flags.patch` forwards `"-DCMAKE_RC_FLAGS:STRING=${CMAKE_RC_FLAGS}"` in only these two test configurations, preserving the existing compiler, toolchain and resource-compiler selection.
- Preserve the shared CMake export, pkg-config, headers, LICENSE, upstream tests, and installed-package consumer.

**Verified from the supplied 2026-11-07 Release log:** zlib's main build and the `zlib_install`, `zlib_example`, `zlib_example64`, and `find_package` CTest checks passed. CTest reported 10 of 14 tests passed; the remaining four results were two nested builds failing due to missing SDK RC include flags and their unrun dependents. **[not verified]** All 14 CTest cases after this patch, the Debug configuration, and the installed consumer smoke remain pending. This XML addition does not update existing downstream dependency version pins.

Source: https://github.com/madler/zlib/releases/tag/v1.4
