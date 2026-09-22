# BuildEngine Contract `build-libraries.xml`

[TOC|Content]

**Status:** current declarative contract as of 22 September 2026. The active `admin/build-libraries.xml` and `schemas/build-libraries.xsd` use **schemaVersion 16**.

`admin/build-libraries.xml` is the executable declarative contract for C and C++ libraries managed by BuildEngine. It describes logical library identity, exact versions, dependencies, source acquisition/preparation, build variants, tests, installation, publication, smoke tests, metadata/security evidence, documentation and library extensions.

Related documents:

- [Library extensions](library-extensions.md)
- [Documentation pipeline](documentation.md)
- [Integrated libraries](libraries.md)
- [Tool contract](build-tools.md)
- [BuildEngine architecture](buildengine.md)
- [Server](server.md)

## Contract authority

The XML is the source of library/build knowledge. Runtime code evaluates this contract; it must not contain hidden per-library alternatives.

```mermaid
flowchart TD
    XML[build-libraries.xml schema 16] --> XSD[schemas/build-libraries.xsd]
    XML --> BE[BuildEngine]
    XML --> C[BuildEngine-Common LibraryCatalog]
    BE --> STATE[Logical scope state]
    BE --> ART[Artifact evidence]
    BE --> PKG[Physical payload]
    BE --> META[Metadata / SBOM]
    C --> S[Server / Manager / consumers]
```

Common/DLL is the leading shared interpretation for logical library/extension repository semantics. Applications do not reconstruct a second catalog from physical package directories.

## Basic structure

```xml
<buildLibraries xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:noNamespaceSchemaLocation="schemas/build-libraries.xsd"
                schemaVersion="16">
   <library id="example"
            version="1.2.3"
            category="network"
            timestamp="2026-09-15T00:00:00Z">
      <dependency library="zlib" version="1.3.2"/>
      <metadata name="Example Library"
                description="One-line statement of the library purpose."
                supplier="Example Project"/>
      <source>...</source>
      <build>...</build>
      <install>...</install>
      <publish>...</publish>
   </library>
</buildLibraries>
```

## `<library>` identity

| Attribute | Meaning |
| --- | --- |
| `id` | Unique logical library ID used by the Library-FSM, state, metadata, documentation and evidence. |
| `version` | Exact logical upstream version. |
| `category` | Stable inventory grouping. |
| `timestamp` | Logical contract-change token for this library. |

A logical library normally owns a separate physical package, but this is not mandatory. Extension libraries may share source, producer or installation areas.

A physical directory is never itself the logical identity.

## Metadata

`metadata/@description` is a concise one-line purpose description. It is not build status.

Metadata can feed:

- Common library catalog,
- CycloneDX component information,
- Server/Manager presentation,
- generated documentation.

License declarations are evidence/override data; declared upstream license text is not rewritten by BuildEngine.

## Dependencies

```xml
<dependency library="openssl" version="3.5.8"/>
<dependency library="zlib" version="1.3.2"/>
```

A normal library dependency is a **package dependency**. It is used by the Library-FSM, package prerequisites, metadata and SBOM.

From the consumer's `Build` state onward, every downstream consumer state requires each direct dependency to have completed its full `Install` state:

```text
dependency.state > Install
```

The shared Library registry evaluates each requirement as `satisfied`, `waiting` or `impossible`. `waiting` means the producer can still reach the required state. `impossible` means it has failed/stopped before that point, so the consumer becomes BLOCKED instead of waiting forever.

Dependency requirements are runtime gates only. A newer dependency `completedAt` is **not** persisted as a stale minimum for the consumer and does not by itself rebuild an otherwise current consumer.

BuildEngine may expose the already-installed package through `InstallRoot`, `PATH`, `CMAKE_PREFIX_PATH`, `CMAKE_INCLUDE_PATH` and `CMAKE_LIBRARY_PATH`. These paths are package consumption, not scheduler control.

Filesystem proximity alone never creates a dependency. Technical `ProcessJob::DependsOn` edges must not form a second global library DAG beside the FSM.

## Library extensions

Schema 16 supports a logically independent component that intentionally reuses a base library's physical structures.

