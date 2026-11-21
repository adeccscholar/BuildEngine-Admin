# Ceres Solver 2.2.0: BCC64X integration

Status: registered, native build and consumer smoke pending (21 November 2026).

External dependencies: Eigen 5.0.1, glog 0.7.1, gflags 2.3.1.

Initial backend: EigenSparse enabled; SuiteSparse, CXSparse, LAPACK and CUDA disabled. Ceres 2.2.0 intentionally retains glog and gflags; Ceres 2.3 replaces them with Abseil. The independent native consumer must optimize a concrete least-squares residual and verify the numerical result.

Integration risks: Eigen 5 compatibility and the availability of an Eigen3 CMake package; dynamic export/import contract; dependency discovery. Optional SuiteSparse must be reviewed separately for CHOLMOD/SPQR licensing before activation. Do not mark PASS without native evidence.
