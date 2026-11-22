# dxc 1.9.2609: native BCC64X integration

[TOC|Content]

**Status: [not verified]**. A declared source/package configuration is not a successful native build or consumer test.

Upstream is pinned in admin/build-libraries.xml. C++Builder 13 BCC64X, Ninja, installed dependency packages and the dynamic runtime are mandatory. Keep upstream features and tests where provided. Do not use unpinned network fetching or alternative MSVC/MinGW compiler pipelines.

The LLVM-derived compiler must build and run its native tools and HLSL/DXIL/SPIR-V tests; its prebuilt DLL is not a valid native compilation substitute.

Complete Release/Debug, install, metadata, license, SBOM, publish, native test/consumer and FSM evidence remains **[not verified]**.

[Library catalog](/manual/libraries.md) · [Development news](/news/2026-11-22/)


## First native run and scoped correction

LLVM GetHostTriple.cmake previously tried to execute config.guess using sh on native Windows because BCC64X is neither MSVC nor MinGW. The local source patch identifies Clang+EMBT before the shell fallback and sets the Windows LLVM target triple. DXC's LLVM build still needs native verification. **[not verified]** until BCC64X Release and Debug rerun.

## Managed Python interpreter and host triple, 22 November 2026

The complete CMake configure log identified `Could NOT find Python3 (missing: Python3_EXECUTABLE Interpreter)` at the original LLVM root `CMakeLists.txt:451`. BuildEngine already validates Python 3.14.7. The DXC package now passes `Python3_EXECUTABLE={Tool:python}` explicitly into both Release and Debug configuration. The existing BCC64X host-triple patch remains active; a version-bound follow-up normalizes the corresponding CMake `else()` and `endif()` block keywords, resolving the recorded developer warning instead of suppressing it.

Upstream LLVM/DXC targets, SPIR-V features and CTest gates stay enabled. Native configure/build/test results are **[not verified]** until rerun.
