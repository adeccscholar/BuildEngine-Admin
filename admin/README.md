# BuildEngine-Admin – Technical Administration Data

**Status date:** September 8, 2026  
**Status:** frozen contract state before the final clean-room test

This directory contains the administration contracts that BuildEngine synchronizes and uses at runtime for tool provisioning, library builds, packaging, publishing, and small consumer smokes.

The normative library definition resides exclusively in:

```text
build-libraries.xml
```

Active libraries are not distributed across XML fragments.

## Current contract state

```text
build-libraries.xml : Schema 14
Library contracts   : 22
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

Other central content:

```text
build-tools.xml    managed tool provisioning
smoke-tests.xml    transitional/compatibility file
schemas/           XSD contracts
cmake/             generic and library-specific CMake/toolchain adapters
patches/           version-bound source patches
programs/          technically justified special bridges
smokes/            small package-related consumer smokes
```

## Verified freeze state

The last unchanged follow-up run on the target machine with this functional Admin contract produced:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0
Machine state: jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

This confirmed 451/451 library tasks as `CURRENT`.

Functional baselines before documentation-only changes:

```text
BuildEngine       268504010b54245124005fde968400f57b6514b5
BuildEngine-Admin f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
```

Detailed freeze rules are documented in:

```text
../docs/FREEZE_CLEANROOM.md
../TODO.md
```

## Core principle

BuildEngine keeps library knowledge out of the C++ core. XML describes libraries; C++ implements generic mechanisms.

Preferred flow:

```text
Upstream / source pin
-> download and hash/identity verification
-> extraction
-> optional version-bound patch
-> original build system
-> BCC64X build
-> tests / validation where meaningful
-> install
-> require gates
-> publish
-> package smoke
-> ready
```

## Generic technical actions

Currently used generic action types include:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 14 additionally permits optional graph metadata:

```text
id
dependsOn
```

Semantics:

- if `dependsOn` is absent, the historical serial predecessor relationship remains in effect,
- explicitly empty `dependsOn` means no local predecessor dependency,
- explicit dependencies currently reference previously defined technical actions through stable IDs.

No new parallelization structures are introduced into library contracts until after the clean-room test.

## Extended copy semantics

Besides simple file/directory copies, `<copy>` supports generic packaging operations including:

- `recursive`,
- `overwrite`,
- include/exclude patterns,
- `flatten`,
- `cleanTarget`,
- `singleFile`.

This generic capability replaces the former ACE/TAO-specific Python packaging path.

## `<require>` and path kinds

Standalone `<require>` actions support:

```xml
<require path="..."/>
<require path="..." kind="file"/>
<require path="..." kind="directory"/>
<require path="..." kind="any"/>
```

`file` remains the compatible default. `directory` requires a real directory; `any` accepts any existing filesystem entry.

The nested `<extract><require path="..."/></extract>` remains file evidence for the extracted upstream artifact.

## Upstream, sources, and patches

Repository-managed compatibility patches are version-bound below:

```text
patches/<library>/<version>/
```

The flow remains:

```text
download upstream
-> verify identity
-> extract completely
-> check patch against exactly that state
-> apply patch
-> execute original build system
```

Source trees are regenerable artifacts. A temporary extraction tree must never become visible as a valid source tree.

## Native archive processing

The historical custom generic Python extraction path for `.tar.xz` is no longer part of the active architecture. BuildEngine uses its internally available libarchive/xz functionality.

## Technically necessary special programs

`programs/opengl/meson_bootstrap.py` deliberately remains. It implements concrete Mesa/Meson/BCC64X compatibility requirements and is not general packaging logic.

The former ACE/TAO Python installer is no longer part of the active contract.

## Package and publish contract

Versioned producer packages reside below:

```text
install/packages/<id>/<version>/
```

Publish maps those packages into the shared consumer tree:

```text
install/Win64x
```

The versioned producer tree remains authoritative. Complete ownership/manifest separation of the shared consumer tree is follow-up work after the clean-room test.

## Small consumer smokes

`smokes/<library>/...` contains small package-acceptance tests. Their only purpose is to prove the published consumer contract:

```text
configure
-> compile
-> link
-> small runtime path
-> PASS
```

Complex multi-process, integration, and demo scenarios belong in `BuildEngine-Tests`.

The ACE/TAO smoke is already implemented as a small IDL/ORB/Naming path.

## Product form

For normal runtime libraries, Shared DLL + import library is the preferred default where technically meaningful.

Static is permitted as a documented exception. GoogleTest will be deliberately reassessed for Static vs Shared only after the freeze.

## Incremental state

The library `timestamp` describes the revision of the effective contract. Library timestamps remain unchanged during the freeze.

Ongoing incremental-state authority resides in the BuildEngine core through technical step markers. The verified follow-up run with 451/451 `CURRENT` confirms the current contract.

## Freeze rule

No functional changes are permitted in this `admin/` tree until the complete clean-room test has finished.

In particular, do not change:

- `build-libraries.xml`,
- `build-tools.xml`,
- XSDs,
- patches,
- CMake/toolchain adapters,
- programs,
- smokes,
- source pins,
- hashes,
- library timestamps,
- build/test/install/publish parameters.

If the clean-room test finds a functional error, only the minimally necessary correction is made; the complete clean-room test must then be repeated against a newly documented freeze baseline.

## After the clean-room test

Only then resume:

- OpenCL,
- GoogleTest and the Static-vs-Shared decision,
- further security identities,
- publish ownership,
- new explicit DAG parallelization,
- further libraries only where they add new evidence.
