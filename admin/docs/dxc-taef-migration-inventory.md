# DirectXShaderCompiler v1.9.2609 - TAEF / WEX migration inventory

Status: source inventory complete for GitHub code-search matches; test migration **INCOMPLETE**.

Source: https://github.com/microsoft/DirectXShaderCompiler/tree/v1.9.2609

## Build-time hard dependencies (4)

1. `projects/dxilconv/unittests/CMakeLists.txt` - `find_package(TAEF REQUIRED)`, TAEF libraries, DLL test project. Already redirected by BuildEngine's portable CTest patch; original assertions still await migration.
2. `tools/clang/unittests/HLSL/CMakeLists.txt` - `find_package(TAEF REQUIRED)`, TAEF DLL test project `ClangHLSLTests`; separate DiaSDK/D3D12 dependencies.
3. `tools/clang/unittests/HLSLExec/CMakeLists.txt` - `find_package(TAEF REQUIRED)`, `ExecHLSLTests` DLL, and D3D12 dependencies.
4. `tools/clang/unittests/HLSLTestLib/CMakeLists.txt` - `find_package(TAEF REQUIRED)` under `WIN32` even before the other two suites build. The source uses shared WEX test helpers.

The historical `FindTAEF.cmake` module is an additional resolver, not a fifth `find_package` caller.

## Other migration surfaces

- LLVM/LIT adapter: `utils/lit/lit/formats/taef.py`, `utils/lit/lit/formats/__init__.py`, `utils/llvm-lit/llvm-lit.in`
- LIT registration and site configuration: `projects/dxilconv/test`, `tools/clang/test` and their `taef`/`taef_exec` directories and `lit.local.cfg` subtrees
- WEX abstraction and test utility source: `include/dxc/Test/WEXAdapter.h`, `include/dxc/Test/HlslTestUtils.h`, and `tools/clang/unittests/HLSLExec` sources
- Test fixtures across `tools/clang/unittests/HLSL` use `BEGIN_TEST_CLASS`, `TEST_METHOD`, `TEST_F`, `WEX::Logging`, and/or `WEX::TestExecution` even when the word TAEF is absent
- Tooling and documentation: `utils/hct`, `docs/BuildingAndTestingDXC.rst`, and Visual Studio `.vcxproj.user` files

## File inventory: 38 TAEF keyword hits

```text
cmake/modules/FindTAEF.cmake
docs/BuildingAndTestingDXC.rst
include/dxc/Test/HlslTestUtils.h
include/dxc/Test/WEXAdapter.h
projects/dxilconv/test/CMakeLists.txt
projects/dxilconv/test/taef/lit.cfg
projects/dxilconv/test/taef/lit.site.cfg.in
projects/dxilconv/unittests/CMakeLists.txt
projects/dxilconv/unittests/dxilconv-tests.vcxproj.user.in
tools/clang/test/CMakeLists.txt
tools/clang/test/CodeGenHLSL/lit.local.cfg
tools/clang/test/DXILValidation/lit.local.cfg
tools/clang/test/HLSL/lit.local.cfg
tools/clang/test/HLSLFileCheck/lit.local.cfg
tools/clang/test/taef/lit.cfg
tools/clang/test/taef/lit.site.cfg.in
tools/clang/test/taef_exec/DownloadWarp.cmake
tools/clang/test/taef_exec/lit.cfg
tools/clang/test/taef_exec/lit.site.cfg.in
tools/clang/unittests/CMakeLists.txt
tools/clang/unittests/HLSL/CMakeLists.txt
tools/clang/unittests/HLSL/ClangHLSLTests.vcxproj.user.txt
tools/clang/unittests/HLSLExec/CMakeLists.txt
tools/clang/unittests/HLSLExec/ExecHLSLTests.vcxproj.user.txt
tools/clang/unittests/HLSLExec/HlslExecTestUtils.h
tools/clang/unittests/HLSLExec/LongVectors.cpp
tools/clang/unittests/HLSLExec/ShaderOpTest.cpp
tools/clang/unittests/HLSLExec/TableParameterHandler.h
tools/clang/unittests/HLSLTestLib/CMakeLists.txt
utils/hct/VerifierHelper.py
utils/hct/hctbins.cmd
utils/hct/hctbuild.cmd
utils/hct/hctdb_test.py
utils/hct/hctstart.cmd
utils/hct/hcttest.cmd
utils/lit/lit/formats/__init__.py
utils/lit/lit/formats/taef.py
utils/llvm-lit/llvm-lit.in
```

Search methodology: repository-wide GitHub code search for `TAEF` on the default branch, cross-checked for `find_package(TAEF`, `TAEF_LIBRARIES`, `TAEF_INCLUDE_DIRS`, `TaefTest`, `taef_exec`, `WEX::TestExecution`, `WEX::Logging`, `BEGIN_TEST_CLASS`, and `TEST_METHOD(`. Upstream release tag used for examining build files: `v1.9.2609`. The list is the observed search inventory, not a proof that every WEX usage has been ported.

## Required migration work (do not suppress coverage)

1. Port shared `HLSLTestLib` support and WEX logging/assertion layer to portable test infrastructure.
2. Port HLSL compiler/API unit cases with the same fixtures, expected results and failure semantics.
3. Port D3D12 shader-execution tests in `HLSLExec` with independent device-capability reporting, preserving original acceptance requirements.
4. Port six automated DXIL converter batch/regression methods plus the original explicitly ignored manual method (keep manual status).
5. Refactor LIT configuration and its `TaefTest` adapters to invoke portable test runners, preserving test discovery and reporting.
6. Compare per-suite original/ported case counts, fixture sets and pass/fail semantics before declaring any equivalence.
7. Keep all unported test categories marked **INCOMPLETE**, not PASS.

No TAEF runner or host `te.exe` is required by the initial BuildEngine CTest shader checks. This does **not** establish equivalent coverage of all upstream tests.
