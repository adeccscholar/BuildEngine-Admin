# PDFium prerequisite packages for the BCC64X BuildEngine

Date: 2026-10-15. Status: **integration contracts committed, build validation pending**.

The following four independent packages have been added to
`admin/build-libraries.xml`, with BCC64X/Ninja CMake configuration,
install, publish, license collection and a separate Consumer smoke:

| ID | Version/tag | License | Upstream CMake package | Smoke |
|---|---|---|---|---|
| fast_float | 8.2.3 / v8.2.3 | MIT (choose LICENSE-MIT) | FastFloat::fast_float | fast_float-consumer |
| abseil | 20260817.0 | Apache-2.0 | absl::strings | abseil-consumer |
| lcms2 | 2.19.1 / lcms2.19.1 | MIT **core only** | lcms2::lcms2 | lcms2-consumer |
| openjpeg | 2.5.4 / v2.5.4 | BSD-2-Clause | openjp2 | openjpeg-consumer |

The LittleCMS **GPL-3.0 fast_float and threaded plugins** are disabled.
These are unrelated to the independently permissively licensed
`fastfloat/fast_float` C++ parsing library.

## Verification gates before release

**The package definitions are not yet proven to work under BCC64X.** A
complete test must run on the actual Windows BuildEngine installation.

1. Download each pinned release archive. Independently obtain its SHA-256,
   compare archive and expected top-level directory. **The XML deliberately
   contains no invented SHA-256 values.** Add verified hashes to each
   `<download>` before treating builds as reproducible or publishing.
2. Confirm no unexpected transitive source fetches and no GPL-licensed
   LittleCMS plugins. Review *all* transitive notices.
3. Validate the `admin/build-libraries.xml` document against
   `admin/schemas/build-libraries.xsd` and run BuildEngine contract
   validation before performing source/build/install actions.
4. Build Debug and Release with BCC64X; inspect exported `.lib`,
   `.dll` and installed CMake targets for the *actual* upstream target
   names. For Abseil, confirm its full static archive closure and options
   header reflect the same compiler, CRT and C++ standard as its consumers.
5. Execute the four independent consumer smokes, including runtime DLL
   resolution for LittleCMS and OpenJPEG. Verify installed configurations
   do not reference their build/source trees.
6. Run the upstream tests separately where feasible. Smoke passes alone
   do not establish feature completeness, thread safety or robust decoding.
7. Verify StackBuilder archives, hashes, license dashboard, bootstrap
   and package dependency metadata. Mark all four packages **release
   blocked** until these gates succeed.

## Important compatibility checks

- The upstream OpenJPEG package lives in
  `lib/win64/<Configuration>/cmake/openjpeg-2.5`. Its CMake package
  is `OpenJPEG` (the smoke sets `OpenJPEG_DIR`).
- The fast_float CMake install configuration normally uses the
  `share/cmake/FastFloat` folder; our GN/CMake package sets
  `CMAKE_INSTALL_DATAROOTDIR` to the per-configuration library tree
  to place that configuration in the usual BuildEngine SDK view.
- The LittleCMS 2.19.1 CMake build must keep both optional GPL plugins
  disabled and must install the core library as a standalone component.
- PDFium's custom versions of lcms2/OpenJPEG may include extra patches
  and hardened decode paths. Reusing these external libraries in PDFium
  is a separate compatibility and regression task, not automatically
  guaranteed by successful independent package builds.

## Suggested local execution

Run the BuildEngine contract validator and its normal
source/build/test/install/publish workflow for each of the IDs
`fast_float`, `abseil`, `lcms2`, `openjpeg`.
Use the existing BuildEngine command-line syntax rather than inventing
a new one. Do not commit or upload actual customer PDFs.

The version/tag coordinates above were checked against the upstream
CMake sources. Archive hashes, extraction roots, built binaries and
smoke execution **have not been verified in the user's Windows
environment**.


## Agreed package boundaries (2026-10-16)

These are architecture decisions, **not** claims that packages have already been built
or that PDFium can replace its vendored sources without changes.

