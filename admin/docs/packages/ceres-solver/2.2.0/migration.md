# Ceres Solver 2.2.0: BCC64X integration

Status: registered, native build and consumer smoke pending (21 November 2026).

External dependencies: Eigen 5.0.1, glog 0.7.1, gflags 2.3.1.

Initial backend: EigenSparse enabled; SuiteSparse, CXSparse, LAPACK and CUDA disabled. Ceres 2.2.0 intentionally retains glog and gflags; Ceres 2.3 replaces them with Abseil. The independent native consumer must optimize a concrete least-squares residual and verify the numerical result.

Integration risks: Eigen 5 compatibility and the availability of an Eigen3 CMake package; dynamic export/import contract; dependency discovery. Optional SuiteSparse must be reviewed separately for CHOLMOD/SPQR licensing before activation. Do not mark PASS without native evidence.


## BCC64X Schur specialization and dependency correction

The 21 November failure in `libceres.dll` / `libceres-debug.dll` reported a missing `PartitionedMatrixView<2, Eigen::Dynamic, Eigen::Dynamic>` constructor. Ceres generates this explicit instantiation in `internal/ceres/generated/partitioned_matrix_view_2_d_d.cc`. It is excluded when `SCHUR_SPECIALIZATIONS=OFF` selects only the fully dynamic generated files. This is not an acceptable way to reduce compiler load.

The integration now explicitly enables `SCHUR_SPECIALIZATIONS=ON`, retains EigenSparse, and requests a fresh CMake configure so the old `CERES_RESTRICT_SCHUR_SPECIALIZATION` state cannot persist. The glog/gflags import namespace is consistent with the installed DLL package. All generated specializations must compile and link, or a specific BCC64X compile error must be documented and corrected without suppressing them. Native Release, Debug, and numerical consumer results remain pending.
