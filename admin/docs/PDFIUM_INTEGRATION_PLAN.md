# Integrating PDFium into BuildEngine
## Native C++23/BCC64X Build Using the Existing Skia 153 Package

**Historical plan dated:** 15 October 2026  
**Status at the time:** Design and implementation plan, **not** a verified successful PDFium build. Later decisions may supersede the historical version pins and options recorded here. In particular, this is not a current declaration that every external component passed.

### 0. Objectives, boundaries and binding decisions

Integrate PDFium as an independent third-party package in the existing BuildEngine FSM (Source/Verify/Patch/Build/Test/Install/License/Package/Bootstrap). The primary objective is **a native BCC64X build of the C++ core**, not consumption of a prebuilt foreign C ABI DLL. A separate C++23 facade shall isolate unstable internal PDFium types. PoDoFo remains available for its existing PDF forms and editing functionality.

**Non-negotiable constraints at the time of this plan:**

1. Use exactly the existing BuildEngine **Skia 153** package, source commit `2eed75b956045eb8603d3690a1e84bc582a2135d`; do not silently download or build a second Skia.
2. Prefer existing packages (zlib, FreeType, ICU, JPEG, PNG, etc.). Document incompatibilities as test findings rather than silently bundling another copy.
3. Introduce missing, independently useful permissively licensed libraries as normal BuildEngine packages **before** PDFium.
4. Do not automatically change Skia 153 pins. Select a compatible PDFium revision or design a minimal versioned compatibility patch. If neither is possible, **block** integration.
5. No hidden networking during the build. Source downloads must be controlled by expected SHA-256 checksums, full revisions, lock entries and verifiable licenses.
6. No dependencies on PowerShell, .NET, Cygwin/cygpath or global package managers. Resolve GN, Ninja, BCC64X, llvm-ar and Python using managed BuildEngine tools.
7. Do not accidentally introduce GPL/AGPL components into the runtime dependency graph. Assess the **actual selected build variant**, not merely the root library name.
8. Avoid mixing incompatible CRT, STL, exception, RTTI and exported-symbol configurations across C++ library boundaries.

### 1. Existing BuildEngine baseline at planning time

Source: `BuildEngine-Admin/admin/build-libraries.xml`, coordinate `skia/153`.

| Package | Present | Role and verification need |
|---|---|---|
| Skia 153 | Yes | Mandatory existing package; verify native BCC64X GN port and shared/component build |
| zlib 1.3.2 | Yes | External GN target; no second zlib |
| libjpeg-turbo 3.2.0 | Yes | JPEG decoding; verify external target and definitions |
| libpng 1.6.58 | Yes | Skia codecs/optional PDFium paths; no second copy |
| FreeType 2.14.3 | Yes | PDF fonts; compare PDFium's modified FreeType configuration |
| ICU | Yes | fxcrt/Unicode; verify ABI, data and version |
| HarfBuzz 14.4.0 | Yes | Skia dependency; no unchecked duplicate |
| Brotli 1.2.0 | Yes | Optional; experimental PDFium Brotli feature disabled initially |
| libtiff | Yes | XFA TIFF disabled initially |
| GoogleTest | Yes | Consume the existing package for smoke/unit tests |
| GN, Ninja, llvm-ar, Python | Existing tool workflow | Lock revisions and host-tool identities |

PDFium expects a Chromium-specific GN structure. The Skia GN integration is an **implementation reference**, not a proven drop-in replacement.

### 2. Missing libraries: package them before PDFium

**Priority A, separate normal BuildEngine packages:**

| Proposed package ID | Upstream / license | Role | Packaging strategy |
|---|---|---|---|
| `abseil` / `abseil-cpp` | Google Abseil, Apache-2.0 | Direct `fxcrt` core dependency | Pinned compatible revision, CMake, BCC64X, tests, exported targets and license |
| `fast_float` | fastfloat/fast_float, MIT / Apache-2.0 / BSL-1.0 options | `fxcrt` numeric parser | Header-only/interface package, compiler tests, original license |
| `lcms2` | LittleCMS 2, MIT | ICC color management | Separate C package, exports, smoke and Windows GN external adapter |
| `openjpeg` | OpenJPEG, BSD-2-Clause | JPEG 2000 / JPX | Independent package, JPEG2000 smoke and comparison with PDFium fork |

**Priority B, only if actually reachable:**

