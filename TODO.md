# BuildEngine-Admin TODO

**Status date:** September 22, 2026  
**Status:** active BuildEngine/Admin integration work; the September 8 freeze section below is retained as historical provenance

## First PDF/XML integration run: 22 September 2026

Observed target-machine evidence:

- [x] libxml2 2.15.3 source: PASS
- [x] libxml2 2.15.3 Release build: PASS
- [x] libxml2 2.15.3 Debug build: PASS
- [ ] libxml2 tests: failed because the official 2.15.3 release archive omits two CMake test helper files that are present in the tagged Git source. A version-bound patch now restores both helpers.
- [ ] QPDF Debug configure: failed because upstream searches for `jpeg` while the managed Debug package is `jpegd.lib`. The contract now supplies exact variant-specific dependency paths.
- [ ] QPDF Release build: reached compilation/linking, then failed in the upstream MinGW runtime-copy target for `libstdc++-6.dll`. BCC64X is now excluded from that MinGW-only post-build path by a version-bound patch.
- [ ] Poppler configure with `ENABLE_CPP=ON`: exposed the upstream mandatory Iconv dependency. win-iconv 0.0.8 is now a separate BuildEngine participant and Poppler consumes its exact variant package.
- [ ] PoDoFo remained blocked only because its new libxml2 dependency had not reached successful test/install evidence.

The next run must verify the fixes rather than introduce replacement compiler/toolchain paths.

## Second PDF/XML integration run: 22 September 2026

Observed target-machine evidence:

- [ ] libxml2 tests still executed against the previous source evidence and therefore still missed `run_and_diff.cmake`. The libxml2 library timestamp is now advanced so the source patch stage must be reconsidered on the next run; no state deletion is required.
- [ ] QPDF Release and Debug both reached the build stage. The hard-coded JPEG import-library paths were wrong: libjpeg-turbo's MinGW-style shared import artifacts are `libjpeg.dll.a` and `libjpegd.dll.a`, not `jpeg.lib` / `jpegd.lib`. The QPDF contract now uses those exact installed artifacts.
- [ ] win-iconv stopped in `patch-check` because the generated patch hunk length was malformed. The patch header is corrected and the win-iconv timestamp is advanced so source/patch evidence is invalidated naturally.
- [ ] Poppler and PoDoFo remained blocked transitively; no new compiler failure was observed.
- [x] License traffic-light metadata added to all BuildEngine libraries and mirrored on the relevant server documentation pages.

## Third PDF/XML integration run: 22 September 2026

Observed target-machine evidence at the supplied run cutoff:

- [x] win-iconv 0.0.8 source: PASS
- [x] win-iconv 0.0.8 Release build: PASS
- [x] win-iconv 0.0.8 Debug build: PASS
- [x] win-iconv 0.0.8 Release upstream test: PASS
- [x] win-iconv 0.0.8 Debug upstream test: PASS
- [ ] win-iconv install: failed only because the contract expected `iconv.lib`. Under the active BCC64X CMake model, the MinGW-style `lib` import-library prefix combines with BuildEngine's `.lib` import suffix, yielding `libiconv.lib`. Contract, smoke and Poppler dependency paths are corrected.
- [x] libxml2 2.15.3 source including the release-test-helper patch: PASS
- [x] libxml2 2.15.3 Release build: PASS
- [x] libxml2 2.15.3 Debug build: PASS
- [ ] libxml2 tests were still running at the supplied log cutoff; no result is inferred.
- [x] QPDF 12.4.1 patched source: PASS
- [ ] QPDF Release/Debug build: dependency path still used a MinGW `.dll.a` suffix. Under the active BCC64X CMake model the shared import libraries are `libjpeg.lib` and `libjpegd.lib`; the contract is corrected.
- [ ] Poppler remains transitively behind win-iconv install evidence; its Iconv path is updated to `libiconv.lib`.
- [ ] PoDoFo remains behind successful libxml2 test/install evidence; no PoDoFo compiler result is inferred.
- [x] License traffic lights are propagated through BuildEngine-Common into the dynamic HTML server pages and REST library metadata.

## Fourth PDF/XML integration run: 22 September 2026

Observed target-machine evidence:

