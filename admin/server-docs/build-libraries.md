# BuildEngine Contract `build-libraries.xml`

[TOC|Content]

`admin/build-libraries.xml` is the central declarative build contract for the C and C++ libraries managed by BuildEngine. It describes versions, dependencies, sources, build actions, installation rules, publication, smoke tests, security metadata, and any remaining optional upstream documentation phases.

Related documents:

- [Tool contract `build-tools.xml`](build-tools.md)
- [Tool overview](tools.md)
- [Documentation contract](documentation.md)
- [BuildEngine configuration](configuration.md)

The schema is located at `admin/schemas/build-libraries.xsd`. The current contract uses `schemaVersion="14"`.

## Basic structure

```xml
<buildLibraries xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:noNamespaceSchemaLocation="schemas/build-libraries.xsd"
                schemaVersion="14">
   <library id="example" version="1.2.3" timestamp="2026-09-12T18:00:00Z">
      <dependency library="zlib" version="1.3.2"/>
      <metadata .../>
      <security>...</security>
      <source>...</source>
      <build>...</build>
      <publish ...>...</publish>
      <smoke .../>
   </library>
</buildLibraries>
```

`library/@id` is unique within the contract.

## `<library>`

| Attribute | Meaning |
| --- | --- |
| `id` | Unique library ID within BuildEngine. |
| `version` | Exact version built by the contract. |
| `timestamp` | Modification time of the library contract. It is part of technical state and must be changed only when the technical contract of that library actually changes. |

Version numbers should not be duplicated unnecessarily in build/install path constants when the path is already formed from contract variables. The version remains contract data and is inserted into paths through variables.

## Dependencies

```xml
<dependency library="openssl" version="3.5.8"/>
<dependency library="zlib" version="1.3.2"/>
```

A dependency describes an explicit BuildEngine dependency. BuildEngine can derive DAG relationships, installation prerequisites, metadata, SBOM relationships, and documentation from it.

Dependencies must not be replaced by accidental file discovery: the contract remains authoritative.

## Metadata

```xml
<metadata name="Example Library"
          category="network"
          supplier="Example Project"
          homepage="https://example.invalid/">
   <license name="MIT" spdx="MIT" file="LICENSE">
      <summary>Permissive MIT license.</summary>
   </license>
</metadata>
```

| Field | Purpose |
| --- | --- |
| `name` | Display name. |
| `category` | Grouping in UI and documentation. |
| `supplier` | Upstream vendor/project. |
| `homepage` | Upstream project page. |
| `license/@name` | Human-readable license name. |
| `license/@spdx` | SPDX identifier where unambiguous. |
| `license/@licensor` | Optional licensor. |
| `license/@file` | License file in source/package context. |

This metadata flows into package information, SBOMs, and documentation.

## Security metadata

```xml
<security>
   <repository url="https://github.com/curl/curl.git"
               ref="curl-8_22_0"/>
</security>
```

`ref` typically identifies the upstream tag. An optional `commit` can be supplied when an exact commit is part of the security/source contract.

Security metadata does not replace the source hash. Repository identity, release tag/commit, and the downloaded archive are different evidence layers.

## `<source>`

The source phase consists of declarative technical actions. Supported actions include:

```text
download | extract | copy | execute | target
```

Actions can form a technical DAG through `id` and `dependsOn`. Without explicit graph metadata, the declared order applies.

### `<download>`

```xml
<download url="https://example.invalid/example-{LibraryVersion}.tar.xz"
          archive="example-{LibraryVersion}.tar.xz"
          sha256="..."/>
```

`sha256` is optional in the schema but should normally be present for reproducible external source downloads whenever upstream provides a stable release artifact.

### `<extract>`

```xml
<extract format="libarchive" root="example-{LibraryVersion}">
   <require path="CMakeLists.txt"/>
</extract>
```

Supported formats are `zip`, `gzip`, `gz`, and `libarchive`.

| Attribute | Meaning |
| --- | --- |
| `root` | Expected archive root. |
| `merge` | Merge content into an existing workspace. |
| `preserveCurrentArtifact` | Preserve the current action artifact for following steps. |

`<require>` verifies expected files/directories immediately after extraction.

### `<copy>`

```xml
<copy source="..." target="..."
      recursive="true" overwrite="true">
   <include pattern="**/*.h"/>
   <exclude pattern="**/test/**"/>
</copy>
```

Options include:

- `recursive`
- `overwrite`
- `flatten`
- `cleanTarget`
- `singleFile`
- `preserveCurrentArtifact`
- `test`
- `phase="test|validation"`

### `<execute>`

```xml
<execute name="configure"
         executable="{Tool:perl}"
         workingDirectory="{Workspace}"
         successExitCode="0">
   <environment name="NAME" value="VALUE"/>
   <argument value="..."/>
</execute>
```

`showOutput` controls whether process output is shown directly. The complete process run remains available through BuildEngine logging.

### `<target>`

`target` is the declarative evaluated process adapter for known build drivers.

Supported `type` values in the library schema are:

```text
generic, cmake, meson, perl, gmake, python, bmake
```

Example:

