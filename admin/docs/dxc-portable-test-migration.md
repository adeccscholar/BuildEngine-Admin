# DXC 1.9.2609: independent native test architecture

Status: **implemented on main, BCC64X verification pending** (2026-11-24).

## Architectural decision

BuildEngine does not translate Microsoft TAEF, WEX, ATL or Windows test
fixtures. Its own DXC test world is built from native C++23 and HLSL test
cases against the genuine DXC products. There is no Python source rewriting,
TAEF runner, Microsoft test host, WEX assertion adapter or ATL emulation in
the new test suite.

The retired original-test translation assets were removed from the
BuildEngine-Admin repository. The original upstream DXC sources are not
modified to pretend that their Windows test fixtures are portable.

The original Windows HLSL test groups are not enabled in this BuildEngine
integration: `HLSL_INCLUDE_TESTS=OFF` deliberately replaces those suites.
This is not a test success or an assertion of original test coverage parity.
The independent C++23 suite is registered unconditionally from
`projects/dxilconv/portable` by `PrepareNativeBehaviorTests.cmake`.

Other supported testing infrastructure remains enabled:

- `BUILD_TESTING=ON`
- `LLVM_INCLUDE_TESTS=ON`
- `LLVM_BUILD_TESTS=ON`
- `HLSL_BUILD_DXILCONV=ON`
- `ENABLE_SPIRV_CODEGEN=ON`
- `SPIRV_BUILD_TESTS=ON`

The production DLL and real DXC interface ABI are preserved. A COM-shaped
`IDxcCompiler3` is still part of the DXC API; using that interface does
not require COM activation or ATL.

## Test matrix

| CTest | Source | Expected evidence |
| --- | --- | --- |
| `DXC.Behavior.PixelDXIL` | `smoke.hlsl` | Valid pixel shader compiled to a DXBC/DXIL container |
| `DXC.Behavior.PixelSPIRV` | `smoke.hlsl` | Valid pixel shader compiled to SPIR-V |
| `DXC.Behavior.ComputeDXIL` | `compute.hlsl` | Compute profile compiles to a DXIL container |
| `DXC.Behavior.ComputeSPIRV` | `compute.hlsl` | Compute profile compiles to SPIR-V |
| `DXC.Behavior.IncludeDXIL` | `include-test.hlsl` | Local HLSL include is resolved |
| `DXC.Behavior.MissingEntry` | `smoke.hlsl` | Missing entry point rejected, diagnostic present, no output artifact |
| `DXC.Behavior.InvalidSyntax` | `invalid-syntax.hlsl` | Invalid source rejected, diagnostic present, no output artifact |
| `DXC.Behavior.NativeApi` | `DxcApiBehavior.cpp` | Real DXC factory, compile result, output blob and error blob verified |

`RunShader.cmake` only invokes the actual `dxc` binary and validates
results. It does not transform or generate C++ test sources.

`DxcApiBehavior.cpp` links against the actual `dxcompiler` target and uses
`DxcCreateInstance`, `IDxcCompiler3`, `IDxcResult`, and
`std::unique_ptr` with a `Release` deleter. It verifies both compilation
success and a negative test through the public API.

## Scope, evidence and limitations

The original DXC Windows test cases, XML-driven HLSLExec GPU tests and
DXIL converter regressions are **not** represented as migrated or passing.
Their historic test semantics are not a correctness requirement for the new
architecture. New functional coverage can be expanded independently
against the public compiler API, intermediate containers, CLI and shader
diagnostics; full DXC validation still requires executing the resulting
CTests, checking runtime DLL staging, and reviewing any BCC64X errors.

The last uploaded build log predates this architecture replacement. This
repository change has not yet been compiled and executed under BCC64X:
**[not verified]**.

The production-specific BCC64X compatibility and DIA fixes are separate
from the independent test architecture. Their necessity must be judged by
the real compiler and library build, not by historical TAEF or ATL tests.
