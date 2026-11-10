# BCC64X Integration Findings for Modern Third-Party Libraries

**Historical snapshot:** 20 September 2026  
**Target platform:** RAD Studio 13 Florence / C++Builder 13 / BCC64X 20.1.7 / Embarcadero C++ 7.80 / Win64 Modern

## Purpose

This document collects reproducible technical findings from recent third-party library integration and Skia. It supplements the authoritative `admin/build-libraries.xml` contract without replacing it.

The investigation asks which upstream libraries compile unchanged with BCC64X; which package, build-system or compiler adaptations are necessary; whether a correction belongs in the library contract, a versioned patch or a generic toolchain layer; and which problems are general upstream packaging failures rather than BCC64X-specific defects.

The project does **not** silently switch to MSVC, clang-cl, MinGW or another compiler. The subject of the investigation is the actual BCC64X compilation path.

## Summary

The packages exhibited three different categories of integration problems:

1. **Normal packaging:** source compiles with BCC64X but requires reliable install, publish and smoke contracts.
2. **Upstream build/packaging defects:** C++ source is compatible, but exported targets, installed headers or dependency metadata are incomplete.
3. **BCC64X-specific semantics:** source or build logic assumes an older compiler family, linker, runtime or character encoding behavior that does not hold for modern BCC64X.

These layers must remain distinct. Local library workarounds stay local; generic compiler/linker defects are fixed centrally once.

---

## libjpeg-turbo 3.2.0

The historical contract uses the upstream CMake project to build a **shared** libjpeg-turbo 3.2.0. Static libraries, SIMD, upstream tools and tests were disabled for this profile; TurboJPEG was enabled; Debug and Release were installed separately with an independent package-consumer smoke.

Skia consumes the explicit BuildEngine libjpeg-turbo 3.2.0 dependency rather than recompiling an embedded Skia copy.

### GN/BCC64X finding

Skia's GN `libs` mechanism did not always preserve absolute paths to BCC64X import libraries correctly. The specific installed import library is therefore propagated as a raw linker option through `ldflags`. This adapts the Skia third-party GN contract, **not** libjpeg source code.

---

## libpng 1.6.58

The historical profile uses libpng 1.6.58 with pinned SHA-256 source, explicit zlib 1.3.2 dependency, shared library output, static output disabled, separate Release/Debug and an installed-consumer smoke.

### Upstream test status

`PNG_TESTS` was disabled in this contract. This documents an actual configuration choice, **not** a general policy allowing upstream tests to be disabled. The inspected repository evidence did not contain a final independent technical explanation for that choice.

### Skia system integration

Skia consumes BuildEngine libpng rather than a second built copy. For BCC64X, `third_party/libpng/BUILD.gn` was changed from GN `libs` to `ldflags` to preserve the complete absolute import-library path and avoid an unsuitable `-l...` transformation. This pattern recurred with other external libraries.

---

## libtiff 4.7.2

The historical configuration uses libtiff 4.7.2, zlib 1.3.2 and libjpeg-turbo 3.2.0; shared build with static output disabled, separate variants and a package smoke. Tools, contrib, documentation and upstream tests were disabled in that profile. Installed `tiffio.h` and `tiff.h` are explicit require gates.

### Assessment

libtiff did not require a deep compiler patch comparable to Skia. Its primary integration value was a reproducible dependency, packaging and consumer-validation contract. As with libpng, the disabled upstream tests were a recorded configuration choice without a conclusive independent justification in the examined evidence; they required reconsideration under test-policy review.

---

## HarfBuzz 14.4.0

HarfBuzz is an external BuildEngine dependency of Skia, not source fetched or compiled inside Skia. To avoid a cycle with FreeType, the HarfBuzz package builds **without FreeType**. Optional Cairo, Graphite2, GLib, ICU, GObject, GDI, Uniscribe and DirectWrite paths are disabled; core shaping and subsetting remain enabled; raster/vector/GPU/utilities are disabled in this profile.

### Missing Threads dependency in upstream CMake package

The generated CMake export may expose `Threads::Threads` without the generated package configuration calling `find_dependency(Threads)`. Rather than altering HarfBuzz source, BuildEngine preserves the upstream export as `harfbuzzTargets.cmake` and adds a small `harfbuzzConfig.cmake` wrapper that resolves the required dependency.

This is a **package configuration** repair, not a language-compatibility patch.

### Missing public subset header

