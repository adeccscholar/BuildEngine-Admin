# Ceres Solver 2.3.0 development snapshot — BCC64X

**Status:** registered for independent native Release/Debug builds. No successful Ceres 2.3 consumer build has been observed.

## Source identity

The Ceres upstream GitHub repository currently has **no published 2.3.0 release tag**. This package is a deliberate, immutable development snapshot of commit `c39d3cdced6051690c7992ade0f6566fa822ccd2`, whose source `include/ceres/version.h` declares `2.3.0`. It must not be represented as an official released archive. The version identity must retain this commit provenance in metadata and SBOM.

[Upstream source commit](https://github.com/ceres-solver/ceres-solver/commit/c39d3cdced6051690c7992ade0f6566fa822ccd2)

## Dependency and solver policy

- Direct external [Abseil 20260817.0](../../abseil/20260817.0/migration.md); no direct glog or gflags dependency.
- Managed header-only Eigen 5.0.1 exports an `Eigen3::Eigen` CMake target from the installed package root.
- EigenSparse enabled; SuiteSparse and LAPACK are not enabled for the first BCC64X build.
- CUDA, examples, benchmarks, documentation and upstream tests disabled initially.
- Production artifact must be a native BCC64X DLL with a real import library.

Ceres 2.2.0 remains a separate versioned package using glog 0.7.1 and gflags 2.3.1. Both should be built and evaluated independently. The 2.3 consumer smoke solves a concrete one-parameter nonlinear least-squares problem.

## Verification gates

The GitHub source tree is *not* a release-qualified package. Check Eigen 5 API compatibility, CMake imported Abseil targets and installed package exports, DLL ABI, independent numerical consumer, and provenance/SBOM. Mark PASS only after the native FSM run.


## Build correction (21 November 2026)

Restore `WITH_SCHUR_SPECIALIZATIONS=ON` and request `cmake --fresh` when the FSM invalidates this version. The earlier forced `OFF` reduced the solver configuration and is reverted. The separate configure error at `internal/ceres/CMakeLists.txt:279` concerns the `ceres_static` dependency link interface, including Abseil targets. The provided summary omits the full CMake diagnostic; no speculative removal of targets or solver functionality is permitted. Inspect the full `01-cmake-configure.log` and retain the independent Abseil DLL contract. Status: still requires native verification.


## CMake generator recursion correction (21 November 2026)

The complete CMake 4.1.1 configure log proves that `target_link_libraries(ceres_static INTERFACE $<TARGET_PROPERTY:ceres,INTERFACE_LINK_LIBRARIES>)` at `internal/ceres/CMakeLists.txt:279` triggers recursive evaluation of the transitive link interface. The pinned development snapshot now receives a version-bound source patch replacing only that expression with the existing `CERES_LIBRARY_PUBLIC_DEPENDENCIES` plus `Eigen3::Eigen`. Its private dependencies, full Schur specializations, and original solver configuration are preserved. Because both configurations share the constrained Windows commit budget with Ceres 2.2.0, this package also sets `CMAKE_BUILD_PARALLEL_LEVEL=1` for its CMake builds. Clean native DLL/consumer validation is still required.
