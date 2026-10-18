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
