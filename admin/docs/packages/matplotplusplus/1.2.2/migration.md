# Matplot++ 1.2.2 — BCC64X Integration and Migration

## Purpose

Native mathematical plotting with 2D/3D figures and graphical output.

## Package contract

| Property | Value |
| --- | --- |
| Coordinate | `matplotplusplus@1.2.2` |
| Artifact type | **Shared Library (planned)** |
| Language | C++ |
| License | MIT |
| Upstream | [Project repository](https://github.com/alandefreitas/matplotplusplus) |
| Native validation | **Not verified** |

## Dependency and extension decisions

Independent mathematical plotting library. Requires examination of its plot backend and transitive graph; CMake offers MATPLOTPP_BUILD_SHARED_LIBS and backend-specific options. No successful BCC64X build is claimed.

No extension relationship.

```mermaid
flowchart TD
    BACKEND[Native plotting backend] --> MATPLOT[Matplot++ 1.2.2 - shared]
```

## BCC64X policy

All native C/C++ code must be consumed by the BCC64X Clang toolchain, not an MSVC or MinGW binary. Interface libraries install headers without DLL/ILIB checks. Shared libraries require matching Debug and Release DLL/import-library artifacts, complete exported API coverage, license/SBOM and a linked consumer smoke.

## Smoke-test contract

The repository includes `admin/smokes/matplotplusplus/consumer`, where a C++23 consumer verifies a real library operation. A committed test definition is not evidence of a successful test. Rendering, SDL2/OpenGL or VCL integration remain separate functional demonstration gates.

## Status and follow-up

**Not verified:** source acquisition, native BCC64X build, installation, linked smoke, runtime graphics backend and package distribution. No successful build, installed artifact or package export is asserted.
