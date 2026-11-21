# Library adoption: Assimp, CGAL, xtensor and OpenAL Soft

Date: 2026-11-21. Status: registered, native BCC64X builds and product licensing not verified.

## Scope and licensing

| Library version | Upstream licensing | Managed dependencies | Product suitability |
| --- | --- | --- | --- |
| Assimp 6.0.6 | BSD-3-Clause and notices for integrated upstream components | zlib 1.4 | Yellow until compiled importer/contrib inventory and notices are reviewed. Nonfree C4D, USD and Draco optional integration is not selected. |
| doctest 2.4.12 | MIT | none | Green, independently managed upstream unit-test dependency. |
| xtl 0.8.0 | BSD-3-Clause | doctest 2.4.12 (test only) | Green with attribution and license preservation. |
| xsimd 13.2.0 | BSD-3-Clause | doctest 2.4.12 (test only) | Green with attribution and license preservation. |
| xtensor 0.27.1 | BSD-3-Clause | xtl 0.8.0, xsimd 13.2.0, doctest 2.4.12 (test only) | Green for core use, including SIMD. |
| CGAL 6.2.1 | Individual files under LGPL-3.0-or-later or GPL-3.0-or-later | Boost 1.92.0 | Yellow: only the explicitly tested LGPL kernel usage may be considered for proprietary software, subject to LGPL obligations and a per-header license review. GPL algorithms require GPL-compliant distribution or a commercial license. |
| OpenAL Soft 1.25.2 | GNU Library General Public License 2 (upstream COPYING) | Windows platform APIs | Yellow: LGPL-family source, notice and relinking/replacement obligations apply for a proprietary consumer. Distribute as a replaceable DLL, not a monolithic static library. |

This is an engineering suitability assessment, not a legal opinion or a blanket proprietary-distribution approval.

## Declared integration

The authoritative XML registers all seven exact library/version coordinates, maintains separate producer and consumer evidence, preserves Release/Debug variants, packages license text and publishes the installed artifacts. No compiler substitutions or foreign-ABI binaries are part of the contract.

- `Assimp` uses the managed `zlib/1.4` package. Its actual compiled format/importer set, minizip and other bundled source must be inventoried from the configured build before redistributable approval. Tests are declared.
- `xtl`, `xsimd` and `xtensor` use upstream header-only CMake projects with no fabricated DLL. `xtensor` enables its independently managed SIMD layer. The separate MIT-licensed **doctest 2.4.12** package is registered for their upstream suites; those suites are selected when `TestsEnabled` is true. No upstream test result is claimed.
- `CGAL` uses the upstream archive, BuildEngine-managed Boost, `CGAL_DISABLE_GMP=ON`, and Boost.Multiprecision enabled. This removes the external GMP/MPFR build dependency for the selected configuration. The smoke only uses `CGAL::Simple_cartesian<double>` and squared-distance computation, not GPL triangulations. The actual compile/installed CMake contract must be checked.
- `doctest` is an independently installed MIT-licensed interface package, retaining its own native test selection and consumer smoke.
- `OpenAL Soft` uses its native Windows CMake build, an independently packaged DLL, utilities and upstream tests. Examples are excluded from the distributed SDK because they are separate applications and introduce optional dependencies. HRTF/sample-data rights must be reviewed before packaged redistribution.

All seven entries have separate Debug and Release native package consumer smokes under `admin/smokes/`, including actual Assimp OBJ import, numerical xtensor broadcast and CGAL 2D geometry.

## Verification and deployment gates

1. Validate the full XML against Schema 17, and the real archives against their declared source/extract requirements. SHA-256 archive verification and security audit are pending.
2. Execute the native BCC64X Release/Debug producer builds, upstream test gates and each package smoke. Resolve discovered compatibility problems without silently relaxing project policy.
3. Confirm actual installed header, DLL/import, exported CMake and runtime-closure names; the declarations are provisional until real output evidence confirms them.
4. Verify LICENSE-INFO and CycloneDX SBOM against the **compiled**, not merely present, third-party modules.
5. Do not mark Assimp or CGAL green before their component/source-license reviews; retain OpenAL Soft yellow until product-specific LGPL compliance has been decided.
6. Update documentation and provenance based on the actual target-machine results.

## HDR Photon Observatory showcase discovery

The registered `graphics/hdr-photon-observatory/showcase.json` exists on the current `adeccscholar/BuildEngine-Tests` main branch (commit `62c3cb27b`, 2026-11-21 04:29:08 UTC). The manifest is `kind=showcase` with Ceres/OpenEXR/Imath dependencies and both GUI and `--verify` applications.

The PostgreSQL `/showcases/` page does not fetch GitHub. The BuildEngine writer loads `<ProductionRoot>/tests/**/showcase.json` into `buildengine.showcase_cache` when `CatalogDatabase::RefreshPresentationCache()` runs; the server reader selects only records with `is_available=true`. The filesystem-only renderer similarly reads local `tests/` projects, not remote Git. The JSON therefore cannot appear from Git update alone.