```xml
<library id="tao" version="4.0.6" category="middleware" timestamp="...">
   <extension library="ace"
              version="8.0.6"
              source="TAO"
              producer="ACE_wrappers"
              install=".">
      <shared path="bin"/>
      <shared path="lib"/>
   </extension>
   ...
</library>
```

The extension edge is the direct base relationship and must not be duplicated as an ordinary dependency.

| Field | Meaning |
| --- | --- |
| `library` | exact base logical library ID |
| `version` | exact base version |
| `source` | extension source subtree below base source |
| `producer` | shared producer root below the base variant workspace |
| `install` | overlay root below base payload; `.` means same payload root |
| `shared/@path` | deliberately shared producer areas |

See [Library extensions](library-extensions.md).

## Source contract

Source Actions may include download, extract, copy, execute, target and other generic Actions.

Technical Actions remain execution/diagnostic units inside the logical `source` scope. They are not independent persistent Current-State files.

For an extension, shared source acquisition belongs to the base. An extension must not download/extract a second copy of the same upstream archive.

Variant-specific patches that must affect an already copied shared producer belong to the corresponding producer-preparation/build sequence, not to a late mutation of an unrelated source workspace.

## CMake project import

Schema 16 allows a CMake configure action to generate its source tree from existing project metadata before CMake itself is executed.

Example:

```xml
<cmake mode="configure"
       executable="{Tool:cmake}"
       source="{BuildRoot}\generated\cmake\example"
       build="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}">
   <import format="vcxproj"
           source="{Workspace}\example\example.vcxproj"
           target="example"
           type="shared">
      <include path="{Workspace}\example\include"/>
      <define value="EXAMPLE_BUILD"/>
      <link value="dependency_target"/>
   </import>
</cmake>
```

Supported formats:

| `format` | Imported inventory |
| --- | --- |
| `vcxproj` | `ClCompile`, `ResourceCompile` |
| `cbproj` | `CppCompile`, `ResourceCompile`, `RcCompile` |

Supported target types are `auto`, `shared`, `static` and `executable`. `auto` uses project metadata where that metadata is unambiguous.

Optional output attributes are `runtimeOutput`, `archiveOutput`, `debugPostfix` and `install`. Child elements can add target-local `include`, `define` and `link` information.

The import is intentionally narrower than MSBuild or the C++Builder project evaluator. BuildEngine does **not** import the compiler/toolchain contract from the project file. Platform, BCC64X, Release/Debug, install roots, dependency versions and common CMake arguments remain in `build-libraries.xml`.

Project files are read-only inputs. In particular, a `.cbproj` is never rewritten by this mechanism.

Source items with conditional inclusion or `ExcludedFromBuild` semantics are rejected instead of guessed. If such semantics are needed, they must be represented explicitly in the BuildEngine contract.

Generated CMake source trees belong below the build root and are build artifacts, not Admin source.

**Verification status:** the schema and both import adapters are implemented. The real BCC64X execution path is still **not verified** until BuildEngine is rebuilt with the new `ProjectImport.cpp` and representative VCXPROJ/CBPROJ cases pass.
## Build variants

Release, Debug and future configurations are variants of one logical build contract. Their number is not fixed.

Common arguments/environment belong in the common build node. Variant nodes contain only differences.

Build, Test, Validation and Install are logical upper states. The selected variant scopes form parallel branches below the upper state and share one exit barrier:

```text
Build
  +-- build:Debug -----------+
  +-- build:Release ---------+
  +-- build:<variant> -------+
                             |
                             +--> Test only when all are current
```

Separate variant directories permit parallelism without collisions.

Dependency mapping stays variant-correct:

```text
Release -> Release
Debug   -> Debug
```

## Tests and validation

Tests remain enabled unless a technical investigation justifies a deliberate exception.

Typical logical scopes include:

```text
test:Release
test:Debug
validation:Release
validation:Debug
```

## Installation

Compiled libraries typically use variant-specific and common install actions.

Install uses an explicit barrier:

```text
install:<variant>*  in parallel
        |
        v
install:common
        |
        v
install
        |
        v
Metadata
```

`install:common` must not run while any selected variant installation is still open. The aggregate `install` scope is the single shared exit of the Install upper state.

Shared DLL + import library is the preferred standard product shape where appropriate.

### Shared extension payloads

Extensions may overlay a base payload. Physical operations need the dependencies/barriers required by the **actual shared resource**, not automatically the strongest global library barrier.