```xml
<target name="build"
        type="cmake"
        tool="cmake"
        workingDirectory="{Build}">
   <argument value="--build"/>
   <argument value="{Build}"/>
</target>
```

`tool` references a logical tool ID from [build-tools.xml](build-tools.md). An explicit `executable` can be used instead when the contract requires it.

`<parameter>` supplies driver-specific parameters. `<temporaryFile>` can materialize temporary text files from `<line>` entries. Double braces in target lines produce literal braces.

## `<build>` and variants

Build parameters that are common to all variants belong on the shared level. Variants contain only the differences.

```xml
<build>
   <environment name="CC" value="{Tool:bcc64x}"/>
   <argument value="-G"/>
   <argument value="Ninja"/>

   <target .../>

   <variant name="Release">
      <argument value="-DCMAKE_BUILD_TYPE=Release"/>
   </variant>
   <variant name="Debug">
      <argument value="-DCMAKE_BUILD_TYPE=Debug"/>
   </variant>

   <install>
      <perVariant>...</perVariant>
      <common>...</common>
   </install>
</build>
```

The model deliberately represents **one build contract with multiple variants**, not two independent build contracts.

### `testsAffectBuild`

`testsAffectBuild="true"` means that the test setting already affects the build fingerprint. It should be set only when the build configuration itself changes as a result of tests being enabled or disabled.

## Direct installation without a build

For header-only libraries or libraries that otherwise do not require compilation, a direct `<install>` block can be used instead of `<build>`. Supported actions there are `execute`, `target`, `copy`, and `require`.

## Test and validation actions

Several action types support:

```xml
test="true" phase="test"
```

or:

```xml
test="true" phase="validation"
```

This assigns technical actions semantically to a test/validation phase. Tests should not be disabled casually; failures are investigated first.

## Installation

For normal build contracts, installation is split into two levels:

```xml
<install>
   <perVariant>
      ...
   </perVariant>
   <common>
      ...
   </common>
</install>
```

`perVariant` processes Release/Debug-specific artifacts. `common` processes shared headers, CMake metadata, licenses, or other variant-independent files.

Shared libraries with DLL plus import library are the project default. Static artifacts, when additionally produced, must have clearly distinguishable names.

## Publication with `<publish>`

`publish` creates the consumable SDK/package view from the installed state.

```xml
<publish root="..."
         configuration="Release"
         consumer="cmake"
         requiresAllVariants="false">
   <tree source="include" target="include"/>
   <files source="bin" target="bin" extensions=".dll"/>
   <cmake source="lib/cmake" target="lib/cmake"/>
</publish>
```

Supported entries:

- `<tree>` – complete subtree
- `<files>` – file-type-based publication
- `<cmake>` – CMake package information

`optional="true"` deliberately allows an optional artifact to be absent. It must not be used to hide a build failure for a required artifact.

## Library-local smoke tests

```xml
<smoke id="basic"
       scope="published"
       source="..."
       configuration="Release"
       cmake="cmake"
       ninja="ninja"
       toolchain="..."
       executable="...">
   <argument value="..."/>
   <run executable="..."/>
</smoke>
```

`scope` is either `published` or `package`. Smoke tests validate the consumable state and must not succeed accidentally through internal build directories.

## `<documentation>` in the library contract

Historically, the library contract contains optional `doc` and `doxygen` phases. They are still supported independently until the affected upstream contracts have been cleaned up.

The **central BuildEngine API documentation** is controlled by [build-documentation.xml](documentation.md). It defines the Doxygen profile, source visibility, exclusions, and PDF/LaTeX behavior.

In particular, when central PDF documentation is enabled, **the same Doxygen run produces HTML and LaTeX**. There is no second Doxygen run for PDF.

## BuildEngine variables

Contracts use resolved variables instead of hard-coded machine paths. Typical examples are:

```text
{LibraryId}
{LibraryVersion}
{ProductionRoot}
{SourceRoot}
{BuildRoot}
{InstallRoot}
{WorkspaceRoot}
{BDS}
{Tool:<id>}
{ToolVersion:<id>}
{ENV:<name>}
```

The concrete variables available depend on the action context. Paths and commands should be assembled declaratively from these values.

## Technical state

BuildEngine does not primarily decide whether a step is current by looking at output files. Technical step-state records with library timestamp and fingerprint are authoritative. The existence of required result files is additional evidence.

Two rules follow:

1. An existing artifact without the matching step state does not automatically make a step current.
2. An independent contract change must not force all libraries to rebuild through a global aggregate fingerprint.

## Change rules

When changing a library contract:

1. Change only the library that is actually affected.
2. Update that library's `timestamp` when its technical contract changes.
3. Do not duplicate version numbers unnecessarily in path constants.
4. Do not silently replace the BCC64X integration target with an alternative compiler/toolchain.
5. Do not disable tests without analysis.
6. Update source hashes, patch binding, and security evidence when the source version changes.
7. Extend this Markdown documentation when semantics or XML vocabulary change.

## Maintenance rule

`build-libraries.xml`, its XSD, and this document are maintained together. A new XML feature is considered fully integrated only when its semantics are documented here as well.