- `simdutf`: MIT or Apache-2.0; appears in PDFium DEPS but must be shown reachable under the selected production GN flags. If required, package separately with UTF-8/UTF-16 tests.
- `fp16`: MIT; DEPS presence alone does not prove it is a runtime requirement. Provide a header-only package if necessary.
- Other Chromium helpers: inspect with `gn desc` and dependency resolution. Package generally reusable permissive libraries separately. GN scripts and PDFium-specific shader/configuration helpers are **reproducible build inputs**, not artificial runtime packages.

Do not indiscriminately enable V8, XFA, PartitionAlloc, Rust/Fontations, Chromium codecs, Android tooling, CIPD compiler downloads or fuzz corpora. They are disabled in the minimum profile or belong to optional later profiles. Their presence in DEPS alone does not make them shipped runtime dependencies.

For every new package require a version pin, download hash, license, patches, Debug/Release or interface contract, BCC64X consumer smoke, installation/package, lock/bootstrap, dependency closure and SBOM before PDFium can reference it.

### 3. PDFium revision and Skia compatibility: Gate 0

Relevant upstream files: `DEPS`, `pdfium.gni`, `BUILD.gn`, `skia/BUILD.gn`, `core/fxge/BUILD.gn`.

- The studied PDFium DEPS pins Skia `7ef86a5b0eb91db85580bd71ebed3c4ba489d5ba`, different from the installed Skia 153 commit `2eed75b956045eb8603d3690a1e84bc582a2135d`. A shared Google origin proves neither API nor ABI compatibility.
- Evaluate three PDFium revisions near the Skia 153 timeframe: prefer the same Chromium/Skia milestone family, then newer stable revisions. Record every candidate's commit SHA and full DEPS map; do not choose an unverified floating “latest.”
- Inspect `git merge-base` and revision distance, signatures used by `core/fxge/skia/*` and `skia/config/SkPdfiumUserConfig.h`, flags, headers, exports, inline functions and linker symbols.
- PDFium's GN project uses `//skia` and imports `//third_party/skia/gn/*.gni`, `skcms`, etc. An external installed Skia mode is **not** an upstream GN option; supply it through a BuildEngine-specific, version-bound GN adapter/patch.
- Skia 153 declares `is_component_build = true`; PDFium should initially use `is_component_build = false` independently. External Skia imports must match actual exports, `SKIA_DLL` and build definitions.
- Compiling Skia source again within PDFium is **prohibited**. `gn desc` must confirm no internal Skia source target is active.

**Gate 0 acceptance:** precisely pinned PDFium revision, verified external Skia headers/exports/ABI, a bounded patch list and complete dependency/license manifest. An unresolved Skia mismatch means STOP, not a second Skia copy.

### 4. Reproducible build inputs

The BuildEngine `source` phase prepares an isolated PDFium workspace from verified archives and commits. Chromium DEPS is a **reference inventory**, not permission to run uncontrolled `gclient sync`.

Provision only the required GN import closure:

- PDFium sources, `pdfium.gni`, `build_overrides/*` and Skia adapter inputs.
- Chromium `build`, `buildtools`, required `testing` GN definitions and Python helpers, all pinned to PDFium's selected DEPS revision.
- Abseil, fast_float and other reusable components as BuildEngine packages, not extra PDFium source copies.
- GN, Ninja, Python and BCC64X through managed tool resolution; `gn gen` must not install a toolchain.
- Test whether GN generation can run without `depot_tools`/`gclient` hooks. Create missing minimal hook outputs deterministically in source/prepare, not through depot-tool upgrades.

For every artifact record URL, commit/version, SHA-256, expected extraction root, patch SHA, licenses and NOTICE files. Generated source trees must be rebuildable without unversioned checkout assumptions.

### 5. Dedicated GN/BCC64X port

Use the existing Skia 153 XML and `bcc64x-gn.patch` design as an integration reference:

