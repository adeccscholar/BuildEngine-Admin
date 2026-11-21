# Ceres Solver 2.2.0: BCC64X integration

Status: native integration completed (operator-reported); superseded by Ceres Solver 2.3.0 development snapshot (21 November 2026).

External dependencies: Eigen 5.0.1, glog 0.7.1, gflags 2.3.1.

Initial backend: EigenSparse enabled; SuiteSparse, CXSparse, LAPACK and CUDA disabled. Ceres 2.2.0 intentionally retains glog and gflags; Ceres 2.3 replaces them with Abseil. The independent native consumer must optimize a concrete least-squares residual and verify the numerical result.

Integration risks: Eigen 5 compatibility and the availability of an Eigen3 CMake package; dynamic export/import contract; dependency discovery. Optional SuiteSparse must be reviewed separately for CHOLMOD/SPQR licensing before activation. Do not mark PASS without native evidence.


## BCC64X Schur specialization and dependency correction

The 21 November failure in `libceres.dll` / `libceres-debug.dll` reported a missing `PartitionedMatrixView<2, Eigen::Dynamic, Eigen::Dynamic>` constructor. Ceres generates this explicit instantiation in `internal/ceres/generated/partitioned_matrix_view_2_d_d.cc`. It is excluded when `SCHUR_SPECIALIZATIONS=OFF` selects only the fully dynamic generated files. This is not an acceptable way to reduce compiler load.

The integration now explicitly enables `SCHUR_SPECIALIZATIONS=ON`, retains EigenSparse, and requests a fresh CMake configure so the old `CERES_RESTRICT_SCHUR_SPECIALIZATION` state cannot persist. The glog/gflags import namespace is consistent with the installed DLL package. All generated specializations must compile and link, or a specific BCC64X compile error must be documented and corrected without suppressing them. Native Release, Debug, and numerical consumer results remain pending.


## Native memory failure (21 November 2026)

With all Schur specializations enabled, BCC64X 20.1.7 compiled numerous `partitioned_matrix_view_*.cc` files, then emitted 31 `LLVM ERROR: out of memory` crashes while compiling in parallel, including ordinary Ceres sources. Windows reported `0x5AF`: the paging file was too small to complete the operation. This is an environment commit-limit exhaustion, not evidence that Schur algorithms are incorrect. No solver feature is disabled and no generated specialization is removed. This package now sets `CMAKE_BUILD_PARALLEL_LEVEL=1` (only for this library's native CMake build), leaving the BuildEngine scheduler unchanged. If memory is still insufficient at reduced concurrency, provision a larger Windows paging file and retry. Successful native Release/Debug and independent optimization smoke remain unverified.


## Native link target correction (21 November 2026)

The full Schur specialization compilation now reaches the native DLL link in both configurations. The remaining linker error is `lld: error: unable to find library -lgflags`. Ceres 2.2.0 upstream appends the unqualified string `gflags` to `CERES_LIBRARY_PUBLIC_DEPENDENCIES`, which is wrong for the installed namespaced CMake import. A source-scoped patch makes its package discovery accept `gflags::gflags` and links this actual imported target rather than generating `-lgflags`. The patch does not change Ceres, glog, or gflags functionality. The independent numerical consumer now emits the required `SMOKE|CHECK|...` and `SMOKE|RESULT|...` records; it still verifies a usable solution and the parameter converging to 3. The native Release/Debug DLL link and consumer must be rerun before claiming PASS.


## Installed package consumer discovery correction (21 November 2026)

The native Release and Debug DLL builds, installation, publication and documentation have now passed. The remaining independent smoke fails during CMake configure at `CeresConfig.cmake:85`. Upstream `cmake/CeresConfig.cmake.in` still verifies `TARGET gflags`, although the earlier installed-package fix uses `gflags::gflags` as an imported target. The version-scoped source patch now also accepts that namespaced target during package consumption. This preserves the required gflags dependency; it does not bypass the package check or alter the numerical solver. Native installed-package consumer validation remains pending.


## Smoke-only recovery (21 November 2026)

The later library timestamp bump was incorrect for a consumer-only failure: a library timestamp is shared by all FSM scopes and forced Source, Build, Install and documentation to run again. Restore the last successfully built library timestamp (2026-11-21T10:00:00Z) and the original two-hunk source patch. The previously appended third hunk against `cmake/CeresConfig.cmake.in` was not applicable to an already patched source workspace and caused the `gflags-patch-check` failure. The versioned source patch execution is now idempotent: the helper accepts the fully original source (apply once) or the fully patched source (skip) and rejects mixed/unexpected trees. The independent 2.2 consumer loads `gflags::gflags` and supplies the legacy imported `gflags` target required by the installed 2.2-era CeresConfig; this is explicitly a compatibility bridge, not proof that the installed CeresConfig is independently correct for arbitrary external consumers. Use BuildEngine `--make --refresh=smoke --lib=ceres-solver --libversion=2.2.0` to retry the consumer without requesting a library rebuild, provided preceding scope state is still valid.

## Bessel public header compatibility (21 November 2026)

The library's DLLs, installation, publish and Doxygen previously completed successfully, but the independent C++23 consumer failed compiling the installed public `include/ceres/jet.h:878`: `j0` was undeclared. This is a POSIX runtime name used unconditionally by the upstream 2.2.0 header, not proof of incorrect compiler classification. BCC64X is Clang/EMBT on Windows, not MinGW.

Ceres 2.2.0 retains its complete solver and Schur specialization implementation. A version-specific, idempotent patch is applied **after the normal CMake install** to the package's public `include/ceres/jet.h`. On `_WIN32`, its existing BesselJ0/BesselJ1/BesselJn wrappers call the documented Microsoft/UCRT `_j0`, `_j1`, and `_jn` functions, with explicit `<math.h>`. Other platforms retain the original wrappers. This is a runtime API correction, not a change of compiler identity or disabled functionality.

The published package consumes the patched header, and the independent numerical smoke now checks the original optimization plus Bessel values at zero and parity for negative arguments/orders. Use `BuildEngine --make --lib=ceres-solver --libversion=2.2.0 --refresh=install --config=Release` to reinstall and republish the existing build without recompiling the Ceres DLL; `--refresh=smoke` then targets only the consumer. Native BCC64X compilation, linking and runtime validation of the patched public header remain pending.

## Final native integration and supersession — 21 November 2026

**Outcome: operator-confirmed native BCC64X acceptance.** The numerical installed-package consumer was corrected to use the actual `ceres-consumer.exe` target, after a real Ceres optimization test and public-header Bessel checks had been implemented. The earlier integration gates (source, full Schur specialization build, Debug and Release DLLs, install, license metadata, publish, documentation and smoke) were subsequently reported completed by the operator. The final 134/134 raw FSM result has not been attached to this documentation update, so that statement is recorded explicitly as operator confirmation rather than an independently captured build transcript.

This version is now **supersededBy 2.3.0** in the library catalog. Supersession is descriptive only: Ceres 2.2.0 remains installable, its historical state and its glog/gflags dependency chain are retained, and any explicitly pinned consumer keeps its original dependency. The newer 2.3.0 is an immutable upstream development snapshot, not a published release tag. Its independent Abseil-based integration is the default active Ceres package for new showcases.

The project does not resolve the old BCC64X `j0/j1/jn` mismatch by disabling automatic differentiation or Schur specializations. A version-scoped header patch selects the Windows UCRT `_j0/_j1/_jn` functions, retaining the complete mathematical interface.