The community CMake packaging of HarfBuzz 14.4.0 installs `hb-subset.h` and `hb-subset-serialize.h` but omits `hb-subset-depend.h`, which `hb-subset.h` includes. Meson's upstream public subset-header list already contains that header. BuildEngine installs the missing file explicitly without weakening consumer tests.

### Skia system integration

As with libpng, the Skia BCC64X GN path passes HarfBuzz import libraries as raw `ldflags`, including the subset library where PDF subsetting is enabled.

---

## FreeType 2.14.3

FreeType is built as a shared library with explicit dependencies: **zlib 1.3.2, bzip2 1.0.8, Brotli 1.2.0, libpng 1.6.58 and HarfBuzz 14.4.0**. These are required through the BuildEngine contract rather than opportunistic CMake discovery.

Dependency direction is important: build HarfBuzz **without** FreeType first, then FreeType **with** HarfBuzz. This keeps the graph acyclic. The FreeType consumer smoke was extended to verify actual HarfBuzz integration rather than loading an isolated FreeType binary.

### Transitive runtime DLLs

FreeType brings bzip2 into its runtime dependency chain. Consumer smokes must therefore include more than immediate DLL directories. BuildEngine resolves the transitive runtime closure recursively and makes required directories available on the smoke `PATH`.

---

## Skia 153 / pinned source `2eed75b956045eb8603d3690a1e84bc582a2135d`

Skia was the most complex integration experiment in this historical set. Logical version 153 refers to the exact source commit `2eed75b956045eb8603d3690a1e84bc582a2135d`; source and patches were pinned to that commit.

### External BuildEngine dependencies

The historical Skia integration declares OpenGL 26.2.1, zlib 1.3.2, Brotli 1.2.0, libjpeg-turbo 3.2.0, libpng 1.6.58, HarfBuzz 14.4.0 and FreeType 2.14.3. It does not rebuild these libraries internally; version, licensing and SBOM relationships stay in the BuildEngine graph. Transitive dependencies, for example FreeType → bzip2, are also resolved by that graph rather than copied into Skia.

### Internally bundled Skia components

Other upstream components intentionally remain pinned to the selected Skia revision, including ICU, WebP, libavif, libgav1, JPEG XL, Wuffs, DNG SDK, Expat, SPIRV-Cross, SPIRV-Tools, glslang, Vulkan headers, and Vulkan/D3D memory allocators.

They are inventoried separately from external BuildEngine dependencies and fed into CycloneDX. One verified Skia metadata run reported **27 components**, including the root, external dependency closure and bundled components.

### Windows desktop profile

This historical profile enabled CPU raster/skcms, DirectWrite and Windows GDI, FreeType, ICU/HarfBuzz/SkUnicode/SkShaper, PNG/JPEG/WebP/Wuffs, AVIF/JPEG XL/DNG, SVG/Expat, PDF and XPS, Skottie/Lottie, Ganesh, Graphite, OpenGL, Direct3D 12, Vulkan/Vulkan Memory Allocator, SPIR-V validation and SPIRV-Cross.

It deliberately excluded Android/iOS/macOS integrations, WebGL/WebGPU/CanvasKit/Emscripten, Dawn, ANGLE, Rust codecs/Fontations/ICU4X, FFmpeg, Lua, Perfetto/pprof and development viewers/fuzzers.

### GN toolchain for BCC64X

Upstream Skia GN has no native BCC64X model. The version-specific `bcc64x-gn.patch` supplies a Win64 Modern toolchain route with the actual BCC64X compiler and linker, RAD and Windows SDK library directories passed as GN arguments, a BCC64X driver-based `solink` rule producing DLL and import library, and raw linker-option propagation for absolute external import libraries.

### Reusing system packages without duplication

The GN contracts for zlib, Brotli, libjpeg-turbo, libpng, HarfBuzz and FreeType were adapted to use BuildEngine packages. The common BCC64X issue for libpng, libjpeg-turbo and HarfBuzz was forwarding actual absolute import-library paths via `ldflags` rather than GN `libs`.

### Component output and export boundaries

Applying blanket `--export-all-symbols` to Skia produced an impractically large PE/COFF export surface. The solution was a **hybrid component model** with shared libraries deliberately limited to public interfaces:

- `skia`
- `skunicode_core`
- `skunicode_icu`
- `skunicode_client_icu`
- `skunicode_bidi`
- `skunicode_libgrapheme`
- `skunicode_icu4x`
- `skshaper`
- `svg`
- `skottie`