1. Introduce an encapsulated PDFium BCC64X target configuration, provisionally named `pdfium_use_bcc64x = true`, for x64, BCC64X C/C++ compile and link, `llvm-ar` archiving, host GN/Ninja.
2. Translate MSVC/Clang flags explicitly and use Windows SDK/RAD library paths. Set `clang_use_chrome_plugins = false`; never misidentify BCC64X as MSVC or generic GNU Clang.
3. Specify UTF-8, C++ standard, exception/RTTI/CRT policy, iterator-debug compatibility, visibility, exports, Debug and Release. Do not mix incompatible configurations with Skia.
4. Redirect GN `//skia` to the **installed BuildEngine Skia package**, including includes, public defines, import library, DLL lookup and transitive dependencies. Do not use a `source_set` that recompiles Skia implementation sources.
5. Create Windows GN external targets for zlib, JPEG, PNG, FreeType, ICU, LittleCMS, OpenJPEG, Abseil and fast_float rather than relying on global `pkg-config`. Enable only when API/ABI and CMake/GN publication contracts match.
6. Investigate PDFium-specific FreeType, lcms2 and OpenJPEG fork changes. Externalize only after functional regression tests prove the required PDFium behavior and security changes are retained.
7. Keep functional patches separate: `bcc64x-gn`, `buildengine-external-skia`, `buildengine-external-deps`, `bcc64x-source-compat`. Verify with `git apply --check` before applying, and retain provenance.
8. Test BCC64X `llvm-ar` archive reconstruction and object inventories; prevent duplicate objects and symbols in incremental archives.

**Illustrative GN profile only; validate the target revision before treating it as production `args.gn`:**

```gn
target_cpu = "x64"
is_debug = false
is_component_build = false
clang_use_chrome_plugins = false

pdf_enable_v8 = false
pdf_enable_xfa = false
pdf_use_partition_alloc = false
pdf_enable_fontations = false
pdf_enable_rust_bmp = false
pdf_enable_rust_png = false
pdf_enable_brotli = false

pdf_use_skia = true
pdf_use_agg = true

use_system_zlib = true
use_system_libpng = true
use_system_lcms2 = true
use_system_libopenjpeg2 = true
pdf_bundle_freetype = false

# BuildEngine-specific GN patch/adapter, not an upstream flag:
pdfium_use_bcc64x = true
```

Other package options, especially JPEG, ICU and Skia, depend on the exact revision. Do **not** initially force `pdf_is_complete_lib`; demonstrate modular native C++ linking and external Skia first. Test complete archives later only without duplicating external libraries.

### 6. Profiles and optional functionality

**P0 / minimum viewer and printing (required):** BCC64X, Skia renderer, AGG as a built comparison backend (not primary), AcroForms, text, images, ICC, JPEG2000 and print bitmaps; no V8 scripts/XFA/Rust/Fontations or network access.

**P1 / product integration:** C++23 RAII facade, VCL viewer, zoom/rotation, text search/highlighting, form widgets, PoDoFo integration, GDI printing and a test printer.

**P2 / only by separate decision:** V8/JavaScript, XFA, Fontations/Rust, additional image types, Linux backend and extended GPU paths. Every extension requires separate licensing and dependency approval.

For forms, a saved field value and its visible appearance stream are different. Verify rendered AcroForms and any widget overlays. PoDoFo saves, PDFium renders, and printing is an independent job process.

### 7. Native C++23 rather than C-API-first

- Use internal `CPDF_Document`, `CPDF_Page`, bitmap/render helpers and their native ownership semantics where practical.
- Do not expose unstable internal PDFium classes uncontrolled to every consumer.
- Provide facade types such as `adecc::pdf::document`, `page`, `render_surface`, `text_search`, `form_view`, with move-only RAII semantics, precise result/error types and controlled lifetimes.
- Keep the PDFium C API optional for comparative/differential testing or a compatibility fallback; it is not the primary route.
- For display, connect PDFium to compatible Skia `SkBitmap`/`SkImage`/Surface and then VCL. For printing, render into an appropriate Skia/PDFium pixel or DC target and pass it to Windows printing. Distinguish spooler acceptance from successful physical printing.

### 8. BuildEngine FSM and packaging contract

Add a versioned `<library id="pdfium" ...>` entry with explicit `<dependency>` coordinates, including `skia/153` in this historical plan.

| Stage | Actions | Required evidence |
|---|---|---|
| Source | Materialize verified PDFium/Chromium GN inputs | Hashes, commit lock, no hidden downloads |
| Patch | Validate/apply isolated GN, port and external-dependency patches | Patch SHA and `git apply --check` |
| Build | `gn gen`, verify target closure, `ninja` Debug/Release | Build logs, linker map, exactly one Skia package |
| Test | Own smokes and compatible PDFium unit/embedder tests | CTest/log summary, no silently skipped tests |
| Install | Includes, C++/import libraries, runtime DLLs, license data, CMake package | Complete installed paths |
| Publish | Publish consumer SDK under `install/Win64x` with `find_package(PDFium CONFIG REQUIRED)` | Targets, transitive dependencies, independent consumer smoke |
| StackBuilder | Versioned package with binaries, patch/dependency locks, licensing | Reproducible archive |
| Bootstrap | Validate and integrate package and transitive dependencies into ThirdParty | Existing XML/lock-based resolution |