- [x] win-iconv 0.0.8 source/build/test/install/metadata/publish/doxygen: PASS.
- [x] win-iconv 0.0.8 Release and Debug upstream tests: PASS.
- [ ] win-iconv consumer smoke failed because the smoke used a flattened `{ConsumerRoot}\lib\libiconv.lib` path while package-scope smokes consume the installed package layout. The smoke now uses `{ConsumerRoot}\lib\win64\{Configuration}\libiconv.lib`.
- [x] Poppler 26.09.0 Release build: PASS.
- [x] Poppler 26.09.0 Debug build: PASS.
- [x] Poppler 26.09.0 install/common/metadata/publish/doxygen/ready: PASS. This is the first complete BCC64X build evidence for Poppler with `ENABLE_CPP=ON`.
- [x] QPDF 12.4.1 Release build: PASS.
- [x] QPDF 12.4.1 Debug build: PASS.
- [ ] QPDF upstream tests: 0/7 passed. Upstream qtest's Windows path handling assumes MinGW/MSYS Perl, while BuildEngine deliberately provides Strawberry Perl. A version-bound native-Windows qtest adapter is added; result remains **[nicht verifiziert]** until the next run.
- [x] libxml2 2.15.3 source/build remains PASS in both variants.
- [ ] libxml2 tests now reach the real upstream output comparison. Failure occurs at the stderr comparison in `run_and_diff.cmake`; the helper now normalizes CRLF/LF before textual comparison. Result remains **[nicht verifiziert]** until the next run.
- [ ] PoDoFo remains blocked solely by libxml2 test/install evidence; no PoDoFo compiler result is inferred.

## Fifth PDF/XML integration run: 22 September 2026

Observed target-machine evidence:

- [x] win-iconv 0.0.8 complete including package consumer smoke: PASS.
- [x] QPDF 12.4.1 Release build: PASS.
- [x] QPDF 12.4.1 Debug build: PASS.
- [ ] QPDF upstream tests still report 0/7. Because `check-assert` is one of the seven and does not load QPDF binaries, the common failure is before the actual library tests. The QPDF 12.4.1 test adapter now pins every Perl invocation to BuildEngine's managed Strawberry Perl and keeps native Windows paths/semicolon-separated bindirs.
- [x] libxml2 2.15.3 source and Release/Debug builds remain PASS.
- [ ] libxml2 tests still fail at `run_and_diff.cmake:45`. Inspection of the Admin patch revealed that the previously intended CRLF/LF normalization had not actually been written to the patch file. The version-bound helper now really normalizes line endings and prints actual/expected text on mismatch.
- [ ] PoDoFo remains blocked only by libxml2 test/install evidence.

## Active PDF / XML stack

The following participants are now declared in the active Schema-16 BuildEngine stack:

- [x] Poppler 26.09.0: enable `ENABLE_CPP=ON`. **[nicht verifiziert]**
- [x] libxml2 2.15.3: add shared CMake/Ninja/BCC64X contract with zlib, XML Schema, Relax NG, XPath, XInclude, tests, package publish and consumer smoke. **[nicht verifiziert]**
- [x] QPDF 12.4.1: add shared CMake/Ninja/BCC64X contract using managed zlib, libjpeg-turbo and OpenSSL, upstream tests, package publish and consumer smoke. **[nicht verifiziert]**
- [x] PoDoFo 1.1.1: add shared CMake/Ninja/BCC64X contract using managed zlib, OpenSSL, FreeType, libxml2, JPEG, PNG and TIFF; use Win32 GDI font search and keep AFDKO/tools disabled in the first library profile. **[nicht verifiziert]**
- [ ] Run the new source/configure/build paths and correct only evidence-backed BCC64X incompatibilities.
- [ ] Run upstream tests for libxml2, QPDF and PoDoFo.
- [ ] Run Release package consumer smokes for all three libraries.
- [ ] After the first PASS, add Debug consumer smokes where they provide additional ABI/runtime evidence.
- [ ] Add BuildEngine-Tests scenarios for E-invoice embedded XML extraction, XML validation, PDF form discovery, field filling, save/reopen verification and printing adapters.
- [ ] Add exact source archive SHA-256 values where the upstream distribution publishes a stable digest and record them in the contract.
- [ ] Decide the concrete product/distribution architecture before Poppler becomes a mandatory proprietary in-process runtime dependency because Poppler is GPL licensed.
- [ ] Record the selected PoDoFo licensing path (MPL-2.0 or LGPL-2.0-or-later) for each distributable product rather than leaving the alternative implicit.

## Verified freeze baseline

The current unchanged follow-up run on the target machine produced:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0

Machine state summary
---------------------
jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

For the current Admin contract this confirms:

- 451/451 library tasks are correctly recognized as `CURRENT`,
- no library task is unnecessarily re-executed on an unchanged follow-up run,
- 0 failed,
- 0 blocked,
- 0 incomplete.

Functional baselines before documentation-only changes:

```text
BuildEngine       268504010b54245124005fde968400f57b6514b5
BuildEngine-Admin f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
```

The current contract uses:

```text
schemaVersion = 14
22 library/platform contracts
```

See `docs/FREEZE_CLEANROOM.md`.

## Freeze rule

