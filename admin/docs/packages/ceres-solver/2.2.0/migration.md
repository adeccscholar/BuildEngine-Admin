# Ceres Solver 2.2.0: BCC64X integration

Status: registered, native build and consumer smoke pending (21 November 2026).

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
