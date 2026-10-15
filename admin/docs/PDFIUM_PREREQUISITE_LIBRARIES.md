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
