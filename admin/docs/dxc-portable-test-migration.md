# DXC 1.9.2609: Portable testing migration

The Microsoft TAEF runner (`te.exe`) is not invoked by the BuildEngine DXC integration.

The legacy upstream project `projects/dxilconv/unittests/DxilConvTests.cpp` uses WEX and TAEF macros directly. Its six automated batch/regression test methods are **not yet migrated**, and must not be counted as passing. The explicitly ignored manual method is also not migrated. Original sources are retained intact for the port.

The BuildEngine source patch redirects DXIL converter test registration to a portable CTest suite. The independent CTest suite invokes the real `dxc` executable for successful DXIL and SPIR-V compilation and additionally checks that a missing HLSL entry point is rejected, reports diagnostics, and leaves no output artifact. These three behavior tests do not import WEX, TAEF or ATL. They are useful functional evidence but **not equivalent coverage** to the historical DXIL converter regression tests.

Existing upstream LLVM/LIT and other tests remain enabled. BuildEngine retains `BUILD_TESTING=ON`, `LLVM_INCLUDE_TESTS=ON`, `LLVM_BUILD_TESTS=ON`, `HLSL_BUILD_DXILCONV=ON` and `SPIRV_BUILD_TESTS=ON`.

## Migration debt

Port `BatchDxbc2dxil`, `BatchDxbc2dxilAsm`, `BatchDxilCleanup`, `BatchNormalizeDxil`, `BatchScopeNestIterator`, and `RegressionTests` to CTest or LLVM/LIT with identical fixture sets, output verification and failure semantics. Until then report original DXIL converter regression coverage as **INCOMPLETE**.

Security rationale: no need to invoke TAEF's `te.exe` as a proxy loader in the reproducible integration pipeline.

Status (2026-11-24): Three independent behavioral CTest cases authored on `main`; BCC64X Release and Debug execution pending. The separate original HLSL/HLSLExec/DXILConv suites still contain WEX-compatibility and ATL-compatibility migration code and are not cleanly migrated. Original source-level test migration and full coverage parity remain INCOMPLETE. No original test suite is to be reported as PASS until verified.