Check the local repository checkout synchronization (if remote mode is enabled), that `<ProductionRoot>\\tests\\graphics\\hdr-photon-observatory\\showcase.json` exists, and execute the BuildEngine `--make` refresh (normal state-driven execution; no library rebuild is implied if states are current). Observe `[DATABASE] ...presentation caches refreshed` then refresh `/showcases/`. Verify the exact database identity with:

```sql
SELECT showcase_path, is_available, checked_at
FROM buildengine.showcase_cache
WHERE showcase_path = 'graphics/hdr-photon-observatory';
```

If absent after refresh, examine the configured test repository synchronization and production-root `tests` path. Do not fabricate a server-side Git auto-fetch, filesystem fallback or HTML special case. The updated remote repository alone is insufficient proof that a local Windows instance has synchronized.

## Upstream references

- https://github.com/assimp/assimp/blob/v6.0.6/LICENSE
- https://github.com/xtensor-stack/xtensor/blob/0.27.1/LICENSE
- https://github.com/xtensor-stack/xtl/blob/0.8.0/LICENSE
- https://github.com/xtensor-stack/xsimd/blob/13.2.0/LICENSE
- https://github.com/CGAL/cgal/blob/v6.2.1/Installation/LICENSE
- https://www.cgal.org/license.html
- https://github.com/kcat/openal-soft/blob/1.25.2/COPYING

## BCC64X compiler-compatibility corrections (2026-11-21)

The first native BCC64X attempts exposed three separate upstream compiler assumptions. The changes below are restricted to the exact pinned versions and are applied as checked, reversible source patches during the ordinary BuildEngine source state.

### Assimp 6.0.6

`include/assimp/defs.h` rejected the historical macros `__BORLANDC__` / `__BCPLUSPLUS__` unconditionally, even when `bcc64x` is the modern Clang frontend. Patch `patches/assimp/6.0.6/bcc64x-clang-compiler-detection.patch` keeps the unsupported classic Borland code path blocked but permits the Clang path via `!defined(__clang__)`. Native importers, threading and tests are not turned off. The existing managed zlib 1.4 import names (`libz.lib` Release, `libzd.lib` Debug) remain authoritative.

### OpenAL Soft 1.25.2

Upstream detects `Clang >= 17` and `Ninja` and enables `FILE_SET CXX_MODULES` without checking whether the exact compiler **driver and dependency scanner** are compatible. CMake 4.1.1 then invokes `clang-scan-deps` for BCC64X `.cppm` files; on this setup the scanner traverses incompatible header paths and reports missing `vcruntime_exception.h` or `sdks/_mingw_ddk.h`. This failure occurs during module scanning, before a meaningful native module build.

The BCC64X integration selects the supported upstream `ALSOFT_ENABLE_MODULES=OFF` fallback. **This does not disable OpenAL's audio mixer, spatialization, effects, Windows backends, HRTF processing, utilities or test suite**; it omits optional C++20 module interfaces. Module interoperability itself remains **unverified** and must not be advertised as passed. A later scanner/driver correction should restore modules explicitly, with dedicated native tests, rather than silently pretending they compiled.

Independently, an upstream Windows API compile-probe fallback sets `NTDDI_VERSION=NTDDI_VISTA`, inconsistent with the selected modern Windows SDK and `_WIN32_WINNT`. The `bcc64x-windows-sdk-version.patch` selects the coherent Windows 10 target pair `_WIN32_WINNT=0x0A00` and `NTDDI_VERSION=NTDDI_WIN10` only for the BCC64X integration when that fallback is needed; other platforms keep upstream behavior.

### doctest 2.4.12

When upstream tests are enabled, `scripts/cmake/common.cmake` enables `-Werror` and `-Weverything`. BCC64X's Clang 20 reports historical pointer arithmetic and C-runtime calls as `-Wunsafe-buffer-usage` and `-Wunsafe-buffer-usage-in-libc-call`. The version-specific patch retains **all** upstream test/example targets, warnings and remaining `-Werror` categories. It only adds `-Wno-error=unsafe-buffer-usage` and `-Wno-error=unsafe-buffer-usage-in-libc-call` under `ADECC_BCC64X`, so the diagnostic messages remain visible.

### Verification

All three diff hunks were compared against their exact upstream-tagged source files, including full old-side context and unified-diff hunk lengths. The source patch-check / patch-apply / reverse-check sequence is declared. The BCC64X producer builds, smoke tests and upstream ctests remain **not verified** until a subsequent BuildEngine log establishes the result.

CGAL 6.2.1 separately passed source, Release/Debug configuration, installation, metadata and publishing in the supplied run; the Doxygen scope was still in progress. No unrelated CGAL work or reconfiguration is warranted by these three failures.

## Follow-up compatibility corrections (2026-11-21, native rerun)

