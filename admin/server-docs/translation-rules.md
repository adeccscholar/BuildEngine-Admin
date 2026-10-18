# Translation rules: Chromium/GN/MSVC to Embarcadero BCC64X

[TOC|Content]

**Status:** implementation inventory, 19 October 2026. **Scope:** shared `admin/programs/bcc64x/gn_argument_adapter.py`, the PDFium-specific GN toolchain and declarative source adjustments. Rules below describe the **implemented behavior**, not a claim of equal compiler semantics or a successful PDFium loader smoke. The source adapter and versioned XML remain authoritative if they change.

## Purpose and boundary

BuildEngine uses Embarcadero C++Builder 13 **BCC64X**, a Clang/LLVM-derived Windows x64 compiler **with its own driver, runtime and linking contract**. Chromium GN sometimes generates MSVC-spelled options even when `is_clang = true`. The adapter translates only explicitly known options; it does not switch to Microsoft's compiler, `clang-cl` or direct `ld.lld`. `is_clang = true` describes compiler family; `custom_toolchain = "//build/toolchain/bcc64x:bcc64x"` selects the driver. A GN flag, a compiler preprocessor branch and a platform ABI are **three distinct decisions**.

The adapter is a generic engine component. Library-specific choices, such as PDFium's installed Skia package and the exclusion of Chromium's own builtins archive, belong in the XML-defined library contract or versioned GN patch. The BuildEngine's FSM, not an unrelated external script or copied third-party source, owns preparation and execution.

## Compile argument mapping — exact implemented rules

| Input / rule | Output to BCC64X | Category and remarks |
|---|---|---|
| `/WX` | `-Werror` | Translate; diagnostics remain errors. |
| `/W3` | `-Wall` | Approximate warning group; **not** a verified equivalence to MSVC level 3. |
| `/W4` | `-Wall -Wextra` | Approximate warning groups; strict warning handling is preserved. |
| `/wd4244` | `-Wno-shorten-64-to-32 -Wno-implicit-int-conversion -Wno-implicit-float-conversion -Wno-float-conversion` | Explicit warning approximation. |
| `/wd4267` | `-Wno-shorten-64-to-32 -Wno-implicit-int-conversion` | Explicit warning approximation. |
| `/wd4800` | `-Wno-bool-conversion` | Explicit warning approximation. |
| `/wd4117` | no emitted argument | MSVC-specific diagnostic with no mapped Clang warning. |
| Other `/wdNNNN` | **error** | No automatic suppression of unknown numbered warnings. |
| `/Gy` | `-ffunction-sections` | Per-function sections. |
| `/Gw` | `-fdata-sections` | Per-data sections. |
| `/Oy-` | `-fno-omit-frame-pointer` | Preserve frame pointers. |
| `/O2` | `-O2` | Optimization. |
| `/Od` | `-O0` | Disable optimization. |
| `/Ob0` | `-fno-inline` | Disable inlining. |
| `/GF` | `-fmerge-all-constants` | Constant merging; exact MSVC pooling semantics not proven. |
| `/GR-` / `/GR` | `-fno-rtti` / `-frtti` | RTTI. |
| `/Zi` / `/Z7` | `-gcodeview` | CodeView generation; no claim of identical PDB layout. |
| `/utf-8` | `-finput-charset=UTF-8` | Source input charset; target execution charset is not covered. |
| `/Zc:sizedDealloc-` | `-fno-sized-deallocation` | Explicit language behavior. |
| `/guard:cf` | `-mguard=cf` | Request CFG instrumentation; validate resulting PE/loader. |
| `/guard:cf,nochecks` | `-mguard=cf` | Deliberately **does not** disable checks; no verified BCC64X nochecks equivalent. |
| `/MD`, `/MDd`, `/MT`, `/MTd` | `-tR` | **ABI decision, not equivalent CRT mode.** Uses dynamic Embarcadero RTL, never MSVC CRT; static/debug MSVC distinctions are not preserved. |
| `/arch:AVX2` | `-mavx2 -mf16c -mfma` | Per-source ISA selection, not `-march=native`. |
| `/arch:AVX512` | `-mavx512f -mavx512bw -mavx512dq -mavx512vl -mf16c -mfma` | Explicit subset of architecture features. |
| `/DNAME[=value]` | `-DNAME[=value]` | Preprocessor defines. |
| `/std:...` | `-std=...` | Standard selection; actual compiler support governs acceptance. |
| `/clang:<flag>` | `<flag>` | Forward embedded Clang option unchanged for native validation. |
| `/TC` | no emitted argument | Compile rule already identifies C source by extension. |
| `/FS` | no emitted argument | MSVC PDB synchronization does not apply as written. |
| `/bigobj` | no emitted argument | Native COFF backend handles extended section counts. |
| `/Zc:inline`, `/Zc:__cplusplus`, `/Zc:twoPhase` | no emitted argument | Clang standard-language behavior. |
| `/Brepro` | no emitted argument | Existing GN `-ffile-reproducible`/prefix controls used; **byte reproducibility not asserted**. |
| `-Wno-uninitialized-const-pointer` | no emitted argument | Exact newer-Clang warning suppression unsupported by pinned BCC64X; does not suppress any warning known to that compiler. |
| `-mllvm -pick-merged-source-locations` | no emitted arguments | Exact LLVM-internal tuning pair is unsupported by pinned BCC64X; other `-mllvm <option>` pairs remain intact. |
| Other non-MSVC options | unchanged | Compiler decides their validity. |
| Unknown `/<MSVC option>` | **error** | Never silently drop unknown MSVC switches. |