Other internal `skia_component` targets remain static under BCC64X. `jsonreader` and `sksg` are linked into consumers as static implementation parts rather than independently exported DLLs. Default visibility is applied selectively to Skia's own library code; foreign/internal code is not exported wholesale.

### Individual compatibility corrections

The version-bound patch set addressed Direct3D `GetDesc`, Debug SPIR-V validator paths, libgav1 Windows threading, SkArenaAlloc exports, skcms DLL boundaries, centralized visibility and component logic, external dependency propagation, and HarfBuzz/libpng/libjpeg/Brotli paths.

These corrections apply to the **exact Skia source pin**, not arbitrary future revisions.

### Generic UCRT issue: `fabsf` and related functions

Skia Debug revealed a generic BCC64X 20.1.7 linking issue. Certain addressable C math functions imported from `api-ms-win-crt-math-l1-1-0.dll`; the binary linked successfully but failed to load with Windows error **127**.

Minimal reproducers isolated the problem from Skia. Five symbols were affected: `fabsf`, `nextafterl`, `nexttoward`, `nexttowardf` and `nexttowardl`. RAD Studio's `libucrt.a` contains local implementations in exactly four `libucrt_extra` object members. Extracting them into `libbcc64x-ucrt-compat.a` and linking that archive before the ordinary runtime resolved the loader problem.

See `docs/bcc64x-ucrt-runtime-link-bug.md` for complete evidence. This is **not** a Skia source patch: BuildEngine generates the compatibility archive from the locally installed Embarcadero runtime through the managed `bcc64x-ucrt-compat` tool contract, distributing no vendor object files. Skia adds it ahead of the runtime through upstream `extra_ldflags`.

### Smoke and runtime outcomes

The Release Skia package smoke passed **17/17 checks**. Recursive runtime DLL resolution was required to find transitive dependencies, not only immediate Skia DLLs. The final Debug smoke after UCRT corrections was **[not verified]** in the evidence used for this historical report.

---

## Catch2 3.16.0

Catch2 was introduced as a current test framework rather than relying on historical test packages. It is intentionally built as a **static** C++23 test library, an accepted exception to the general shared-library preference.

Source revision: `317ac1ed4c0bb6e6b91eafc817e05c488feffcb3`. Upstream SelfTests stay enabled.

### BCC64X finding 1: obsolete `__BORLANDC__` detection

Catch2 enables the legacy `CATCH_CONFIG_POLYFILL_ISNAN` whenever `__BORLANDC__` is set, causing an invalid `std::_isnan()` reference in `catch_polyfills.cpp` on BCC64X 20.1.7.

Catch2 itself supports a negative configuration override, so no source patch is necessary:

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
```

Release and Debug then compiled successfully.

### BCC64X finding 2: execution code page for narrow string literals

The `RunTests` upstream SelfTest still failed in `XmlEncode: UTF-8` because Catch2 places real Unicode characters such as `š` and `👾` directly in narrow `char` string literals and expects UTF-8 bytes.

For `š`, native minimal tests reported:

```text
BCC64X default                              -> 9A
-finput-charset=UTF-8                       -> 9A
-finput-charset=UTF-8 -fexec-charset=UTF-8  -> 9A
UTF-8 source with BOM                       -> 9A
\u0161 in ASCII source                      -> C5 A1
```

The installed BCC64X 20.1.7 exposes these Embarcadero-specific switches:

```text
-fborland-input-code-page=<code page>
-fborland-exec-code-page=<code page>
```

Control tests produced:

```text
-fborland-input-code-page=65001                           -> 9A
-fborland-exec-code-page=65001                            -> C5 A1
-fborland-input-code-page=65001 -fborland-exec-code-page=65001 -> C5 A1
```

The relevant setting is the **execution code page**, not input code page. The Catch2 contract adds:

```text
-fborland-exec-code-page=65001
```

The generic Clang `-finput-charset=UTF-8` and `-fexec-charset=UTF-8` switches were ineffective for this case and were not adopted into the Catch2 contract.

### Verification

With both required options:

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
-fborland-exec-code-page=65001
```

the isolated upstream test passed:

```text
1/1 Test #1: RunTests ... Passed
100% tests passed, 0 tests failed out of 1
```

Before correcting the code page, the same SelfTest contained **531 cases**: 507 normal successes, 16 expected failures, seven skips and exactly one genuine failing case with three assertions. Distinguishing the deliberate Catch2 `[!shouldfail]` and `[!mayfail]` output from the genuine UTF-8 failure was essential.