For ACE/TAO this means:

- TAO compilation may proceed once its required ACE producer/artifact prerequisites exist;
- a later ACE global consumer-publish failure does not by itself invalidate the fact that TAO compilation started;
- an extension install/overlay must still not race an operation that is actively reading the same base-only payload.

Do not encode an unconditional `base ready + metadata` rule when only a narrower producer/install evidence dependency is physically required.

## Artifact inventory and ownership

Artifact jobs are Evidence helpers and do not create a second persistent Current-State hierarchy.

The **target contract** for a relevant artifact entry is:

- relative path;
- size;
- SHA-256;
- where needed, relationship/classification such as `present`, `created`, `modified`.

For an extension:

```mermaid
flowchart LR
    BEFORE[Base inventory] --> CMP{Compare}
    AFTER[After extension] --> CMP
    CMP --> CREATED[created]
    CMP --> MODIFIED[modified]
    CREATED --> OWN[extension ownership candidate]
    MODIFIED --> SHARED[shared / explicit handling]
```

`modified` is not automatically exclusive extension ownership.

### Known current implementation gap

`BuildEngine-Common/src/ArtifactManifest.h` currently stores path and size, but no SHA-256. Same-size content changes are therefore not yet detected correctly.

This implementation gap must be fixed before hash-safe cleanup or authoritative shared-payload ownership can be claimed.

## Publication

`<publish>` creates a consumer view. Publication and ownership are different concepts.

A consumable extension SDK may need files from its base closure, but including a base file in that consumer view does not make the extension its owner.

Publish must consume the central artifact/ownership model.

### Known current implementation gap

Current Publish code still interprets other publish manifests as collision ownership too strongly. Stale publication evidence can block a current producer, especially with shared extension payloads.

Publish manifests are publication Evidence, not an independent ownership authority.

## Smoke tests

Smoke tests validate the intended consumer/package contract. Their scope (`published`, package/payload or another declared contract) must match what is actually being proven.

A shared physical payload does not imply shared logical component identity.

## Security metadata

Repository/tag/commit identity and downloaded archive hashes are distinct evidence layers and remain separate from logical Current-State.

## Documentation

The central `build-documentation.xml` policy controls generated documentation.

A logical Doxygen scope produces HTML and optional LaTeX in one Doxygen run. Consumer publish is not semantically required merely to document an already installed/public API.

For collection libraries such as Boost, the current contract is `1 + N` logical Doxygen scopes:

```text
1 root scope, non-recursive
N first-level module scopes, recursive
```

There is no logical `collection-prepare` scope and no separate logical module-PDF scope.

See [Documentation pipeline](documentation.md).

## Persistent logical state

Persistent state belongs to logical scopes, for example:

```text
source
build:Release
install:Release
install
publish
metadata
doxygen
doxygen:root
doxygen:<module>
ready:Release
ready
```

Current canonical state:

```text
timestamp=<library timestamp>
completedAt=<completion timestamp>
state=completed
```

Historical `upstream=` and `fingerprint=` lines are migration-compatible input only.

Fingerprints, technical Action IDs, output hashes and output existence are not Current-State authority.

Old Evidence is never deleted before new work. On success BuildEngine atomically replaces only the completed Scope record. On failure it leaves the previous record untouched. Later pipeline states become stale through the normal temporal predecessor comparison; no downstream state deletion is required.

## Common repository semantics

The XML catalog is consumed by BuildEngine-Common. Extensions can resolve to:

```text
physical payload != logical metadata/SBOM root
```

Known current Common gaps before full extension verification:

- `LibraryExists()` / `VersionExists()` still use physical package paths;
- extension `installed` is inferred too strongly from base payload existence;
- PackageExporter is not yet ownership-safe for shared payloads.

These are Common corrections, not reasons for application-specific workarounds.

## Maintenance rules

When changing a library contract:

1. change only the affected logical library timestamp when its technical contract changes;
2. keep XML, XSD, Runtime, Common, applications and documentation aligned;
3. preserve exact dependency versions;
4. bind local patches to exact source versions;
5. validate XML before production execution;
6. do not substitute another compiler for BCC64X failures;
7. technical Actions do not become persistent State merely because they have IDs;
8. artifact evidence does not become Current-State;
9. physical paths do not define logical identity;
10. mark implementation **not verified** until the intended BCC64X/Common/Server run proves it.

