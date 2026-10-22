# VTK 9.6.2 migration

## Native SMP backend integration
- BuildEngine enables `VTK_SMP_ENABLE_TBB=ON` and `VTK_SMP_ENABLE_OPENMP=ON` and selects `VTK_SMP_IMPLEMENTATION_TYPE=TBB`.
- External, version-pinned dependencies are oneTBB 2023.1.0 and LLVM OpenMP 20.1.7, using installed shared runtimes with BCC64X C/C++ builds.
- VTK's existing Release consumer smoke **passed on 2026-10-22**, following the backend integration, and VTK reached FSM `ready`. The smoke checks the generated backend definitions for TBB and OpenMP and runs the VTK core/data-model/filter pipeline.
- The smoke is a configuration-and-basic-runtime check, **not** a benchmark and not proof that both SMP implementations execute in the same computation.

## Toolchain and Windows resources
- The BuildEngine contract passes `CB_WINDOWS_RC={Tool:windows-rc}` so resource compilation receives SDK include paths without depending on Visual Studio.
- C and C++ compilation and DLL linking remain BCC64X. No source copying or static replacement of oneTBB/OpenMP runtimes.

## Validation scope
- Preserve existing VTK CommonCore, CommonDataModel and FiltersCore package/smoke checks.
- Release consumer evidence: `vtk/9.6.2 smoke:consumer:Release PASS 8.6s` and `vtk/9.6.2 ready PASS`.
- Debug SMP consumer and performance improvement have not yet been demonstrated.
