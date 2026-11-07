# LZ4 1.10.0: native BCC64X integration

[TOC|Content]

**Version:** `lz4@1.10.0` · **Target:** Windows x64 / BCC64X · **Product:** shared DLL only

Build upstream `build/cmake` directly with CMake/Ninja; retain all original upstream source files. Disable the static product and CLI; exported C functions are used via the shared library.

- The Admin CMake adapter `cmake/packages/lz4-bcc64x.cmake` sets `LZ4_DLL_EXPORT=1` on the shared target; upstream's Windows export switch is MSVC-specific and does not cover BCC64X.
- Export BCC64X import target `LZ4::lz4_shared`, with `LZ4_DLL_IMPORT=1` on consumers.
- Expected: `liblz4.dll/liblz4.lib` (Release), `liblz4d.dll/liblz4d.lib` (Debug), exported CMake target files, pkg-config, and headers.
- The consumer smoke checks block compression, decompression, frame compression, and matching runtime/header version.
- The source release has an upstream `.sha256` sidecar; **pin the verified official digest in the XML before production promotion**. It is deliberately not guessed.
- The build/test gates must run on the actual BCC64X system before declaring it ready.

**[not verified]** No BCC64X compilation/runtime evidence was available in this environment. libarchive 3.8.11 is intentionally not modified; update it only after both LZ4 and zlib 1.4 complete verification.

Source: https://github.com/lz4/lz4/releases/tag/v1.10.0