## Current integration gate

The FSM architecture is active. The historical 15 September static no-run gate is no longer the architectural status of this contract.

Verification remains staged: targeted BCC64X runs first, complete Clean-Room evidence as a separate milestone.

For the current BZip2/libarchive transition:

1. build managed `bzip2 1.0.8`;
2. rebuild managed `libarchive 3.8.9` with `ENABLE_BZip2=ON`;
3. refresh the private BuildEngine libarchive runtime;
4. rebuild/relink BuildEngine;
5. verify `[LIBARCHIVE] filter bzip2 : in-proc`;
6. only then restore the pinned Bash `.tar.bz2` tool contract.


## CMake-Projektimport ab Schema 16

Schema 16 erweitert den bestehenden `cmake`-Knoten um optionale `import`-Elemente. Der Import läuft ausschließlich vor `mode="configure"` und erzeugt das CMake-Source-Verzeichnis, das anschließend vom normalen Configure-Schritt verwendet wird.

Beispiel:

```xml
<cmake mode="configure"
       executable="{Tool:cmake}"
       source="{BuildRoot}\generated\cmake\{LibraryId}"
       build="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}">
   <import format="vcxproj"
           source="{Workspace}\project\project.vcxproj"
           target="project"
           type="shared">
      <include path="{Workspace}\project\include"/>
      <define value="PROJECT_BUILD"/>
      <link value="dependency_target"/>
   </import>
</cmake>
```

Unterstützte Importformate:

- `vcxproj`
- `cbproj`

Unterstützte Zielarten:

- `auto`
- `shared`
- `static`
- `executable`

Optionale Zielattribute:

- `runtimeOutput`
- `archiveOutput`
- `debugPostfix`
- `install`

Optionale Ergänzungen innerhalb eines Imports:

- `include/@path`
- `define/@value`
- `link/@value`

Die importierte Projektdatei ist **keine zweite Buildautorität**. Compiler, Toolchain, Konfiguration, CMake-Argumente, Installationspfade und Library-Dependencies bleiben Teil des umgebenden BuildEngine-Vertrags.

Der Importer übernimmt derzeit:

`vcxproj`:

- `ClCompile`
- `ResourceCompile`
- `ConfigurationType` für die optionale automatische Zielarterkennung

`cbproj`:

- `CppCompile`
- `ResourceCompile`
- `RcCompile`
- `ProjectType` bzw. `Borland.ProjectType` für die optionale automatische Zielarterkennung

Nicht automatisch ausgewertet werden insbesondere komplexe MSBuild-/C++Builder-Conditions, importierte Property-Dateien und projektspezifische Tool-Tasks. Wenn einzelne Source-Einträge bedingte Buildsemantik besitzen, bricht der Import absichtlich ab. Solche Unterschiede müssen explizit im BuildEngine-Vertrag modelliert werden.

Das generierte `CMakeLists.txt` ist ein Buildartefakt. Die ursprüngliche `.vcxproj`- oder `.cbproj`-Datei wird nicht verändert.



### Aktueller ICU-Anwendungsfall

ICU 78.3 ist der erste produktive Einsatz des Schema-16-Imports. Der aktive Vertrag verwendet drei Upstream-VCXPROJ-Dateien ausschließlich als Inventarquelle:

```text
stubdata.vcxproj -> icudt78
common.vcxproj   -> icuuc78 / icuuc78d
i18n.vcxproj     -> icuin78 / icuin78d
```

Die logische Buildreihenfolge wird über die generierten CMake-Targets hergestellt:

```text
icudt -> icuuc -> icuin
```

Die Toolchain bleibt BCC64X/CMake/Ninja. Der frühere ICU-Pfad über `runConfigureICU MinGW`, Bash, `cygpath`, GNU Make und den nachträglichen MSYS-Pfadnormalisierer ist aus dem aktiven Vertrag entfernt.

Dieser erste Schritt ist bewusst ein Bootstrap-Build mit ICU-`stubdata`. Die echten ICU-Datengeneratoren und die vollständige Upstream-Teststrecke folgen erst nach erfolgreichem Nachweis dieses Core-Graphs.
