# DXC 1.9.2609: Portable testing migration

The Microsoft TAEF runner (`te.exe`) is not invoked by the BuildEngine DXC integration.

The legacy upstream project `projects/dxilconv/unittests/DxilConvTests.cpp` uses WEX and TAEF macros directly. Its six automated batch/regression test methods are **not yet migrated**, and must not be counted as passing. The explicitly ignored manual method is also not migrated. Original sources are retained intact for the port.

The BuildEngine source patch redirects DXIL converter test registration to a portable CTest suite. The initial two tests compile a shader with the upstream `dxc` binary and validate DXIL and SPIR-V output container signatures. They are useful functional evidence but **not equivalent coverage** to the historical DXIL converter regression tests.

Existing upstream LLVM/LIT and other tests remain enabled. BuildEngine retains `BUILD_TESTING=ON`, `LLVM_INCLUDE_TESTS=ON`, `LLVM_BUILD_TESTS=ON`, `HLSL_BUILD_DXILCONV=ON` and `SPIRV_BUILD_TESTS=ON`.

## Migration debt

Port `BatchDxbc2dxil`, `BatchDxbc2dxilAsm`, `BatchDxilCleanup`, `BatchNormalizeDxil`, `BatchScopeNestIterator`, and `RegressionTests` to CTest or LLVM/LIT with identical fixture sets, output verification and failure semantics. Until then report original DXIL converter regression coverage as **INCOMPLETE**.

Security rationale: no need to invoke TAEF's `te.exe` as a proxy loader in the reproducible integration pipeline.

Status: Source/CTest migration authored; BCC64X Release and Debug execution pending.