No functional changes to the Admin contract are made until the complete clean-room test has finished.

Not permitted before the clean-room test:

- changes to `admin/build-libraries.xml`,
- changes to `admin/build-tools.xml`,
- XSD/schema changes,
- new or changed patches,
- new or changed CMake/toolchain adapters,
- changes to `admin/programs/`,
- new or changed package smokes,
- new libraries,
- changed library timestamps,
- changed source pins/hashes,
- changed build/test/install/publish parameters.

Only documentation, evidence, and handoff corrections remain permitted.

If the clean-room test finds an error, only the minimally required functional correction is made. A new freeze baseline is then established and the complete clean-room test is repeated.

## Before lifting the freeze

- [x] Document the 451/451 CURRENT follow-up run
- [x] Document schema/library state
- [x] Document freeze rules
- [x] Freeze follow-up TODO items
- [ ] Run the complete clean-room test in a fresh target environment
- [ ] Record the first complete run including commits, tool versions, patches, and machine state
- [ ] Immediately record the following unchanged second `--make` run
- [ ] Confirm that all library tasks are `CURRENT` on the second run
- [ ] Add the clean-room evidence to BuildEngine and BuildEngine-Admin

## Frozen library work after the clean-room test

### OpenCL

- [ ] Use historical BCC64X evidence as the starting point
- [ ] Separate headers/loader and vendor runtime cleanly
- [ ] Declare exact source pin and license
- [ ] Establish a minimal BCC64X compile/link contract
- [ ] Design the runtime gate so a missing vendor platform does not falsely invalidate the build contract

### GoogleTest

- [ ] Reconstruct the exact historically proven upstream/version state
- [ ] Deliberately reassess Shared vs Static
- [ ] Explicitly permit Static where technically more appropriate for test infrastructure
- [ ] Define a small test consumer

### Reassess the evidence field afterwards

After OpenCL and GoogleTest, no further libraries are added automatically. First evaluate whether additional packages provide new evidence about BCC64X, ABI, RTL, build-system integration, or toolchain compatibility.

## Frozen shared BuildEngine/Admin topics after the clean-room test

### Publish/consumer ownership

- [ ] Model ownership in the shared `Win64x` consumer tree
- [ ] Review publish manifests
- [ ] Never delete foreign package files as if they were stale files owned by the current package
- [ ] Avoid unnecessary publish repetitions

### Scheduler/DAG use

- [ ] Use new explicit action DAGs only after a successful clean-room test and after meaningful scheduler measurability has been introduced
- [ ] Do not pre-emptively parallelize library contracts
- [ ] Do not introduce type-dependent worker pools
- [ ] Control future parallelism through generic resources (`cpuBudget`, `{JobSlots}`)

### Security

- [ ] Add repository identities where automatic derivation is not sufficiently reliable
- [ ] Document a complete Schema-14 security-monitoring run after the freeze

### Documentation

- [ ] Update library/license/SBOM documentation with real evidence after the clean-room run
- [ ] Add new issues found by the clean-room test only as TODOs unless they must be corrected for PASS

## Currently included library contracts

```text
pugixml
zlib
brotli
zstd
xz
libzip
libarchive
openssl
curl
boost
nlohmann-json
ace-tao
bzip2
glew
opengl
raylib
sdl2
sqlite
xerces-c
soil2
vtk
opencv
```

This list must remain functionally unchanged during the freeze.

## Frozen project rules

1. BCC64X remains the actual target toolchain.
2. No silent replacement by MSVC, clang-cl, or MinGW.
3. Upstream build systems are preserved where possible.
4. `admin/build-libraries.xml` remains the single normative library/dependency contract.
5. Generic mechanics belong in BuildEngine C++; library knowledge belongs in XML/Admin.
6. Patches are version-bound and reproducible.
7. Tests are not disabled without analysis.
8. Release and Debug remain separate variants where meaningful for the library.
9. Shared DLL + import library is the default for normal runtime libraries, but not a dogmatic rule for test infrastructure.
10. Small package smokes and complex `BuildEngine-Tests` remain separate.
11. Project documentation is maintained in English for the wider project audience.
12. Source trees are regenerable artifacts; partially extracted sources must never be visible as a valid source tree.
13. All technical actions are treated equally by the scheduler; no type-dependent scheduler categories.
14. Schema 14 remains unchanged during the freeze.
15. Library timestamps remain unchanged during the freeze.

## Clean-room completion criteria

The freeze is lifted only when:

```text
complete first clean-room run: PASS
unchanged second run: PASS
all library tasks on second run: CURRENT
failed=0
blocked=0
incomplete=0
```

The repository commits, compiler/tool versions, source pins, and applied patches used must also be recorded traceably.
