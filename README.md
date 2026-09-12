# BuildEngine-Admin

This repository is the **declarative administration layer** of the C++Builder third-party integration project. The overall project aims to provide reproducible evidence of the extent to which current C and C++ libraries can be built, tested, packaged, and consumed by normal consumer projects with **Embarcadero C++Builder 13 / BCC64X**.

The project documentation is maintained in English so the project is accessible to the wider C and C++ community.

## Current status

The earlier freeze before the clean-room test was deliberately reopened in the BuildEngine core on September 8, 2026 for two final functions:

1. central Doxygen documentation based on the files actually published,
2. task-based performance evidence (`buildlog_*.log`).

The **Admin contract itself remains functionally unchanged** for that work. No library timestamps, source pins, patches, build parameters, smoke tests, or schemas were changed for those two functions.

Last verified runtime evidence before this core-function block:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0
Machine state: jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

This evidence is the baseline of the preceding functional state and must be reconfirmed after the Doxygen/performance extension.

## Role in the overall project

```text
adeccscholar/BuildEngine
   private C++23 application
   Scheduler, generic technical actions, repository synchronization,
   incremental state, publish, documentation, and smoke orchestration

adeccscholar/BuildEngine-Admin        <-- this repository
   declarative tool and library contracts
   XSD schemas, CMake/toolchain adapters, version-bound patches,
   security metadata, and small package-related consumer smokes

adeccscholar/BuildEngine-Tests
   more complex integration, demonstration, and learning environment
```

This separation is binding.

## Authoritative library contract

`admin/build-libraries.xml` is the **single normative library and dependency contract**.

Current state:

```text
schemaVersion = 14
22 library/platform contracts
```

Included:

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

OpenCL and GoogleTest remain follow-up work after clean-room completion.

## Core principle

```text
official upstream
-> reproducible download / source pin
-> extraction
-> explicit version-bound patch when required
-> original build system
-> BCC64X build
-> upstream tests where meaningful
-> versioned package
-> explicit require gates
-> publish into the consumer tree
-> small consumer smoke
-> central Doxygen documentation when enabled
```

An alternative compiler must never silently replace BCC64X.

## Generic technical actions

The current XML contract uses actions including:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 14 permits optional technical graph metadata (`id`, `dependsOn`). Without `dependsOn`, the historical serial predecessor relationship remains in effect.

## Publish manifest and Doxygen

Besides their ownership/incremental role, publish manifests now have another generic use in the BuildEngine core: with `WithDoxygen=true`, they form the **authoritative file list for public API documentation**.

For a published library, BuildEngine reads:

```text
<PublishRoot>\.buildengine\manifests\<library>.manifest
```

and passes the documentable, actually published header, IDL, and C++ module files from that manifest to Doxygen. This prevents the entire shared consumer tree from accidentally being attributed to one library.

Central output resides in the production root:

```text
documentation\index.html
documentation\<library>\<version>\html\index.html
```

`admin/build-tools.xml` already contains the managed Doxygen contract (currently 1.18.0); no tool/schema change was required for this extension.

## Copy and require

`<copy>` supports simple and filtered packaging operations, including recursive, overwrite, include/exclude, flatten, cleanTarget, and singleFile.

Standalone `<require>` nodes support:

```xml
<require path="..."/>
<require path="..." kind="file"/>
<require path="..." kind="directory"/>
<require path="..." kind="any"/>
```

The `<require>` nested inside `<extract>` remains file-based source evidence.

## Package-related smokes

Small smokes live below `admin/smokes/<library>/...`. They prove only the published consumer contract. Complex multi-process and integration scenarios belong in `BuildEngine-Tests`.

## Technically necessary special programs

`admin/programs/opengl/meson_bootstrap.py` remains a deliberately accepted Mesa/Meson/BCC64X compatibility bridge. Generic orchestration belongs in the BuildEngine core instead.

## Reproducibility and evidence

A credible clean-room proof should identify at least:

- BuildEngine commit,
- BuildEngine-Admin commit,
- BuildEngine-Tests commit where used,
- schema version,
- library version and source pin,
- compiler and tool versions,
- effective build parameters,
- applied patches,
- package/publish results,
- test/smoke results,
- central Doxygen output,
- `buildlog_*.log` as performance evidence,
- machine-state summary,
- an unchanged second run proving full CURRENT state.

## Next freeze

Before the complete clean-room test, the new core state is compiled locally with BCC64X and verified with `WithDoxygen=true`. A new common freeze baseline is then documented. Until then, Admin library contracts remain functionally unchanged.

## Important documents

```text
README.md
TODO.md
docs/FREEZE_CLEANROOM.md
docs/bcc64x-library-integration-findings.md
docs/bcc64x-ucrt-runtime-link-bug.md
docs/catch2-bcc64x-integration.md
docs/library-license-sbom.md
admin/README.md
```

The live project documentation served by BuildEngine Server is maintained under `admin/server-docs/` and uses the `[TOC|Content]` navigation directive.

## License

Project-owned content in this public Admin repository is licensed under the MIT License unless an individual file states otherwise. Third-party sources and their license texts retain their respective upstream licenses.
