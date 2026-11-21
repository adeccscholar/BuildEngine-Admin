# glog 0.7.1

Status: registered, native smoke pending. Native shared-only BuildEngine package with external gflags 2.3.1. Validate exported CMake target and DLL imports using an independent logging consumer. Dependency for Ceres Solver 2.2.0.


## Transitive gflags export correction (21 November 2026)

The native glog build uses `GFLAGS_USE_TARGET_NAMESPACE=ON` while discovering gflags 2.3.1. This ensures upstream glog's `target_link_libraries(glog PUBLIC gflags::gflags)` exports a CMake target with transitive include directories instead of an unqualified library name. The independent smoke retains only `target_link_libraries(glog-consumer PRIVATE glog::glog)` and fails if the installed glog target does not publicly propagate `gflags::gflags`. A fresh configure and a raised library timestamp invalidate the previous build state; no logging functionality is disabled. Native smoke rerun required.