Proposed coordinate: `pdfium/<revision-or-version>` under current BuildEngine conventions. `PDFium::PDFium` is a **proposed** CMake target and must be reconciled with the actual publish model. Never load a Skia DLL from the wrong Debug/Release variant.

### 9. Tests and acceptance criteria

**T0 — independent new libraries:** Native BCC64X build or header smoke, installed dependencies, tests, licenses, package and bootstrap.

**T1 — GN generation:** Reproducible `gn gen`, no implicit tool/source downloads or unresolved Chromium imports. `gn desc` confirms the expected closure and **no compiled internal Skia library**.

**T2 — native C++ linking:** An independent consumer includes native PDFium C++ headers, creates/loads a document and links only against published BCC64X packages, with verifiable Skia revision and build variant.

**T3 — rendering:** Synthetic PDF samples with Unicode text (including diacritics), rotation, transparency, images, font subsets, ICC, JPEG2000, and 72/150/300 dpi. Verify image dimensions, visual goldens, and pixel diffs between Skia and AGG within defined tolerances. AGG is only a comparison backend; no separate AGG installation is needed.

**T4 — AcroForm:** Populate text, multiline, Unicode, checkbox, radio and choice widgets using PoDoFo. Validate saved values **and rendered appearance** using PDFium, then print a filled form. Detect XFA without V8 explicitly as unsupported.

**T5 — printing and GUI:** VCL viewer with document/page RAII, zoom/scroll/rotation, test/PDF printer and accurate page geometry (margins, duplex, scaling). Do not mistake successful spooler submission for physical print completion.

**T6 — robustness/security:** Damaged PDFs, passwords/protection, blank pages, extreme dimensions, large files, resource limits and invalid fonts. Require controlled errors and no leaks/use-after-free. Optional fuzzing using a compatible host toolchain is additional security evidence.

**T7 — reproducibility:** Two clean builds with identical inputs, compare normalized artifact hashes and byte identity when required, Windows Debug/Release CI, later separate Linux evidence. SBOM must describe actually linked and runtime-loaded components.

**Gate rule:** A failing test or unresolved license means no publication. Do not silently narrow acceptance to “compiles.”

### 10. License management

- Verify PDFium's BSD-style and Apache-2.0 texts against actual files/components of the final pinned snapshot; include notices.
- Respect the historical `licenseSignal="yellow"` for Skia 153 and reassess linked Skia subcomponent licenses.
- Abseil: Apache-2.0; fast_float: permissive alternatives requiring a specific package license selection; LittleCMS: MIT; OpenJPEG: BSD-2-Clause; simdutf: MIT/Apache-2.0; FP16: MIT.
- Record FreeType's license choice and ICU/Unicode, libjpeg-turbo and other image-codec notices.
- Distinguish **host build tools** from **distributed runtime binaries**. Check Apache-2.0 NOTICE and patent provisions; download permission alone does not constitute license approval.
- Generate revision-specific license/SBOM evidence from the actual GN graph and reconcile it with BuildEngine's license status. Do not introduce GPL/AGPL dependencies accidentally.

### 11. Implementation work packages in order

1. **Inventory and revisions / Gate 0:** PDFium candidates, Skia 153 compatibility, required GN/C++ dependency closure and licenses.
2. **Missing packages first:** fast_float, Abseil, lcms2, OpenJPEG; simdutf/fp16 only if reachable, with complete FSM and smokes.
3. **PDFium source/prepare:** deterministic verified PDFium/Chromium GN inputs; no gclient hooks or downloaded compilers.
4. **BCC64X GN toolchain:** adapt Skia lessons; handle warnings and flags separately.
5. **External-dependency GN:** map installed packages, Windows without global pkg-config.
6. **External-Skia GN:** Skia 153 headers, DLL/imports/ABI, demonstrate no recompilation.
7. **Native PDFium compile/link:** core C++ targets first, then complete Skia+AGG core, Debug/Release.
8. **C++23 consumer and visual tests:** rendering, forms, viewer and printing using synthetic files; never commit confidential customer PDFs.
9. **Publish/bootstrap/StackBuilder:** CMake targets, licensing, locks, SBOM, package archives and reproducible CI.
10. **Optional profiles:** Linux, V8/XFA, Rust/Fontations only after separate approval and license review.