No Catch2 source patch was required.

### Architectural implication

`-fborland-exec-code-page=65001` remains **Catch2-local**. Globally changing the semantics of ordinary narrow string literals would require a separate explicit BCC64X toolchain policy decision.

---

## BitmapPlusPlus 1.1.1

A small modern C++ BMP library was introduced rather than recreating an older EasyBMP integration.

- Version 1.1.1, MIT licensed and header-only
- Upstream CMake offers only an INTERFACE target and no install or package config
- BuildEngine creates **no artificial binary build**
- Install headers and license and provide a small Admin CMake package configuration
- Run native BCC64X/C++23 Release and Debug consumer smokes

The consumer saves a BMP, reloads it and validates geometry and pixel values: actual compile and runtime usability, not merely an `#include`.

---

## Graphite2 1.3.15

Graphite2 used upstream CMake without a custom build adapter. The primary work concerned test portability and evidence. The historical `admin/patches/graphite2/1.3.15` set includes:

- `fix-nametabletest-langtags.patch`
- `fix-fnttxtrender-advance.patch`
- `diagnose-fnttxtrender-mismatch.patch`
- `fix-gr2fonttest-log-line-endings.patch`
- `fix-nametabletest-utf8-literals.patch`
- `add-managed-fonttools-path.patch`
- `fix-jsontest-log-line-endings.patch`
- `fix-bcc64x-padauk3-reference.patch`

The patches address test portability, references and diagnostics, encoding/line endings, and managed tool resolution. They are version-bound. After that correction round, **91/91 tests passed in each of Release and Debug**.

---

## Expat 2.8.4

Expat belongs in the **data** category as a streaming XML parser. Its use by TECkit/SFconv is a dependency relationship, not an alternative category.

Expat uses upstream CMake to build a shared package including `xmlwf`, headers and package metadata.

The `native-windows-tests.patch` corrects an incorrect upstream build-system assumption: classifying all non-MSVC compilers as POSIX and invoking `bash run.sh`. BCC64X is native Windows Clang; the correct test decision is **Windows versus non-Windows**, with direct test-executable launch on Windows. Parser source is unchanged.

---

## TECkit 2.5.13

TECkit lacked a suitable native upstream Windows/BCC64X build contract. Autotools and the upstream MinGW cross-build did not answer the project's native compilation question.

The Admin CMake adapter therefore imports only explicit target inventory from upstream `Makefile.am`:

- `source/Engine.cpp` → TECkit engine
- `source/Compiler.cpp` and `source/UnicodeNames.cpp` → compiler library
- Sample tools `teckit_compile` and `txtconv`
- `SFconv` with Expat

This is an explicit downstream build description; upstream C++ sources remain unchanged. zlib and Expat are actual package dependencies. Under the corrected FSM rule, TECkit build begins only after both dependencies complete `Install` and consumes their installed package surfaces.

The Windows-aware upstream test driver `test/dotests.pl` remains unchanged and tests all three tools, preserving functional evidence despite using a different build system.

---

## ICU4C 78.3

ICU uses BuildEngine ProjectImport as a **read-only** Windows project inventory, importing:

- `stubdata/stubdata.vcxproj`
- `common/common.vcxproj`
- `i18n/i18n.vcxproj`

The separation matters: VCXPROJ supplies source/resource inventory and target structure; BuildEngine supplies toolchain, defines, linker flags, variants and installation paths; CMake/Ninja/BCC64X builds the result.

MSBuild, MSVC, NMAKE, MSYS and `runConfigureICU` do not become part of the active route. The bootstrap produces `icudt → icuuc → icuin`. No full data-generator or upstream-test coverage is claimed for this bootstrap.

---

## Poppler 26.09.0

Poppler is built using upstream CMake with explicit zlib, FreeType, libjpeg-turbo, libpng and libtiff dependencies.

Some Windows SDK x64 headers expect the Microsoft architecture selector `_AMD64_`; BCC64X does not describe its target in exactly the same way. The Poppler-local profile therefore defines:

```text
-D_AMD64_ -DNOMINMAX
```

This compatibility definition is **not** added globally to the BCC64X toolchain without broader evidence.

The bounded Poppler profile disables optional Qt/GLib, Boost, OpenJPEG, LCMS, curl, NSS/GPGME, utilities and other backends. Release archives do not include the separate `poppler/test-data` repository, so upstream test-data provisioning needs its own reproducible contract. Missing test data is not justification for falsely claiming full upstream tests.

