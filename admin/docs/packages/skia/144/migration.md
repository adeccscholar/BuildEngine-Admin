# Skia 144 for PDFium 2025.11.19 — independent BuildEngine package

**Status (2026-10-18): UPSTREAM PIN VERIFIED; NATIVE BCC64X DLL BUILD NOT YET PORTED OR VALIDATED.**

## Version evidence

PDFium commit `a84323421e94f484faca52dd9d027934eba42ab8` specifies
`'skia_revision': 'a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0'` in `DEPS`.

At that exact Skia commit, `include/core/SkMilestone.h` defines
`SK_MILESTONE 144`. The commit was authored on 2025-10-29 (UTC).

The established independent BuildEngine package `skia@153` is pinned to
`2eed75b956045eb8603d3690a1e84bc582a2135d`, whose
`include/core/SkMilestone.h` defines `SK_MILESTONE 153`.

**Pin the complete 144 commit, not an unpinned chrome/m144 branch head.**

## Independent package identities and installation contracts

| Property | PDFium-specific Skia | Existing Skia |
| --- | --- | --- |
| Library coordinate | `skia@144` | `skia@153` |
| Git commit | `a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0` | `2eed75b956045eb8603d3690a1e84bc582a2135d` |
| Package root | `{InstallRoot}/packages/skia/144/` | `{InstallRoot}/packages/skia/153/` |
| Headers | `include/` from the same 144 source | existing 153 package headers |
| Build profile | BCC64X, GN, Ninja, Debug and Release; shared DLL | unchanged, validated BCC64X GN/component profile |
| Runtime name | **proposed** `skia144.dll` | existing `skia.dll` |
| Import name | **proposed** `skia144.lib` | existing `skia.lib` |
| Validation | **PENDING** | existing package already validated |

The alternative runtime basename for 144 is intentional. Merely keeping two
different package directories is not sufficient if both `skia.dll` variants
must be loaded in one Windows process. Each Skia 144 component DLL in the
selected build profile must have a distinct runtime basename if a 153
counterpart can coexist. C++ objects, buffers and ownership must not cross
the two Skia ABI boundaries.

PDFium's GN `//skia` adapter must use both the 144 headers and the 144
import library. It must not compile the PDFium private Skia source set,
consume Skia 153 headers, or statically combine a second Skia implementation.
The rest of the established BuildEngine dynamic-library and `-tR` rules
remain mandatory.

## API evidence explaining the failed cross-version build

- The pinned Skia 144 tree contains `include/effects/SkGradientShader.h`,
  used by PDFium. Skia 153 instead publishes `SkGradient.h` and different
  gradient factory signatures.
- Both milestones have a `FillPathWithPaint` API, so the reported
  `no matching function` diagnostic does *not by itself* identify its precise
  cause. Resolve further compilation issues using the 144 API and full
  compiler diagnostic, not additional speculative 153 compatibility patches.
- The prior `bcc64x-skia153-gradient.patch` is a cross-version adaptation,
  not a valid patch when targeting the original Skia 144 API.

## Reproducible BuildEngine integration gates

1. Declare a second `<library id="skia" version="144">` entry in
   `admin/build-libraries.xml` only with its own reproducible BCC64X
   source, patch, build, install and test contract.
2. Download the 144 source at the full PDFium DEPS pin. Re-check all relevant
   Skia 144 DEPS revisions. At least expat, highway, ICU, libjxl,
   SPIRV-Headers, SPIRV-Tools, Vulkan-Headers and VulkanMemoryAllocator
   differ from the 153 source profile. Patches and build flags for 153 may
   not be recycled without applying them against the exact 144 sources.
3. Port and validate the GN/BCC64X toolchain and dynamic export targets
   specifically for 144. The current 153 `bcc64x-gn.patch` references
   `include/private/SkFloatingPoint.h` and `include/private/SkAttributes.h`,
   which do not exist at the PDFium Skia 144 pin; copying the 153 recipe
   unchanged is already known to be invalid.
4. Require separate Debug/Release `skia144.dll` and `skia144.lib`
   artifacts; install their exact 144 headers from the same build; register
   the needed runtime dependencies and CMake package contract.
5. Test the Skia 144 DLL and import library with a native BCC64X consumer.
   Verify DLL dependency closure, dynamic RTL, exports, and runtime loading
   **before** moving the PDFium dependency from 153 to 144.
6. Update PDFium's `<dependency library="skia" version="144"/>`, header
   staging, GN import library, runtime packaging, and compatibility checks
   together. Remove the Skia-153-specific gradient patch from its source FSM.
   Check the complete GN graph for accidental private Skia compilation.
7. Only then build and test PDFium against the version-correct dynamic ABI.