The documented command-line adapter also rejects a dangling `-mllvm` without its value. If a numbered warning or MSVC option is not in the explicit allowlist, the adapter reports it before invoking the compiler.

## Context-sensitive compile additions

These are **conditional behaviors present in the current adapter**, not global flags:

| Trigger | Addition / effect | Rationale |
|---|---|---|
| Any `-D__DATE__=`, `-D__TIME__=` or `-D__TIMESTAMP__=` in the translated arguments | Append `-Wno-builtin-macro-redefined` | Keep upstream reproducible-build values while silencing only the predefined macro redefinition warning. |
| Any `SK_USER_CONFIG_HEADER=` argument | Append `-D__forceinline=inline __attribute__((always_inline))` | Skia configuration explicitly requires always-inline interpretation. |
| Source path ends in `/src/opts/SkOpts_hsw.cpp` and `-mfma` absent | Append `-mfma` | Isolate HSW's FMA requirement to its implementation unit. |

These conditions are not a substitute for checking which C++ preprocessor branch is actually selected. In particular `_MSC_VER`, `__clang__`, `__BORLANDC__`, `_WIN32` and GN `is_clang` must be investigated independently if a source-path incompatibility appears.

## Link response-file translation

The same script supports `--link-rsp` to process GN's Windows/MSVC-flavored linker response file **before** invoking `bcc64x`. It currently:

1. Converts GN's `-lD:\\...\\package.lib`, `"-lD:\\...\\package.lib"` and `-l"D:\\...\\package.lib"` forms to **literal drive-qualified input paths**, normalized to forward slashes. These must not be resolved via the generic `-lname` lookup.
2. Converts exact system names such as `-ladvapi32.lib` to `-ladvapi32`; `-l:<file>` and path-qualified inputs are retained.
3. Converts unquoted MSVC linker switches beginning with `/Name` to `-Xlinker -Xlink=/Name`. This preserves comma-bearing values such as `/OPT:REF,ICF` without using `-Wl,` comma splitting.
4. Rejects **quoted raw MSVC-style linker switches** and any remaining raw `/Name` input after conversion; it does **not** claim that the target LLD driver supports every forwarded switch.
5. Writes a sibling `.bcc64x` response file and invokes the BCC64X driver with the translated response path. Original GN response evidence is retained.

The PDFium GN toolchain performs DLL linking through `bcc64x -tD -tR`, emitting the import library via `-Wl,--out-implib,...`. Executable linking also uses BCC64X; static archives use the provisioned LLVM archiver. This is distinct from launching bare `ld.lld`.

## Library-specific contract adaptations: PDFium 2025.11.19 and Skia 144

