# BuildEngine — Development News

[TOC|Content]

A chronological engineering history reconstructed from dated repository commits. Entries cover library and tool integration, configuration changes, debugging, regressions, and the reasons behind selected dependencies. **A merged commit is not equivalent to a successful native build or runtime test.**

## Timeline (newest first)

- **[2026-11-10](/news/2026-11-10/)** — 95 library documentation pages, transitive Mermaid graphs, news routes and Skia DLL review
- **[2026-11-09](/news/2026-11-09/)** — VTK 9.7.2, OpenCV 5.1, new tools and libarchive, PDFium, libtiff and OpenXLSX corrections
- **[2026-11-08](/news/2026-11-08/)** — Catch2, GoogleTest, oneTBB, PoDoFo, Poppler, Tesseract and other library upgrades
- **[2026-11-07](/news/2026-11-07/)** — ACE/TAO build parallelism and Windows libpq interfaces
- **[2026-10-30](/news/2026-10-30/)** — Brotli 1.2.1 and the OpenSSL resource compiler
- **[2026-10-28](/news/2026-10-28/)** — MariaDB Connector/C 3.4.11 and import libraries
- **[2026-10-25](/news/2026-10-25/)** — WIL, Tesseract, Leptonica, OpenCV Contrib and WinRT
- **[2026-10-21](/news/2026-10-21/)** — oneTBB 2023.1.0, LLVM OpenMP, CMake resource tooling and dependency graphs
- **[2026-10-20](/news/2026-10-20/)** — PDFium/Abseil DLL boundaries, libpq ABI, resource tooling and Debug consumers
- **[2026-10-15](/news/2026-10-15/)** — XZ, Expat, libarchive, Abseil, Mermaid and documentation tools
- **[2026-09-25](/news/2026-09-25/)** — Dynamic BCC64X runtime and PoDoFo DLL consumers
- **[2026-09-24](/news/2026-09-24/)** — BCC64X Meson bootstrap, PostgreSQL source repairs and narrowed libpq scope
- **[2026-09-20](/news/2026-09-20/)** — Restored BCC64X CMake baseline and ICU VCXPROJ bootstrap documentation
- **[2026-09-09](/news/2026-09-09/)** — Managed Graphviz documentation tooling and pinned version preservation
- **[2026-09-05](/news/2026-09-05/)** — Skia integration repairs, GN patches, wrapper removal and reverted cleanup
- **[2026-09-02](/news/2026-09-02/)** — Reproducible Meson and managed Python source bootstraps
- **[2026-08-27](/news/2026-08-27/)** — NASM for OpenSSL and removal of the zstd static product
- **[2026-08-25](/news/2026-08-25/)** — Schema 5 dependency contracts, zlib/libarchive integration and library timing

## Evidence classification

- **Registered:** A dependency, tool or configuration was introduced into the repository.
- **Changed:** An integration patch or BuildEngine correction was committed.
- **Regressed / reverted:** A previous approach was rolled back or constrained.
- **PASS:** Reserved for an exact build/variant with matching FSM, smoke-test or runtime evidence.
- **Not verified:** No reproducible result was available from the inspected evidence.

## Historical counts

The 2026-09-25 entry records an historical set of **44 library documentation entries**. The 2026-11-10 documentation refresh refers to **95 versioned pages across 62 library IDs**. Those numbers describe documented inventory at different points in time, not passing build counts.

## Storage and dating rules

News sources are kept in `admin/server-docs/news/news-YYYY-MM-DD.md`. Date each entry by the actual historical commit or event, not the day of later editorial reconstruction. Keep original entries, annotate retrospective additions, and cite the corresponding commits. All external documentation, migration guidance and news must be written in English.

[Library documentation](/manual/libraries.md) · [BuildEngine manual](/manual/buildengine.md)
