# BuildEngine: nine-library integration plan — 2026-11-19

**Status:** planned integrations, not registered as completed builds.
**Baseline catalog:** 85 logical libraries / 120 versioned integration records.
**Compiler:** C++Builder 13 / BCC64X Clang 20.1.7 on Windows x64, C++23, Ninja, separately validated Debug/Release.
**Artifact policy:** dynamic DLLs for independently linked runtime packages; interface/header-only packages require consumer compilation and runtime verification, not artificial DLLs. No downloaded MSVC/MinGW binaries. Keep upstream source versions, local patches, SBOMs and licenses independently identifiable.

## Global acceptance sequence

1. Verify a precise upstream release tag, source checksum, license and build options before registering the version.
2. Register source and dependency closure only in `admin/build-libraries.xml`; mark planned until real native compilation has passed.
3. Use managed BuildEngine dependencies instead of vendored copies whenever their API/ABI/license and ownership are suitable. Do not change existing producers solely to satisfy a new package.
4. Build with BCC64X Clang, Ninja, controlled 64-bit DLL exports/imports and variant-correct Debug and Release install locations. Do not use MSVC, MinGW GCC or precompiled Windows distributions.
5. Add a small native smoke under `admin/smokes/<id>/consumer`; check real API behavior, errors, and emitted `SMOKE|CHECK|` plus `SMOKE|RESULT|` records.
6. Add a separate optional consumer application or benchmark in BuildEngine-Tests. Validate CTest and actual runtime DLL closure.
7. Publish CycloneDX SBOM, source commit/checksum, license, direct/transitive dependencies, Doxygen and English migration notes. Verify the Server representation.
8. Import applicable CVEs and any local mitigations as dated assessments via `admin/security/assessments.xml` into PostgreSQL; never silently erase provider findings.
9. Only declare a package ready after a full FSM pass, tests, installation and SBOM/documentation gates.

## Proposed version and technical scope

| Logical ID | Candidate | First phase | Managed prerequisites | Key acceptance risk |
| --- | --- | --- | --- | --- |
| `hnswlib` | v0.9.0 (stable; v0.10.0-rc.2 is pre-release) | DeckKernel nearest-neighbor benchmark | C++23 interface; OpenCV remains separate input feature producer | Header-only algorithm, multithreading, serialization and memory limits |
| `zxing-cpp` | v3.1.1 (stable; 3.2.0-rc1 pre-release) | Decoder-only C++ library | Optional zint excluded when writer is unused; Unicode options explicit | Avoid default bundled zint; validate reader-only APIs and barcode ROI workloads |
| `ceres-solver` | **exact release to verify before registration** | Core nonlinear least squares | Eigen 5.0.1; optional sparse/BLAS/SuiteSparse backends disabled until separately provisioned | Eigen ABI, Windows DLL export, compiler and numerical stability |
| `openexr` | 3.5.4 | HDR encode/decode with external compression | Managed zlib 1.4, zstd 1.5.7, libdeflate 1.26; stage separate Imath and OpenJPH versions if required | 3.5.3/3.5.4 security fixes, internal-vs-system libraries, ABI |
| `assimp` | 6.0.6 | Selected 3D import formats | zlib from managed catalog; other optional dependencies explicitly audited | Untrusted 3D inputs, importer enumeration, native DLL boundary |
| `jolt-physics` | 5.6.0 | CPU-only 3D rigid body simulation | Standalone core; managed Vulkan only in separate future GPU integration | C++ ABI, SIMD policy, determinism, thread/allocator interfaces |
| `cgal` | 6.2.1 | Geometry kernels, points/triangulation | Managed Boost 1.92.0; GMP/MPFR require separate source builds and licensing review where demanded | Exact arithmetic dependencies, LGPL/GPL/commercial component obligations |
| `libigl` | 2.6.4 | Header-only/core mesh operations | Managed Eigen 5.0.1; CGAL/Embree/etc optional and disabled initially | Eigen 5.0.1 compatibility and upstream MSH parsing security change |
| `xtensor` | **exact release to verify before registration** | Header-only tensors and numerical expressions | xtl/xsimd dependencies must be pinned, licensed and integrated or explicitly disabled | Template compilation, expression lifetimes, ABI and C++23 smoke |

The version suggestions correspond to upstream evidence available on 19 November 2026; no proposed binary has been compiled by this plan. Where an exact version is not verified, registration must stop rather than select a guessed number.

## Implementation phases and acceptance criteria

### Phase A: DeckKernel and small interface packages