No claim is made that `skia@144` or `skia144.dll` is built, installed
or already present in the active FSM. This document records the exact pin
and a *controlled* second-package integration rather than registering an
untested package that would block the existing working stack.

## Upstream references

- PDFium DEPS:
  https://github.com/chromium/pdfium/blob/a84323421e94f484faca52dd9d027934eba42ab8/DEPS
- Skia 144 milestone:
  https://github.com/google/skia/blob/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0/include/core/SkMilestone.h
- Skia 153 milestone:
  https://github.com/google/skia/blob/2eed75b956045eb8603d3690a1e84bc582a2135d/include/core/SkMilestone.h

## Porting evidence — 2026-10-18

A version-specific patch was committed at:

`admin/patches/skia/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0/bcc64x-gn.patch`

It adapts the eight upstream Skia 144 GN/toolchain files to BuildEngine's
BCC64X driver and dynamically linked RTL. **All 28 patch hunks in eight
files were compared with the exact pinned 144 sources**, and match their
pre-images. It also names the principal library `skia144.dll`/`skia144.lib`
to avoid Windows loader basename collisions with Skia 153.

This proves patch preimage compatibility, **not** successful GN generation,
DLL linking, or ABI export completeness. The broader 153 desktop, GPU,
third-party dependency and generated component patches are not generally
applicable to Skia 144 and must be ported separately; they have deliberately
not been silently reused. The active XML library entry still needs its
own complete validated dependency closure and installation contract.

Do not mark Skia 144 as installed and do not switch PDFium to it until those
steps are completed. When registered, the Schema 17 attribute must be
`supersededBy="153"` on `<library id="skia" version="144" ...>`.


## FSM activation — 2026-10-18

Registered `skia@144` in `admin/build-libraries.xml` with
`supersededBy="153"`. The library has a dedicated pinned source, checked
BCC64X GN patch sequence, GN/Ninja Debug and Release DLL build,
and per-version header/import library installation. This is a small CPU/raster
profile intended for PDFium, not a reproduction of all Skia 153 desktop/GPU
features. Existing Skia 153 is unchanged.

PDFium `2025.11.19` now declares `skia@144`, with Skia headers and
`skia144.lib` from its installed package. Its Skia-153 gradient compatibility
patch is no longer executed. BuildEngine's dependency-install barrier must
now schedule and install Skia 144 before PDFium's build.

**Verification state:** GitHub XML and pinned upstream patch preimage review;
no Windows `gn gen`, `ninja`, DLL load or smoke evidence has been collected.
Build success must not be inferred. If the new 144 producer fails, PDFium is
correctly blocked rather than reverting silently to 153.


## PDFium link closure: PathOps and empty font manager (19 October 2026)

The uploaded Release `04-execute.log` reached `[604/604] BCC64X DLL ./pdfium.dll` without a preceding C++ compilation error and failed at link with **two** undefined symbols, not only the first diagnostic printed by the FSM summary:
- `SkFontMgr_New_Custom_Empty()`, required by `CFX_GlyphCache::GetDeviceCache()`.
- `Op(SkPath const&, SkPath const&, SkPathOp)`, required by `CFX_SkiaDeviceDriver::DrawPath()`.

Both APIs and their implementation sources exist in the **pinned** Skia 144 upstream tree. `SkFontMgr_New_Custom_Empty` is provided through Skia's optional `:fontmgr_custom_empty` GN target; `Op` comes from `:pathops`. Skia's present BuildEngine CPU/raster GN profile sets `skia_use_freetype = false`, which disables the default empty font manager, and the `skia` component's dependency list does not explicitly retain both interfaces for the external PDFium consumer. A successful independent Skia smoke did not prove these particular exports.

The new checked, revision-specific source patch `admin/patches/skia/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0/bcc64x-pdfium-symbol-closure.patch` adds `:pathops` and `:fontmgr_custom_empty` to the Skia DLL GN dependency closure. The Skia 144 XML build now explicitly enables `skia_enable_fontmgr_custom_empty` and configures the existing BuildEngine FreeType 2.14.3 include directory and variant-matched import library via `skia_use_system_freetype2`. The source patch runs with ordered Git preimage/reverse checks before Debug/Release builds. The PDFium Skia 144 dependency and independently installed DLL/import-library architecture are unchanged. **No private Skia objects are linked or recompiled inside PDFium.**

**Evidence boundary:** The source definitions and link errors support this correction, but exported PE symbol names, the transitive FreeType closure, successful Skia rebuild, and Debug/Release PDFium relinking remain **[not verified]** until the next native Windows run. If a GN dependency or export still fails, inspect its exact producer symbol table and generated GN graph; do not hide it with a PDFium-local shim or copied object file.
