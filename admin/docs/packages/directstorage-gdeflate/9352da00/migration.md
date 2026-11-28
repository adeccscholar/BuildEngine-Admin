# directstorage-gdeflate 9352da00: native BCC64X integration

[TOC|Content]

**Status: [not verified]**. A declared source/package configuration is not a successful native build or consumer test.

Upstream is pinned in admin/build-libraries.xml. C++Builder 13 BCC64X, Ninja, installed dependency packages and the dynamic runtime are mandatory. Keep upstream features and tests where provided. Do not use unpinned network fetching or alternative MSVC/MinGW compiler pipelines.

This is the Apache-2.0 open GDeflate reference implementation from the DirectStorage samples, NOT the proprietary DirectStorage runtime SDK. It depends on managed libdeflate 1.26.

Complete Release/Debug, install, metadata, license, SBOM, publish, native test/consumer and FSM evidence remains **[not verified]**.

[Library catalog](/manual/libraries.md) · [Development news](/news/2026-11-22/)


## Upstream source mismatch correction

Native Release/Debug builds failed at `GDeflateDecompress.cpp:29` because regular libdeflate 1.26 does not contain `libdeflate_gdeflate_decompressor`. The Microsoft repository's actual Git submodule is NVIDIA libdeflate commit `8ba9502f...`. This package now depends on the independent managed `libdeflate-gdeflate/1.8-8ba9502f`, not regular libdeflate 1.26. Its CMake consumer uses the installed `NVIDIA::libdeflate_gdeflate` exported target. The native build, round-trip and runtime remain **[not verified]**.

## Integration retrospective and showcase contract (28 November 2026)

This is the source-available GDeflate reference codec from the DirectStorage repository, not the closed-source DirectStorage SDK or a GPU DirectStorage runtime. The managed dependency is the separate NVIDIA libdeflate GDeflate fork. Showcase comparisons should measure compression and decompression throughput on identical buffers and verify byte-for-byte roundtrips.

**Operational boundary:** The operator reported that the developer and Windows Server 2025 integration environments now complete their library work. This is an operator-reported outcome, not a newly executed BCC64X build in this documentation update. Shader compilation, graphics-card selection, measured frame rate and future showcase acceptance remain independent checks. Do not replace the managed C++Builder 13 / BCC64X toolchain, change another library's timestamp or disable upstream tests to obtain a visual result.

[Engineering retrospective](/news/2026-11-28/) · [Library catalog](/manual/libraries.md) · [Showcases](/showcases/)
