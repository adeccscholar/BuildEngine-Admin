# OpenCV 5.0.0 migration

## Native parallel backends
- Built with `WITH_TBB=ON`, `BUILD_TBB=OFF`, `WITH_OPENMP=ON`.
- The BuildEngine library contract declares version-pinned dependencies on oneTBB 2023.1.0 and LLVM OpenMP 20.1.7, with the BCC64X C/C++ toolchain and dynamic libraries.
- The Release consumer smoke **passed on 2026-10-22**. Its `cv::getBuildInformation()` output reported `Parallel framework: TBB (ver 2023.1 interface 12190)`, and the functional core/imgproc/imgcodecs/dnn check passed.
- TBB is the confirmed active OpenCV native parallel framework. `WITH_OPENMP=ON` enables configuration and compilation support; the smoke does **not** establish that OpenMP is simultaneously the active backend.
- The consumer uses `SMOKE|INFO|parallel-framework` and fails if the native TBB backend is absent. Do not replace this with an unconditional PASS.

## Windows resource compiler
- Windows SDK `rc.exe` is resolved by the managed `windows-rc` tool and passed via `CB_WINDOWS_RC` to the BCC64X CMake toolchain.
- This supplies SDK `um` and `shared` include paths required for `vs_version.rc` / `winver.h`. No Visual Studio installation is required.

## Validation and restrictions
- Preserve DLL/import-library packaging and per-configuration dependencies.
- Preserve all existing OpenCV modules and consumer checks; do not introduce bundled TBB or another OpenMP runtime.
- Evidence: OpenCV Release smoke validation log dated 2026-10-22 18:58, result PASS (2 checks, 0 failed).
- Debug native-backend smoke and performance speedups are **not** established by this evidence.
