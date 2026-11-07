# zlib 1.4: BCC64X migration

[TOC|Content]

**Version:** `zlib@1.4` · **Target:** native BCC64X Windows x64 · **Product:** shared DLL only

Retain zlib 1.3.2 for reproducibility and supersede it with zlib 1.4.
Official upstream zip: `zlib14.zip`; SHA-256:
`38bc7ed1c8ac3c870a159623deacf74a46c393ae1ad23f1dca1ed8e272deb058`.

- The new upstream Windows resource is `win32/zlib-cmake.rc`.
- Use `ZLIB_BUILD_SHARED=ON`, `ZLIB_BUILD_STATIC=OFF`, `ZLIB_PREFIX=OFF`.
- Keep the exact BCC64X product contract: Release `libz.dll/libz.lib` and Debug `libzd.dll/libzd.lib`. Verify real outputs in installed-artifact gates.
- No upstream source patch is required. Under BCC64X, upstream CTest does not prepend the DLL location because its test rules select only MSVC, MinGW, MSYS or Cygwin. The BuildEngine `test` action prepends `{InstallRoot}\\packages\\zlib\\1.4\\bin\\win64\\{Configuration}` to `PATH` for CTest and nested tests. This uses the installed Release/Debug DLL and leaves the upstream source unchanged.
- Preserve the shared CMake export, pkg-config, headers, LICENSE, upstream tests, and installed-package consumer.

**[not verified]** Windows/BCC64X CMake, compile, DLL link, upstream CTest and installed consumer execution are pending. This XML addition does not update existing downstream dependency version pins.

Source: https://github.com/madler/zlib/releases/tag/v1.4
