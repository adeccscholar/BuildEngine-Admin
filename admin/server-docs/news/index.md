# BuildEngine — Development News

[TOC|Content]

A chronological engineering account of BuildEngine's growth from an August BCC64X feasibility study to a fully versioned third-party library platform. Entries cover library acquisition, compiler and build-system adaptations, tooling, tests, integration challenges and the engineering decisions that made the platform reproducible.

## Timeline (newest first)

- **[2026-11-10](/news/2026-11-10/)** — 95 library documentation pages, transitive Mermaid graphs, news routes and Skia DLL review
- **[2026-11-09](/news/2026-11-09/)** — VTK 9.7.2, OpenCV 5.1, new tools and libarchive, PDFium, libtiff and OpenXLSX corrections
- **[2026-11-08](/news/2026-11-08/)** — Catch2, GoogleTest, oneTBB, PoDoFo, Poppler, Tesseract and other library upgrades
- **[2026-11-07](/news/2026-11-07/)** — ACE/TAO build parallelism and Windows libpq interfaces
- **[2026-10-30](/news/2026-10-30/)** — Brotli 1.2.1 and the OpenSSL resource compiler
- **[2026-10-28](/news/2026-10-28/)** — MariaDB Connector/C 3.4.11 and import libraries
- **[2026-10-25](/news/2026-10-25/)** — WIL, Tesseract, Leptonica, OpenCV Contrib and WinRT
- **[2026-10-22](/news/2026-10-22/)** — Unified integration tests migrated from the engine into BuildEngine-Tests, including PDFium and CompareTest
- **[2026-10-21](/news/2026-10-21/)** — oneTBB 2023.1.0, LLVM OpenMP, CMake resource tooling and dependency graphs
- **[2026-10-20](/news/2026-10-20/)** — PDFium/Abseil DLL boundaries, libpq ABI, resource tooling and Debug consumers
- **[2026-10-15](/news/2026-10-15/)** — XZ, Expat, libarchive, Abseil, Mermaid and documentation tools
- **[2026-09-25](/news/2026-09-25/)** — Dynamic BCC64X runtime and PoDoFo DLL consumers
- **[2026-09-24](/news/2026-09-24/)** — BCC64X Meson bootstrap, PostgreSQL source repairs and narrowed libpq scope
- **[2026-09-20](/news/2026-09-20/)** — Restored BCC64X CMake baseline and ICU VCXPROJ bootstrap documentation
- **[2026-09-09](/news/2026-09-09/)** — Managed Graphviz documentation tooling and pinned version preservation
- **[2026-09-05](/news/2026-09-05/)** — Skia integration repairs, GN patches, wrapper removal and reverted cleanup
- **[2026-09-02](/news/2026-09-02/)** — Reproducible Meson and managed Python source bootstraps
- **[2026-08-31](/news/2026-08-31/)** — Xerces-C, Doxygen 1.18, license metadata, SBOM and ACE/TAO ports
- **[2026-08-30](/news/2026-08-30/)** — Boost 1.92 DLL stream and serialization limits
- **[2026-08-29](/news/2026-08-29/)** — curl fixes, rebuilds, resets and libzip
- **[2026-08-28](/news/2026-08-28/)** — curl 8.21, installed-consumer smokes, XZ/libarchive and zlib variants
- **[2026-08-27](/news/2026-08-27/)** — NASM for OpenSSL and removal of the zstd static product
- **[2026-08-26](/news/2026-08-26/)** — OpenSSL, Perl, gmake and versioned dependency contracts
- **[2026-08-25](/news/2026-08-25/)** — Schema 5 dependency contracts, zlib/libarchive integration and library timing
- **[2026-08-24](/news/2026-08-24/)** — August feasibility study, initial schema 3 with one library and schema 4 with two

## Library inventory over time

Each news item reports **distinct libraries** (unique logical library IDs) and **versioned integrations** (every retained version, including superseded revisions). These are end-of-day catalog counts reconstructed from historical XML versions, not a count of source files or individual DLLs.

| Date | Distinct libraries | All versioned integrations |
| --- | ---: | ---: |
| 24 Aug | 2 | 2 |
| 31 Aug | 11 | 11 |
| 05 Sep | 25 | 25 |
| 25 Sep | 44 | 44 |
| 15 Oct | 49 | 54 |
| 22 Oct | 55 | 62 |
| 30 Oct | 59 | 67 |
| 07 Nov | 60 | 70 |
| 08 Nov | 60 | 87 |
| 09–10 Nov | 62 | 95 |

The first schema-3 version had just one pugiXML entry earlier on 24 August; the end-of-day schema-4 count was two. The feasibility study examined approximately twenty libraries before the initial Admin library catalog was established.

## How the project started

The original question was whether **C++Builder 13 / BCC64X could compile contemporary upstream C and C++ libraries itself**, not merely link against libraries built elsewhere. The first feasibility tests in August preceded the administrative repository. The later technical task graph was progressively refactored into a **per-library/version finite-state machine**. The dated August entries preserve initial conditions, library choices and the iterative fixes that enabled native BCC64X integration. Read the [full engineering story](/manual/story.md) for that architectural evolution.

## News and editorial conventions

News files live under `admin/server-docs/news/news-YYYY-MM-DD.md`. Each entry contains its historical end-of-day library inventory, a readable summary of the work and, where useful, cross-links to the maintained story or library pages. Git commit evidence links are deliberately omitted from public news. The technical build contracts and current FSM remain the operational records.

Large integration tests re-entered a unified standalone repository on **22 October 2026**, with PDFium, VCL/SDL2 and CompareTest consumers, followed by QPDF regressions and DeckKernel database tests. This became an important parallel track beside individual library smokes.

All public documentation is in English.

[Library documentation](/manual/libraries.md) · [BuildEngine manual](/manual/buildengine.md)