| Component | Ownership | Version policy | Integration |
|---|---|---|---|
| simdutf | Independent BuildEngine library | Pinned release v9.3.0 | Native upstream CMake, Debug/Release, installed consumer |
| AGG 2.6 | Independent BuildEngine library | Pin the reviewed ghaerr/agg-2.6 source commit | BuildEngine-managed CMake integration, explicitly excluding GPC |
| PartitionAlloc | Bundled PDFium dependency | Pin exact revision from PDFium DEPS | Keep within PDFium GN source/build graph |
| PDFium AGG 2.3 fork | Bundled PDFium dependency | Pin exact PDFium revision | Keep PDFium's patches and GN integration; do not substitute AGG 2.6 |

The standalone AGG 2.6 package must not compile, install, ship or expose the
General Polygon Clipper (GPC) implementation, headers, examples or archive
objects. Verify the actual selected source files, copyright notices and
the resulting binary/package contents. Do not infer license approval from
the top-level repository license alone.

Neither standalone simdutf nor AGG 2.6 is automatically a PDFium build dependency.
Declare a PDFium `<dependency>` only when the selected PDFium GN configuration
actually consumes the installed BuildEngine package rather than its own fork.

### GN orchestration

Use the existing BuildEngine-controlled Skia pattern: XML describes downloads,
pinned revisions, extraction, GN arguments and Ninja actions; BuildEngine executes
and tracks phases. Reuse the already functional Skia Python/GN tooling as
appropriate, without new package-specific orchestration scripts and **without
modifying or invalidating Skia's successful states**.

### Bundled dependencies are first-class SBOM evidence

PDFium's `DEPS` file is a **candidate inventory**, not a statement that every
dependency was downloaded, compiled or shipped. For the selected PDFium commit,
record the dependencies actually incorporated into the selected GN targets,
including at least the included AGG 2.3 fork and PartitionAlloc when enabled.
Distinguish absent, disabled, build-only, test-only and shipped components.

Follow the already-implemented Skia evidence contract:

1. Determine each bundled component's exact name, Git commit/tag or version,
   upstream repository/download URL, archive hash where available, license and
   license-evidence path from the **actual pinned upstream source**.
2. Create `sbom/bundled-components.json` as part of PDFium's controlled
   preparation/installation evidence. Use the same `bundledComponents` array
   structure already consumed by `MetadataJob.h`; never guess a version, hash
   or SPDX identifier.
3. Use the BuildEngine XML dependency graph **only** for independently built
   external packages. Do not double-count the same dependency as both external
   and bundled.
4. Verify `install/packages/pdfium/<Version>/sbom.cdx.json` contains all
   actually bundled components with PDFium dependency edges and the matching
   license metadata in `LICENSE-INFO.txt`. Ensure source license/notice files
   are retained as required.
5. Reconcile the manifest against selected `DEPS` pins and the actual GN
   build graph; fail release validation on a missing required bundled component.
   Do not silently treat a downloaded-but-disabled component as shipped.
6. Keep the source manifest and produced SBOM associated with the exact PDFium
   commit and GN profile. Metadata changes must not invalidate unrelated
   packages or cause completed Skia/PoDoFo builds to restart.

`MetadataJob.h` already reads
`<logical metadata root>/sbom/bundled-components.json` and adds such
components to CycloneDX output. This mechanism is already present; PDFium
still needs a verified producer and reconciliation against the selected build.

### Separate acceptance checks

- simdutf v9.3.0: verified source, upstream CMake configure/build/install,
  UTF-8 validation/conversion consumer, both configurations, license/SBOM.
- AGG 2.6: pinned reviewed commit, isolated native CMake build definition,
  deterministic anti-aliased raster consumer and binary/source inventory
  demonstrating **no GPC**.
- PDFium: exact commit, pinned and reconciled DEPS, GN/Ninja integration using
  proven Skia workflow, bundled-component manifest, SBOM/license checks, and
  PDF open/render/text consumer. PartitionAlloc and AGG 2.3 remain bundled.

**State:** this section fixes the contract. It does not attest successful
BCC64X builds or assert that the two new independent library entries have
already been added to the catalog.