| Concern | Implemented rule / location | Why / evidence boundary |
|---|---|---|
| Compiler identity | `is_clang = true` in PDFium GN args **and** `build/toolchain/bcc64x/BUILD.gn` | Correct Clang family; keeps explicit Embarcadero driver. |
| Chromium private compiler runtime | Source preparation adapts `build/config/compiler/BUILD.gn` to avoid `//build/config/clang:compiler_builtins` only for `//build/toolchain/bcc64x:bcc64x` | Chromium's Clang 22 `clang_rt.builtins-x86_64.lib` is not the BCC64X runtime. |
| GN source options | Version-bound preparatory XML actions, guarded upstream transformations and adapter | Do not hide unsupported vendor-specific flags by setting `is_clang = false`. |
| PDFium Skia dependency | `skia@144` matches the pinned Skia revision; `CB_PDFIUM_SKIA_INCLUDE` and `CB_PDFIUM_SKIA_LIB` target installed package | No source checkout/recompilation or header staging inside PDFium. |
| Skia header path | `skia144-installed-includes.patch` rewrites upstream `third_party/skia/include/...` to installed-root-relative `include/...` | Include root is `packages/skia/144/include`, with headers in `include/include` and skcms under `include/modules`. Shared `cfx_dibbase.h` is also patched. **Complete include inventory remains unverified.** |
| Skia DLL symbols | Skia 144 GN dependency patch adds PathOps and empty font manager; `skia_use_freetype = true` and external FreeType 2.14.3 paths | Resolve `Op(...)` and `SkFontMgr_New_Custom_Empty()` from installed Skia DLL; no copy of Skia objects into PDFium. |
| Abseil TLS identity | `ABSL_FORCE_THREAD_IDENTITY_MODE=2` on PDFium C++ commands | Enforces the intended C++11 `thread_local`/RAII identity path rather than an unsuitable pthread-based branch. |
| Native compile and link | Custom `bcc64x.BUILD.gn` with `CB_BCC64X`, `CB_LLVM_AR`, `CB_PYTHON`, `CB_GN_ARGUMENT_ADAPTER`, `CB_BDS` and runtime compatibility archive | Explicit native toolchain and checked prerequisites. |
| PE runtime evidence | PDFium loader smoke probes `skia144.dll` before `pdfium.dll`; Skia 144 consumer smoke checks raster, font manager, PathOps | Skia loaded in supplied native evidence, while PDFium had returned Win32 193; Clang-identity build is **not yet proven to pass the loader smoke**. |

The PDFium source changes are maintained as versioned, checked patches and XML-defined actions, not as post-build header copies. A successful GN generation or native compile alone does not establish ABI correctness, exported symbols, usable import libraries or successful Windows loading.

## Validation and change discipline

- **Classify:** option translation, compiler-defined language behavior, third-party-specific GN configuration, linker response syntax, runtime/ABI decision or diagnostics-only option.
- **Compare evidence:** preserve upstream input option, translated output, invoked executable, and exact compiler diagnostic. A successful adapter run is not proof of a successful C++ compile.
- **Fail closed:** do not add generic ignore lists such as `-Wno-unknown-warning-option`; do not silently strip `/guard:cf`, `/WX`, unknown `-mllvm` sequences, or alter Debug/Release CRT contracts.
- **Check security/ABI:** `/guard:cf,nochecks` is currently strengthened to `-mguard=cf`, not semantically reproduced. PE CFG/TLS metadata and loader behavior remain separately testable. `/MT` to `-tR` is a toolchain policy, **not** a promise of MSVC CRT equivalence.
- **Keep XML authoritative:** a change to the library's compile/link contract requires updating that library's logical `timestamp` to invalidate persisted FSM evidence. Do not delete completed states arbitrarily.
- **Test both variants:** run native Debug/Release builds, installed-package consumer smoke, and the PDFium DLL loader smoke. Record successes and failures with file/version/variant logs.
- **Document provenance:** for every new rule, record originating flag, target compiler version, exact translation or omission, reason, evidence and verification status here; add library-specific detail to its `migration.md`.

## Source of truth

- `admin/programs/bcc64x/gn_argument_adapter.py` — currently implemented compiler and linker argument transformations.
- `admin/patches/pdfium/2025.11.19/bcc64x.BUILD.gn` — dedicated GN toolchain and driver commands.
- `admin/build-libraries.xml` — versioned source/build/test contracts and logical timestamps.
- `admin/patches/pdfium/2025.11.19/` and `admin/patches/skia/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0/` — pinned source patches.
- [PDFium migration](../docs/packages/pdfium/2025.11.19/migration.md) and [Skia 144 migration](../docs/packages/skia/144/migration.md) — detailed variant-specific history and evidence.
