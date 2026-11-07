# zlib 1.4: BCC64X migration

[TOC|Content]

**Version:** `zlib@1.4` · **Target:** native BCC64X Windows x64 · **Product:** shared DLL only

Retain zlib 1.3.2 for reproducibility and supersede it with zlib 1.4.
Official upstream zip: `zlib14.zip`; SHA-256:
`38bc7ed1c8ac3c870a159623deacf74a46c393ae1ad23f1dca1ed8e272deb058`.

- The upstream `win32/zlib-cmake.rc` includes `../zlib.h`; that triggers `zconf.h` including C `stddef.h`. The actual BCC64X BuildEngine log reports `RC1015: cannot open include file 'stddef.h'` at `zconf.h(268)`. The SDK resource compiler `rc.exe` and the SDK include directories are already correctly selected. The version-scoped, checked `bcc64x-rc-version-only.patch` replaces that one C header include with `ZLIB_VERSION`, `ZLIB_VER_MAJOR`, `ZLIB_VER_MINOR`, and `ZLIB_VER_REVISION` as defined in upstream zlib 1.4. No global BuildEngine changes or resource-compiler substitutions are necessary.
- Use `ZLIB_BUILD_SHARED=ON`, `ZLIB_BUILD_STATIC=OFF`, `ZLIB_PREFIX=OFF`.
- Keep the exact BCC64X product contract: Release `libz.dll/libz.lib` and Debug `libzd.dll/libzd.lib`. Verify real outputs in installed-artifact gates.
- No CTest source patch is required. Under BCC64X, upstream CTest does not prepend the DLL location because its test rules select only MSVC, MinGW, MSYS or Cygwin. The BuildEngine `test` action prepends `{InstallRoot}\\packages\\zlib\\1.4\\bin\\win64\\{Configuration}` to `PATH` for CTest and nested tests. This uses the installed Release/Debug DLL and leaves the upstream source unchanged.
- Preserve the shared CMake export, pkg-config, headers, LICENSE, upstream tests, and installed-package consumer.

**[not verified]** After this targeted resource fix, Windows/BCC64X DLL link, upstream CTest and installed consumer execution are pending. The C translation units compiled in the supplied 2026-11-07 log, but `rc.exe` failed before the DLL link. This XML addition does not update existing downstream dependency version pins.

Source: https://github.com/madler/zlib/releases/tag/v1.4