---

## Generic BuildEngine lessons from this integration round

### 1. Transitive runtime dependencies belong in package smokes

The smoke `PATH` must include recursively resolved runtime DLL directories, not just immediate dependencies. FreeType → bzip2 is one example. BuildEngine now resolves the runtime closure from the versioned dependency graph.

### 2. Bundled components must appear in the SBOM

Skia intentionally pins and builds some third-party sources internally. The external BuildEngine dependency graph alone is insufficient; metadata must include bundled components, distinguishing them from external packages.

### 3. Toolchain defects must not be hidden as library patches

The generic BCC64X UCRT defect was first observed through Skia but reproduced independently and fixed in the generic tool/link layer. Conversely, Catch2's obsolete `__BORLANDC__` detection is package-specific and stays in its contract.

### 4. Never fabricate a patch from guessed context

Generate versioned patches from the fully materialized exact upstream state and validate with `git apply --check`. Do not reconstruct source hunks from log excerpts or assumptions.

### 5. Distinguish upstream packaging defects from language compatibility

- HarfBuzz missing `find_dependency(Threads)` or `hb-subset-depend.h`: packaging/install issue
- Catch2 `std::_isnan`: obsolete upstream compiler-family detection
- Catch2 UTF-8 narrow literals: BCC64X execution code-page semantics
- Skia UCRT math imports: generic BCC64X runtime/linker problem
- GN `libs` versus absolute import libraries: build-system/toolchain adapter issue

This classification determines how later upstream version updates should be reassessed.

---

## Project files as input inventories rather than build-system boundaries

ICU revealed a reusable architectural idea: a native Windows project file can contain valuable upstream source/target inventory without becoming the BuildEngine build system.

Schema 16 therefore introduced ProjectImport inside `cmake mode="configure"`:

```text
Project metadata
   -> import adapter
   -> neutral target model
   -> generated CMakeLists.txt

BuildEngine contract
   -> toolchain
   -> build variants
   -> dependencies
   -> installation

both combined
   -> CMake
   -> Ninja
   -> BCC64X
```

Initial adapters: `vcxproj` and `cbproj`. Importing VCXPROJ does not switch to MSBuild/MSVC; importing CBPROJ does not modify the original project or make it a second build authority.

The approach is deliberately conservative: unresolved conditional source selection causes import to fail rather than generating a guessed target. The same model could project existing C++Builder metadata into a native CMake/Ninja/BCC64X build.

**Status at the time:** implementation and schema existed. ICU 78.3 was the first configured VCXPROJ example (`stubdata`, `common`, `i18n` → `icudt`, `icuuc`, `icuin`). A complete native BCC64X proof for this route and a real CBPROJ proof remained pending.

## Open verification items at the time

- Repeat the complete Catch2 Release/Debug workflow, including all 82 CTest cases, install, publish and two installed-consumer smokes after the final execution-code-page correction.
- Verify final Skia Debug smoke after UCRT correction from an actual log.
- Reassess why upstream tests were disabled for libpng, libtiff and libjpeg-turbo; record explicit technical rationale.
- Evaluate any global UTF-8 execution-code-page policy separately; until then, keep `-fborland-exec-code-page=65001` local to Catch2.
- Incorporate tool identities and other external build inputs more precisely into incremental state beyond library timestamps and dependency edges.

## Related documentation

- `docs/bcc64x-ucrt-runtime-link-bug.md` — reproduced BCC64X UCRT loader defect
- `docs/library-license-sbom.md` — cumulative licensing and CycloneDX
- `admin/build-libraries.xml` — authoritative package/dependency contract
- `admin/build-tools.xml` — tool and generated-tool contract, including `bcc64x-ucrt-compat`

## PostgreSQL client stack: libpq 18.6 and libpqxx 8.0.2

The PostgreSQL client stack is now part of the active database catalog.

### libpq 18.6

The official PostgreSQL C client library is built as a managed package against BuildEngine OpenSSL 3.5.8 and zlib 1.3.2. BCC64X-specific source repairs cover the Windows `ntsecapi` conflict and Winsock linkage without redirecting the build to another compiler.

Release/Debug build, install, metadata, publish and the installed consumer smoke have passed. The smoke validates the installed libpq API rather than only build-tree linkage.

### libpqxx 8.0.2

libpqxx is modeled as a package extension of libpq: it owns its own upstream source, build and package roots while the extension relation supplies the exact libpq dependency/evidence.

Important BCC64X findings were:

