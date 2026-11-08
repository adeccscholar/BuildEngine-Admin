# Library update checklist — Phase 1 (2026-11-08)

[TOC|Content]

This engineering record tracks the native Windows x64 / RAD Studio 13 BCC64X
package migration. The normative source is `admin/build-libraries.xml`.
**Contract/configuration checks are not evidence that any native BCC64X
build, CTest, smoke, publication, or loader run passed.** The previous
PDFium 2025.11.19 / SDL2 viewer result remains historical evidence for
the configuration **before** the fast_float dependency update.

## New active package versions

| Package | Replaced coordinate | New coordinate | Declared dependencies and integration |
| --- | --- | --- | --- |
| ACE | 8.0.6 | **8.0.8** | OpenSSL 3.5.9, Xerces-C 3.3.0, zlib 1.4; shared ACE/TAO upstream 8.0.8 source; MPC/BMake, DLL builds |
| TAO | 4.0.6 | **4.0.8** | Version-paired extension of `ace@8.0.8`; same ACE_wrappers producer tree |
| fast_float | 8.2.3 | **8.3.3** | Header-only; installed CMake package, separate consumer smoke; `pdfium@2025.11.19` now depends on 8.3.3 |
| Poppler | 26.09.0 | **26.11.0** | zlib 1.4, FreeType 2.15.0, HarfBuzz 15.1.0, JPEG 3.2.0, PNG 1.6.59, TIFF 4.7.2, win-iconv 0.0.8 |
| QPDF | 12.4.1 | **12.4.2** | zlib 1.4 and JPEG 3.2.0; managed dependencies, native Windows qtest and C++ consumer smoke |

All replaced version records are retained as `supersededBy` entries;
only new records are active. Each new record has a new independent timestamp.
The `pdfium@2025.11.19` timestamp is also changed because its fast_float
dependency was updated. No implicit "newest version" lookup is permitted.

## Version-specific patch inventory

### ACE / TAO

ACE 8.0.8 already contains the Windows SSL socket/select corrections in
the old `bcc64x-win64-upstream-backports.patch`. TAO 4.0.8 already
contains the SSLIOP `params_dup.cpp` conversion. Applying either patch a
second time is incorrect.

The two required local adaptations are kept in a dedicated 8.0.8 patch folder:

- `patches/ace-tao/8.0.8/bcc64x-xerces-modern-borland.patch`
- `patches/ace-tao/8.0.8/bcc64x-zlib-library-names.patch`

The zlib patch is rebased against the 8.0.8 MPC project state, where the
Windows defaults are `z` and `zd`. BuildEngine's existing per-configuration
`-value_template zliblib=...` inputs select the managed import libraries.

### QPDF

- `patches/qpdf/12.4.2/native-windows-qtest.patch`: carried forward only
  after checking all six hunks against upstream QPDF 12.4.2.
- `patches/qpdf/12.4.2/prefer-explicit-managed-dependencies.patch`:
  rebased against QPDF 12.4.2 CMake, retaining the new `STATIC_JPEG`
  handling when explicit managed JPEG inputs are not supplied.

### Poppler

The 26.11.0 tarball has a version-specific SHA-256 recorded in the XML,
with HarfBuzz declared explicitly. Existing platform-specific
`_AMD64_` / `NOMINMAX` flags and disabled frontends are retained;
the compiler and upstream CMake configuration must be validated in the
native Windows build.

## Required verification

- [x] New version contracts and predecessor/superseded records checked
- [x] Active library coordinate count and dependency graph checked
- [x] ACE and QPDF source patch context checked against selected upstream revisions (textual hunk verification, not native git apply)
- [x] ACE 8.0.8 and Poppler 26.11.0 download SHA-256 values matched to published archive metadata
- [x] BuildEngine-Tests QPDF extraction project and PDFium fast_float consumer declarations updated
- [ ] ACE 8.0.8 Release + Debug: Source / Patch / Build / Install / Smoke / Metadata / Publish
- [ ] TAO 4.0.8 Release + Debug: shared ACE extension / Build / Install / Smoke / Metadata / Publish
- [ ] fast_float 8.3.3 Release + Debug: Install / Package CMake import / Consumer Smoke
- [ ] Poppler 26.11.0 Release + Debug: CMake configure / Build / Install / Consumer / Metadata
- [ ] QPDF 12.4.2 Release + Debug: both git patch checks, CTest, native tests, install, consumer smoke
- [ ] PDFium 2025.11.19: rebuild and repeat PDFium C API / SDL2 viewer tests with fast_float 8.3.3
- [ ] BuildEngine-Tests: `pdf-image-extraction` Release and Debug CTest

**Not in Phase 1:** re-pointing the unrelated older `libjpeg-turbo@3.2.0`
or `libzip`/Expat pins in other active libraries. Those are separately
tracked in the catalog dependency audit; do not mix them into a targeted
regression repair without their own rebuild/test plan.

## Upstream sources

- ACE / TAO release: https://github.com/DOCGroup/ACE_TAO/releases/tag/ACE%2BTAO-8_0_8
- fast_float release: https://github.com/fastfloat/fast_float/releases/tag/v8.3.3
- QPDF release: https://github.com/qpdf/qpdf/releases/tag/v12.4.2
- Poppler release: https://poppler.pages.freedesktop.org/poppler-web-page/releases.html
