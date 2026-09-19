# BuildEngine-Admin – Technical Administration Data

**Status date:** September 19, 2026  
**Status:** active declarative contract for the Library-FSM architecture; BZip2/private-libarchive promotion pending verification

This directory contains the administration contracts that BuildEngine synchronizes and uses at runtime for tool provisioning, library builds, packaging, publishing, and small consumer smokes.

The normative library definition resides exclusively in:

```text
build-libraries.xml
```

Active libraries are not distributed across XML fragments.

## Current contract state

```text
build-libraries.xml : Schema 15
Library contracts   : current synchronized logical catalog
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
ace
tao
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

## Historical freeze evidence

The earlier 451/451 CURRENT run remains historical evidence for the pre-FSM architecture. It is not the current verification baseline and does not prove later FSM, documentation, extension, Publish or BZip2/libarchive changes.

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

Technical `id`/`dependsOn` graphs remain local to WorkItems. Fachliche progression and dependency barriers are owned by the Library-FSM, not by a global Action DAG.

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

The historical custom generic Python extraction path is no longer part of the active architecture. BuildEngine uses a private libarchive runtime with explicit in-process gzip, XZ and BZip2 filters. `.tar.bz2`, `.tbz2` and `.tbz` are accepted only when the BZip2 filter is in-process. The managed libarchive contract now consumes bzip2 1.0.8; the private runtime rebuild is still pending verification.

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

The library `timestamp` describes the revision of the effective logical contract and changes when that contract changes.

Persistent Current-State belongs to logical library scopes. Each successful scope records its own library timestamp plus the exact direct upstream completion identities required by that scope. Technical step markers, job IDs, fingerprints and runtime FSM states are not Current-State authority.

The historical 451/451 CURRENT run belongs to the pre-FSM baseline and must not be used as proof for the current contract.

## Current verification boundary

The old freeze rule has been superseded by the active Library-FSM implementation.

Verification remains staged. For the BZip2/libarchive transition the required sequence is:

1. build managed bzip2 1.0.8;
2. rebuild managed libarchive 3.8.9 with BZip2 enabled;
3. refresh the BuildEngine-private libarchive runtime;
4. rebuild/relink BuildEngine;
5. verify `[LIBARCHIVE] filter bzip2 : in-proc`;
6. restore the pinned Bash `.tar.bz2` tool contract.

A complete Clean-Room run remains a separate final evidence milestone.