- Windows shared builds must not emit an empty `__declspec()`; the version-bound patch normalizes that path.
- `[[gnu::cold]]` / `[[gnu::hot]]` in upstream declaration positions are not accepted by BCC64X in the same way as generic Clang/GCC; BCC64X uses `__attribute__((cold))` / `__attribute__((hot))` for this path.
- CMake's MinGW-like platform inference must not rename the produced library as though BCC64X were MinGW/GCC.
- The v8 installed consumer header is `<pqxx/util>`, not the older `<pqxx/util.hxx>`.

The installed package consumer smoke is PASS.

## PDF/XML stack update: libxml2, QPDF, PoDoFo, win-iconv and Poppler

### libxml2 2.15.3

libxml2 is the managed XML foundation for XPath, XML Schema, Relax NG, XInclude and serialization. Its BCC64X integration exposed release-archive test-helper and Windows line-ending assumptions. A dedicated Relax NG recovery consumer smoke also proved that libxml2 2.15.3 can perform streaming validation recovery under BCC64X; this ruled out a generic libxml2 Relax NG failure while diagnosing PoDoFo.

### QPDF 12.4.1

QPDF is built as a shared package against managed zlib and libjpeg-turbo. The initial OpenSSL crypto path was deliberately removed because the managed OpenSSL build brings Brotli/Zstd runtime dependencies that are unnecessary for QPDF's native crypto path. The resulting contract keeps QPDF's runtime closure smaller and easier to audit.

### win-iconv 0.0.8

win-iconv is the explicit Windows iconv implementation required by Poppler's C++ API path. Its source/build/test/install/publish/doxygen and installed consumer smoke have been exercised successfully.

### Poppler 26.09.0

Poppler now builds in Release and Debug with `ENABLE_CPP=ON`; install/common/metadata/publish/doxygen/ready have passed in the active target-machine run. Its GPL licensing remains a product-architecture constraint even though the technical package build succeeds.

### PoDoFo 1.1.1

PoDoFo exposed two separate BCC64X lessons.

First, its Windows DLL export/import selection used `_MSC_VER` as the Windows branch selector. BCC64X is Clang and does not define `_MSC_VER`, so the upstream code incorrectly selected GNU visibility attributes. A version-bound BCC64X Windows branch now uses `__declspec(dllexport/dllimport)`.

Second, the investigation exposed a more fundamental toolchain problem: BCC64X EXEs and DLLs were previously linked without `-tR`, which means separate static C++ runtime/allocator domains. PoDoFo's public API returns ownership across the DLL boundary (for example `std::unique_ptr<PdfXMPPacket>` from `PdfXMPPacket::Create()`). That makes static runtime separation unsafe and explains access violations during destruction/shutdown.

The BuildEngine BCC64X rule is now explicit and global:

```text
ALWAYS dynamic BCC64X runtime (-tR)
NEVER static BCC64X runtime
```

The rule applies to console EXEs, GUI EXEs, DLLs and MODULE targets and is enforced by configure-time assertions. After this toolchain correction, the complete original PoDoFo upstream test set and the installed DLL XMP consumer are the required evidence; symptom-only test exclusions are not part of the intended final state.

## Generic BCC64X findings added in September 2026

### BCC64X is Clang, not MinGW

BCC64X may expose MinGW-compatible target/header conventions and macros because its Win64 environment uses the `x86_64-w64-mingw32` ecosystem. That compatibility surface must not be interpreted as compiler identity. The active compiler is Clang/LLVM with Embarcadero simulation. Third-party branches that equate `__MINGW64__` with a real MinGW/GCC toolchain must be reviewed and corrected where they select incompatible behavior.

### Dynamic runtime is a hard invariant

Static vs shared *library product form* is separate from compiler-runtime linkage. A test framework may legitimately be delivered as a static archive, but every BCC64X Windows executable/DLL/module still links the BCC64X runtime dynamically. This avoids independent allocator/STL/runtime domains across DLL boundaries.

### Patch construction is stateful

A third-party patch is not generated against a pristine upstream file by default. Its input is the complete file state that exists at its exact position in the ordered patch chain. The required workflow is:

1. materialize the complete original upstream file;
2. apply all earlier patches in contract order;
3. create the complete desired target file;
4. generate the unified diff mechanically;
5. reapply the generated diff to the complete input;
6. verify byte identity with the intended target;
7. only then commit the patch and advance evidence.

Hand-written hunk positions and fragment-derived patches are not accepted.
