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
