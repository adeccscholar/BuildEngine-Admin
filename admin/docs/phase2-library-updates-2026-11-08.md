# Library Update Checklist: Phase 2 (2026-11-08)

[TOC|Content]

## Goal and scope

Phase 2 updates testing and parallel-computing libraries for native Windows x64 builds with RAD Studio 13 / BCC64X. The authoritative catalog is `admin/build-libraries.xml`. Verification also includes BuildEngine restart and repository synchronization against the updated BuildEngine-Common DLL. Successful XML or patch reconciliation is **not** evidence of successful BCC64X builds or CTest runs.

## Newly active versions

| Library | Previous | Active | Upstream pin | Consumer |
| --- | --- | --- | --- | --- |
| Catch2 | 3.16.0 | **3.16.1** | v3.16.1, 08092139210881f01a34e60c5f2fd6afa0ea4024 | Catch2 Release/Debug SelfTests + consumer smoke |
| GoogleTest/GoogleMock | 1.17.0 | **1.18.0** | v1.18.0, 063de7e9578f82b369302001269680b4b1553359 | GTest-GMock consumer smoke |
| oneTBB | 2023.1.0 | **2023.2.0** | v2023.2.0, 41158a44ab3fb7ac5df7f99ef82626c87607447f | oneTBB smoke and terminate smoke |

Previous coordinates remain traceable and are marked with `supersededBy`. Each new catalog entry receives an independent timestamp.

The active dependency graph also references `onetbb@2023.2.0` from **VTK 9.6.2** and **OpenCV 5.0.0**. Both therefore receive changed timestamps and require fresh builds and tests. Previous consumer PASS reports must not be reused to validate the updated oneTBB dependency.

The three version-specific BCC64X-oneTBB patches were checked against the textual hunk context of upstream tag v2023.2.0 and placed in `admin/patches/onetbb/2023.2.0/`:

- `bcc64x-profiling-widechar.patch`
- `bcc64x-clang-exports.patch`
- `bcc64x-terminate-conformance.patch`

The existing Catch2 BCC64X protections for `isnan` and the UTF-8 execution code page are retained. GoogleTest continues to use the existing BCC64X CMake target fix.

## Older dependency pins outside phase 2

**Nine** active dependency references deliberately continued to use superseded coordinates. Migrating them was **not** part of this commit, to avoid simultaneously rebuilding the recently tested PDFium/Skia environment.

| Older coordinate | Active version | Consumers |
| --- | --- | --- |
| libjpeg-turbo 3.2.0 | 3.2.1 | Leptonica, libtiff, Skia, QPDF, PoDoFo, Poppler, PDFium |
| Expat 2.9.0 | 2.10.0 | TECkit |
| libzip 1.11.4 | 1.12.1 | OpenXLSX |

## Evidence required for the next BuildEngine run

- [x] Verified upstream Git tags and exact commit identifiers
- [x] Compared oneTBB patch hunks with upstream v2023.2.0
- [x] Prepared three new version records and `supersededBy` declarations
- [x] Updated VTK and OpenCV dependency pins and timestamps
- [x] Inspected the active dependency graph for remaining old-version references
- [ ] Rebuilt BuildEngine-Common with repository mutex and flushed Git process logs
- [ ] Confirmed automatic BuildEngine restart and Admin fetch under actual Windows process conditions
- [ ] Catch2 3.16.1 Debug/Release: CMake, build, CTest, install and consumer
- [ ] GoogleTest 1.18.0 Debug/Release: build, install and consumer
- [ ] oneTBB 2023.2.0 Debug/Release: patches, build, CTest, install, consumer and terminate smoke
- [ ] VTK 9.6.2 / OpenCV 5.0.0: regression against oneTBB 2023.2.0
- [ ] Publication, metadata, license evidence and SBOM checks

**Status at the time of this checklist:** Native build and runtime verification **[not verified]**.