The BCC64X rerun confirms the strict smoke protocol now accepts the doctest,
xtl and xsimd consumers. All three report `ready` for the tested state.
CGAL 6.2.1 Doxygen was running when the operator interrupted the run;
no success, hang or runtime improvement is inferred from that heartbeat.

### Assimp 6.0.6: packed import structures

After the prior `defs.h` fix, Clang still entered a classic Borland branch
in `include/assimp/Compiler/pushpack1.h` and `poppack1.h`, because
BCC64X provides the historical Borland macros as well as `__clang__`.
The existing pinned source patch now guards the classic branch with
`!defined(__clang__)`, keeping the Clang packed-attribute path for
MD2, 3DS and all other importers. No importer and no warning category is
disabled. Check and apply the updated patch during the source state.

### OpenAL Soft 1.25.2: WASAPI bitwise overload

The Windows SDK used with BCC64X exposes an `AudioObjectType` bitwise
operation while upstream `wasapi.cpp` also declares its fallback overload
under `#ifndef _MSC_VER`. The result is ambiguous at the audio channel masks.
The new pinned `bcc64x-wasapi-audioobjecttype-operators.patch` avoids
declaring the duplicate fallback only for Clang with Borland compatibility
macros. Other non-MSVC toolchains keep the upstream fallback; WASAPI,
audio rendering and all test targets remain enabled.

### xtensor 0.27.1: managed JSON test dependency

The upstream CMake test targets precompile the full `XTENSOR_HEADERS` list,
including `xtensor/io/xjson.hpp`. The independently managed
`nlohmann-json/3.12.0` package supplies those headers but does not ship
the CMake package configuration expected by upstream `find_package`.
The XML now declares the explicit managed dependency, supplies its installed
include root, and applies a version-pinned source patch to expose the
header path to the complete upstream test suite, including the PCH and
JSON-specific tests. Neither the PCH nor the tests are switched off.

The five updated/new patch hunks for the three libraries were matched
against the exact upstream source text. Successful native BCC64X Release and
Debug builds, subsequent upstream tests, and runtime smoke results remain
**not verified** until a new Windows run reports their outcomes.

## Native follow-up: successful Assimp/xtensor builds and remaining gates

The subsequent BCC64X run confirms Assimp 6.0.6 and xtensor 0.27.1
Release and Debug build PASS. xtensor also reports upstream test PASS
in both configurations, plus install, publish and Doxygen PASS.

- Assimp install: On Windows, upstream CMakeLists.txt uses
  ASSIMP_LIB_INSTALL_DIR and ASSIMP_BIN_INSTALL_DIR, instead of relying
  on the CMAKE_INSTALL_LIBDIR and CMAKE_INSTALL_BINDIR settings alone.
  The XML now specifies the established per-configuration directories.
- xtensor smoke: A successful numerical result previously omitted the
  required SMOKE|CHECK records. The consumer now verifies both matrix
  shape and scalar broadcasting explicitly before the result record.
- OpenAL Soft WASAPI: Excluding the duplicate bitwise operator exposed
  an SDK overload that is not constexpr under BCC64X. The pinned patch
  now computes all channel masks using a constexpr helper operating on
  the underlying enum values. The helper was checked with Clang C++23;
  native BCC64X confirmation is still pending.
- CGAL Doxygen: The library profile retains DOXYGEN_RUNNING and all API
  and graph categories. It specifies four parser threads, two dot threads
  and QUIET=NO to make captured progress visible. Wall time alone is not
  proof of a hang.

No importer, audio backend, JSON test, or documentation category was
disabled. New producer/consumer acceptance still requires Windows logs.

## Further Windows run: integration corrections

Observed evidence from the later Windows host run:

- xtensor 0.27.1 reached READY, including both smoke consumers.
- Assimp 6.0.6 built and passed its declared upstream test stages in both configurations. Installation then failed because its XML `require` referred to a directory while the BuildEngine requirement validator expects a regular file.
- OpenAL Soft 1.25.2 passed the previous WASAPI operator and constexpr compilation problems, but its enabled `openal-info.exe` utility failed linking with an undefined `WinMain`.
- CGAL 6.2.1 Doxygen progressed through parsing and generated-code stages. No failure is established by this partial log.

Corrections now declared:

1. Assimp installation checks the concrete installed CMake package file `lib/win64/{Configuration}/cmake/assimp-6.0/assimpConfig.cmake` instead of treating the directory itself as a file.
2. OpenAL keeps `ALSOFT_UTILS=ON`. Upstream `openal-info` includes `common/win_main_utf8.h`, which exposes `wmain` for Windows Unicode arguments. The BCC64X-specific CMake source patch links the managed console-main-to-wmain bridge and Shell32, preserving Unicode argument conversion, the executable and its original behavior. The bridge pattern already exists in the managed libdeflate package.
3. The Showcase gallery improvements are made in the Tests assets and BuildEngine-Common renderer, independently from these package corrections.

These changes are source and configuration changes. Native BCC64X acceptance of the updated Assimp installation and OpenAL console utility is not yet verified.