**hnswlib:** build a native C++23 consumer indexing 10,000 deterministic 128-dimensional vectors, persist/reload the index, validate top-k recall against brute force and record memory usage, steady-state latency and rebuild time. For DeckKernel, take the existing OpenCV artwork descriptors (or a separately verified embedding), and compare PostgreSQL metadata candidate narrowing + OpenCV matching against HNSW-assisted matching. No replacement until recall, worst-case latency and false matches improve on the same dataset. Detect accidental OpenMP or SIMD dependency changes.

**ZXing-C++:** configure reader-only decoding (no Zint writer dependency), static sample fixtures and real card camera ROIs; compare code detection on crop, rotation and blur variants. Treat the library as a separate optional mode, not as a supposed direct fix for artwork or border detection. Assert no hidden PNG/JPEG decoding library bundled into the DLL unless explicitly documented.

**xtensor:** register as an interface package; pin xtl and determine xsimd requirements from the selected release. Benchmark numerically stable transformations against Eigen rather than infer performance from API style. No redundant new tensor DLL.

### Phase B: numerical geometry and optimization

**Ceres Solver:** source-first feasibility adapter, managed Eigen, minimal dense trust-region optimizer with a known solution, robust loss function and small camera-reprojection example. Keep BLAS, SuiteSparse, CXSparse and gflags/glog optional until they are explicitly present in the BuildEngine graph. Verify Debug/Release runtime and solver accuracy under BCC64X rather than linking against a foreign compiler output.

**libigl:** register core header-only algorithms against managed Eigen 5.0.1. Check AABB, mesh normals, signed/unsigned distances and a simple intersection query. Disable optional Embree, CGAL, Python, GLFW and external tutorials in the initial build. Retain source provenance for any routines under different license conditions.

**CGAL:** start from the core exact/inexact predicates and triangulation consumer. The required CGAL license selection and GMP/MPFR build/library obligations are explicit gates, not mere metadata. If required, introduce GMP and MPFR as independent managed packages with native Windows builds and importer-ready SBOMs before CGAL. Test numerical degeneracies, exact predicate reproducibility and exception paths.

### Phase C: game physics and asset interchange

**Jolt Physics:** build native 3D simulation CPU engine first with DX12/Vulkan/Metal compute features off. Consumer must simulate a falling body, impulses, contact constraints and a deterministic re-run under a fixed timestep. Confirm import/export symbols, Debug/Release behavior and relevant CPU feature gates. Add an SDL2 or SDL3 interactive showcase only after the core smoke passes. Box2D remains an independent 2D physics library.

**Assimp:** create a shared library with only a documented import-format shortlist in phase one (for example OBJ, glTF/GLB). All enabled parsers are part of the security attack surface; avoid bundled foreign dependencies. Test malformed input, large declared counts, and file/texture path handling; measure parser memory and errors. Preserve optional format growth as separate revisions.

**OpenEXR:** start from upstream security-updated 3.5.4. Make Imath and OpenJPH independent producers if their actually enabled features require them; prefer current managed zlib, zstd and libdeflate rather than vendored compression code. Disable unused optional codec paths only when explicitly recorded. Run RGB/half and float HDR roundtrip, edge dimensions, truncated file rejection, multipart/deep optional-feature gates, and fuzz-style malformed input. Record upstream security advisories from 3.5.3 and 3.5.4 in the security baseline.

### Phase D: delivery and release

Every package must provide a reproducible source checksum, CMake/Ninja command, exact system dependencies, variant output paths, license and CycloneDX SBOM. Document a security assessment for identified CVEs only after source/version or compiled-feature evidence is known. Add a dedicated BuildEngine-Tests showcase to demonstrate meaningful use; do not mark planned packages built on registration alone.

## Architecture and rollout decision

- **First to prototype:** hnswlib, ZXing-C++, libigl, Jolt Physics; these have bounded native feasibility and visible consumer outcomes.
- **Next:** OpenEXR 3.5.4, Assimp 6.0.6 and Ceres Solver, with their dependency gates.
- **Longer dependency chain:** CGAL (GMP/MPFR and licensing); xtensor/xtl/xsimd compatibility after selecting a verified release.
- **Do not schedule security upgrades of existing BuildEngine library versions before the planned weekend review.** XML assessments and PostgreSQL/Server integration are the immediate priority.

## Upstream references

- hnswlib: https://github.com/nmslib/hnswlib/releases
- ZXing-C++: https://github.com/zxing-cpp/zxing-cpp/releases
- Ceres Solver: https://github.com/ceres-solver/ceres-solver/releases
- OpenEXR: https://openexr.com/en/latest/news.html
- Assimp: https://github.com/assimp/assimp/releases
- Jolt Physics: https://github.com/jrouwe/JoltPhysics/releases
- CGAL: https://github.com/CGAL/cgal/releases
- libigl: https://github.com/libigl/libigl/releases
- xtensor: https://github.com/xtensor-stack/xtensor/releases
