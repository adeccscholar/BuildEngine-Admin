# BuildEngine-Admin — Historical Contract Freeze Before the Final Clean-Room Test

**Snapshot date:** 8 September 2026  
**Status at that time:** CONTRACT FREEZE / clean-room candidate  
**Documentation language:** English

> Historical record: this freeze reflects the September 2026 contract, not the active November catalog. Its numerical claims and evidence are preserved as historical observations.

## 1. Purpose

This document records the frozen declarative Admin contract before a final complete clean-room test. Functional changes to the selected Admin revision were to be avoided until the test was complete.

It documents the role of the Admin repository, the verified shared BuildEngine/Admin baseline, the library and schema snapshot, binding rules for the toolchain, patches, tests and product types, the freeze boundary, follow-up work and clean-room evidence requirements.

## 2. Verified shared baseline

The last unchanged follow-up run on the target machine with that functional Admin contract produced:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0

Machine state summary
---------------------
jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

This verified **451/451 library tasks recognized as `CURRENT`**, no unnecessary library-task execution during that unchanged second run, and no remaining failed, blocked or incomplete jobs.

Functional BuildEngine revision:

```text
268504010b54245124005fde968400f57b6514b5
```

Functional Admin revision preceding the documentation-only freeze block:

```text
f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
Allow graph metadata on technical actions
```

Subsequent commits in the freeze block were restricted to documentation changes.

## 3. Role of the Admin repository

`BuildEngine-Admin` is the project's declarative administration layer. The BuildEngine core implements generic mechanisms; library-specific knowledge belongs in the Admin contract wherever possible.

```text
BuildEngine
   C++23 core, scheduler, actions, state, repository sync, publish orchestration

BuildEngine-Admin
   Tool/library contracts, XSD, patches, CMake/build-system adapters,
   security metadata, small package smokes

BuildEngine-Tests
   Complex integration, demonstration and learning projects
```

## 4. Normative library contract

The only normative library and dependency contract is:

```text
admin/build-libraries.xml
```

The **historical** contract snapshot declared:

```text
schemaVersion = 14
22 library/platform contracts
```

Included at the time:

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

OpenCL and GoogleTest were deliberately outside this frozen contract.

## 5. Technical action semantics

The Admin contract used generic actions including:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 14 allowed optional technical graph metadata such as `id` and `dependsOn`. These were generic metadata rather than library-specific scheduler logic. For backward compatibility, the historical serial action order applied in the absence of `dependsOn`; an explicitly empty value could declare an independent action; and explicit dependencies referenced already defined actions through stable IDs.

No new DAG structures were to be added to library contracts until the clean-room test concluded.

## 6. BCC64X contract

The project's explicit purpose was to establish evidence for C++Builder 13 / BCC64X.

1. BCC64X remains the actual target compiler.
2. Do not silently substitute MSVC, clang-cl or MinGW.
3. Preserve upstream build systems where possible.
4. Alternative toolchains may be used only as clearly separated comparisons or workaround experiments.
5. Analyze and document a build problem rather than hiding it behind a compiler switch.

## 7. Source and patch contract

```text
Download upstream source
-> verify source pin / hash / identity
-> extract completely
-> validate version-specific patch
-> apply patch
-> execute original build system
```

Patches are version-bound under `admin/patches/` and must be reproducible. Source trees are regenerable artifacts: when source state is invalid, replace the old tree. Temporary extraction trees must never be exposed as valid source.

## 8. Product form

For normal runtime libraries, shared DLL plus import library is the preferred product form when upstream and the library's purpose permit it. Separately, a strict toolchain rule requires the **dynamic runtime (`-tR`)** for every BCC64X Windows EXE, DLL and MODULE link. Static BCC64X runtime linkage is forbidden.

Static library output remains an admissible, explicitly documented exception. The GoogleTest decision was postponed until the freeze was lifted.

## 9. Tests and smokes

Do not disable upstream tests without technical analysis.

Package smokes under `admin/smokes/` should remain small and validate the real installed-consumer contract:

```text
configure
-> compile
-> link
-> small runtime path
-> PASS
```

Complex multiprocess integration, demonstrations and learning scenarios belong in `BuildEngine-Tests`. The ACE/TAO smoke already exercised a small realistic IDL/ORB/Naming route and was not intended to replace full CORBA testing.

## 10. OpenGL/Mesa compatibility bridge

`admin/programs/opengl/meson_bootstrap.py` was retained as an explicitly accepted technical compatibility bridge for Mesa/Meson/BCC64X, not as generic orchestration logic. It was not to be altered during the freeze.

## 11. Incremental state

The BuildEngine core owns incremental state. For Admin, the relevant rules were:

- The library `timestamp` represents its contract revision.
- Timestamps must not change during the freeze.
- Documentation-only edits outside the runtime contract must not trigger library rebuilds.
- The unchanged second run had recognized 451/451 library tasks as `CURRENT`.

## 12. Freeze rule

Until clean-room acceptance, only non-runtime-impacting Admin edits were permitted.

Allowed: documentation, evidence records and TODO/handoff clarifications.

Prohibited: changes to `admin/build-libraries.xml`, `admin/build-tools.xml`, XSD/schema, patches, CMake/toolchain adapters, `admin/programs/`, smokes, library inventory, library timestamps, source pins/hashes, or build/test/install/publish parameters.

If clean-room testing found a defect, only the minimal required correction was permitted. The new functional Admin baseline then had to be documented and the full clean-room test repeated.

## 13. Deferred work

Resume only after successful clean-room acceptance:

1. Bring OpenCL from historical BCC64X evidence into the active contract.
2. Bring GoogleTest from historical evidence into the active contract.
3. Explicitly decide and document GoogleTest static versus shared output.
4. Add security repository identities where automatic inference is insufficient.
5. Develop publish/consumer ownership together with the BuildEngine core.
6. Introduce further DAG parallelism only after measurement and resource-model work.
7. Add more libraries only when they provide additional BCC64X, ABI, runtime or build-system evidence.

## 14. Clean-room evidence requirements

The record must include at least:

```text
BuildEngine commit
BuildEngine-Admin commit
BuildEngine-Tests commit, if used
Schema version
Library contract list and count
Tool versions
Source pins and applied patches
SUMMARY of the complete first run
Machine state summary of the first run
SUMMARY of the unchanged second run
Machine state summary of the second run
```

The second run must reuse the unchanged library contract completely, without unjustified repetitions of source, build, test, install, publish or smoke stages.

## 15. Lifting the freeze

The historical freeze could be lifted only when:

1. The complete clean-room run succeeded.
2. All library gates passed.
3. There were zero `failed`, `blocked` and `incomplete` jobs.
4. A directly following unchanged run confirmed the library tasks as `CURRENT`.
5. Complete evidence was documented in both BuildEngine and BuildEngine-Admin.

## Additional hard rules established by later integration

These rules also apply to later clean-room repetitions:

- BCC64X is Clang/LLVM; MinGW compatibility macros and the `x86_64-w64-mingw32` target context are not proof of a MinGW/GCC toolchain.
- The BCC64X runtime must always be linked dynamically (`-tR`).
- Patch generation must start from the full effective file state at the specific point in the ordered patch chain. Create the complete desired target file, generate the diff mechanically, reapply and verify byte-identical output.
