# BuildEngine-Admin TODO

**Status date:** September 8, 2026  
**Status:** frozen follow-up work until the final clean-room test

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