### 12. Risks and stop criteria

| Risk | Mitigation / stop |
|---|---|
| Skia 153 API differs from PDFium pin | Compatible PDFium revision or versioned patch; never a second Skia build |
| PDFium compiles Skia internally | Inspect GN closure and adapter; Skia sources in PDFium build mean STOP |
| Chromium GN expects MSVC/Chromium Clang switches | BCC64X GN patch as with Skia; no hidden compiler substitution |
| External lcms2/OpenJPEG differs from PDFium fork | ICC/JPX regression tests; require functional and API compatibility |
| Skia DLL/CRT/export mismatch | Native C++ link/runtime smokes, symbol checks and matching ABI |
| Build fetches undeclared packages | Isolated offline rebuild, complete source/hash lock |
| Native PDFium C++ internals are unstable | Versioned adecc C++23 facade, independent consumer smoke per revision |
| PDF forms save values but not appearances | Widget/appearance tests and printing before production |
| License conflict | Red license indicator; block build/publish |

### 13. Definition of Done

PDFium is considered integrated only when: source/Skia/third-party pins are revision-secure; independently useful missing packages have complete contracts; native BCC64X Debug and Release build from reproducible inputs; exactly **one** Skia 153 package is consumed without recompiling Skia; C++23 consumer and Skia rendering, AcroForm and printing tests pass; GN/build/consumer tests, licensing, notices, SBOM, install/publish, bootstrap and StackBuilder succeed; error cases, limitations including XFA without V8, and all port patches are documented.

### 14. Research sources to pin again at implementation time

- PDFium upstream: https://pdfium.googlesource.com/pdfium/
- PDFium DEPS: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/DEPS
- PDFium GN options: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/pdfium.gni
- PDFium main targets: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/BUILD.gn
- PDFium Skia GN: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/skia/BUILD.gn
- PDFium third-party targets: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/third_party/BUILD.gn
- PDFium license: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/LICENSE
- Abseil: https://github.com/abseil/abseil-cpp
- fast_float: https://github.com/fastfloat/fast_float
- LittleCMS 2: https://pdfium.googlesource.com/pdfium/+/HEAD/third_party/lcms/LICENSE
- OpenJPEG: https://github.com/uclouvain/openjpeg
- simdutf: https://github.com/simdutf/simdutf
- FP16: https://github.com/Maratyszcza/FP16

**Important:** `refs/heads/main` references are research links, **not reproducible build pins**. Fix commit SHA, DEPS closure, exact licenses and sources to one immutable snapshot before implementation.

### 15. Binding clarification: external DLLs and immutable source snapshot (16 October 2026)

This clarification overrides older plan sections where they proposed source inclusion rather than existing managed library packages.

**BuildEngine packages first:** When an installed package is provided as a BCC64X DLL and PDFium's GN targets can be adapted compatibly, consume the installed DLL, import library and public headers. Do not silently make another copy of the same upstream source in PDFium. Verify exports, compiler/CRT compatibility, threading, Debug/Release and behavior first. Document any incompatible PDFium-specific source fork and decide separately. Not all installed packages are DLLs; a static or header-only product must not be misrepresented as a DLL.

**Immutable DEPS resolution:** Pin one PDFium commit and all required internal DEPS revisions before source acquisition. Only the BuildEngine Source phase may acquire these sources, verifying SHA-256 and expected directory structure and assembling the entire GN input tree. Use the already established Skia-style Python/GN/Ninja orchestration without modifying Skia.

**No network after source:** Once source provisioning has completed, `gn gen`, Ninja, Python helpers, tests, install, Doxygen and publish may not download or synchronize source, tools or modules. No `gclient sync`, `git-sync-deps`, automatic Git/CIPD/FetchContent or update hooks in these phases. Missing inputs must cause a concrete missing-pin or missing-artifact error rather than a background download. Repeating these phases offline is a mandatory acceptance test.

**Mapping and SBOM:** Each actually used component is either an external versioned BuildEngine package with DLL/import library or an internally pinned PDFium DEPS source. Bundled parts such as PDFium AGG 2.3 or optionally PartitionAlloc belong in `sbom/bundled-components.json` and CycloneDX licensing evidence, as with Skia. A downloaded but disabled component must not be described as a distributed runtime dependency. Verify classifications against real GN targets.

**Preserve existing evidence:** Creating the PDFium integration must not change timestamps, definitions or FSM evidence of the **56 packages successfully built at that historical point**.
