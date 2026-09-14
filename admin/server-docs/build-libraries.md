# BuildEngine Contract `build-libraries.xml`

[TOC|Content]

`admin/build-libraries.xml` is the central declarative build contract for the C and C++ libraries managed by BuildEngine. It describes versions, dependencies, sources, build actions, installation rules, publication, smoke tests, security metadata, human-facing library metadata, and any remaining optional upstream documentation phases.

Related documents:

- [Tool contract `build-tools.xml`](build-tools.md)
- [Tool overview](tools.md)
- [Documentation contract](documentation.md)
- [Integrated third-party libraries](libraries.md)
- [BuildEngine configuration](configuration.md)

The schema is located at `admin/schemas/build-libraries.xsd`. The current contract uses `schemaVersion="14"`.

## Basic structure

```xml
<buildLibraries xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:noNamespaceSchemaLocation="schemas/build-libraries.xsd"
                schemaVersion="14">
   <library id="example" version="1.2.3" category="network"
            timestamp="2026-09-12T18:00:00Z">
      <dependency library="zlib" version="1.3.2"/>
      <metadata name="Example Library"
                description="One-line statement of the library's purpose."
                supplier="Example Project"
                homepage="https://example.invalid/"/>
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
| `id` | Unique logical library ID within BuildEngine. It is used by dependency references, scheduler job IDs, package paths and persistent state. |
| `version` | Exact logical upstream version represented by this library entry. |
| `category` | Optional stable inventory grouping such as `compression`, `graphics`, `middleware`, `testing`, or `data`. The public library overview uses the same grouping. |
| `timestamp` | Modification time of the library contract. It is the library-local change token used by logical scope state and must be changed only when the technical contract of that library actually changes. |

Version numbers should not be duplicated unnecessarily in build/install path constants when the path is already formed from contract variables. The version remains contract data and is inserted into paths through variables.

A `library` is primarily a **logical BuildEngine unit**. Usually that logical boundary also has its own workspace, build tree and package directory. It is not, however, a requirement that an upstream project physically writes into an isolated directory. When an upstream build system deliberately shares one producer tree between logical products, the XML may describe that physical relationship explicitly instead of inventing a filesystem split that upstream does not have.

ACE and TAO are the current important example: they are separate logical libraries for dependency/state/metadata/SBOM purposes, but TAO continues in the already prepared variant-specific ACE `ACE_wrappers` tree. `TAO_ROOT` is below `ACE_ROOT`, and the generated ACE and TAO artifacts share `ACE_wrappers\bin` and `ACE_wrappers\lib`.

## Dependencies

```xml
<dependency library="openssl" version="3.5.8"/>
<dependency library="zlib" version="1.3.2"/>
```

A dependency describes an explicit BuildEngine dependency. BuildEngine can derive DAG relationships, installation prerequisites, metadata, SBOM relationships, and documentation from it.

Dependencies must not be replaced by accidental file discovery: the contract remains authoritative. The execution DAG is also the basis for the persistent upstream state chain: downstream logical scopes remember the timestamp and completion identity of their direct upstream library scopes instead of maintaining an unrelated global dependency fingerprint.

Dependencies describe **logical ownership and ordering**, not necessarily independent upstream output directories. For example, TAO declares ACE as a direct dependency even though TAO's native build writes into the same physical `ACE_wrappers` producer tree prepared by ACE. Metadata/SBOM generation therefore reports ACE as TAO's direct dependency and OpenSSL/Xerces-C/zlib as transitive dependencies while the producer layout remains faithful to upstream.

## Metadata

```xml
<metadata name="Example Library"
          description="Client-side URL transfer library with HTTP(S) and related protocol support."
          supplier="Example Project"
          homepage="https://example.invalid/">
   <license name="MIT" spdx="MIT" file="LICENSE">
      <summary>Permissive MIT license.</summary>
   </license>
</metadata>
```

| Field | Purpose |
| --- | --- |
| `name` | Human-readable display name of the upstream library/project. |
| `description` | Concise one-line statement of the library's purpose. It describes **what the library is**, not its BuildEngine integration status. The same wording is suitable for library inventories, generated package information, and a CycloneDX component description. |
| `category` | Optional metadata-level grouping. The established contract-wide inventory grouping is `library/@category`; metadata category should not be used to create a conflicting second classification. |
| `supplier` | Upstream vendor, organization, or project responsible for the component. |
| `homepage` | Canonical upstream project/product page. |
| `license/@name` | Human-readable license name. |
| `license/@spdx` | SPDX identifier where unambiguous. |
| `license/@licensor` | Optional licensor. |
| `license/@file` | License file in source/package context. |
| `license/summary` | Optional short license summary/evidence text. |

The purpose description and the integration notes are deliberately separate. A description should remain useful even when compiler flags, patches, tests, or package layout change. Build-specific facts belong in `libraries.md` and the technical contract.

Contract metadata supplements automatically detected upstream evidence. Where BuildEngine has an explicit metadata override, the explicit value is authoritative for that field.

### One-line descriptions

Every managed logical library should have a concise `metadata/@description`. The current inventory is kept aligned with the descriptions in [libraries.md](libraries.md). Adding or splitting a logical library therefore requires updating the XML metadata and the documentation inventory together.

## ACE/TAO: logical split with shared physical output

The ACE/TAO integration demonstrates why logical and physical boundaries must not be confused. The intended contract shape is:

```xml
<library id="ace" version="8.0.6" category="middleware" timestamp="...">
   <dependency library="openssl" version="3.5.8"/>
   <dependency library="xerces-c" version="3.3.0"/>
   <dependency library="zlib" version="1.3.2"/>
   <metadata name="ACE"
             description="Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction."/>
   ...
</library>

<library id="tao" version="4.0.6" category="middleware" timestamp="...">
   <dependency library="ace" version="8.0.6"/>
   <metadata name="TAO"
             description="CORBA object request broker and middleware services built on top of ACE."/>
   ...
</library>
```

The two logical entries still originate from the ACE+TAO release family. The split means:

- `ace` and `tao` have separate scheduler identities and persistent state;
- TAO has the direct dependency `tao -> ace`;
- TAO metadata/SBOM therefore includes ACE directly and ACE's dependencies transitively;
- ACE owns preparation of the variant-specific `ACE_wrappers` producer tree;
- TAO continues building **inside that existing tree** rather than copying it into an artificial TAO build tree;
- `ACE_ROOT` denotes the shared `ACE_wrappers` root and `TAO_ROOT` remains `ACE_ROOT\TAO`;
- `ACE_ROOT\bin` and `ACE_ROOT\lib` are shared upstream output directories for both ACE and TAO;
- package installation may assign ACE-specific and TAO-specific artifacts to different logical packages, but that package ownership must not be mistaken for separate native producer directories.

This is an explicit modeling of the upstream build layout, not a generic recommendation that unrelated libraries should share output directories.

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

Actions can form a technical DAG through `id` and `dependsOn`. Without explicit graph metadata, the declared order applies. These Actions are execution units inside the logical `source` scope; they are not independent persistent-state authorities.

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

The model deliberately represents **one build contract with multiple variants**, not two independent build contracts. Persistent state preserves that distinction with logical scopes such as `build:Release`, `build:Debug`, `test:Release`, `install:Release`, and `install:common`.

### `testsAffectBuild`

`testsAffectBuild="true"` means that enabling or disabling tests changes the build configuration itself rather than only adding a downstream test phase. It should be set only where the generated build really differs as a result of the test setting.

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

Test and validation work is represented as logical variant-aware scopes where applicable. The technical Actions inside those scopes remain visible for ordering and diagnostics but do not each create persistent state files.

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

Installation describes the **package view**, not necessarily the exact shape of the native producer tree. In a shared producer layout such as ACE/TAO, install actions select the files that belong to each logical package from the common native output tree. They must not require the upstream producer to emit separate physical `bin`/`lib` directories when it does not do so.

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

The **central BuildEngine API documentation** is controlled by [build-documentation.xml](documentation.md). It defines the Doxygen profile, source visibility, exclusions, collection behavior, and PDF/LaTeX behavior.

In particular, when central PDF documentation is enabled, **the same Doxygen run produces HTML and LaTeX**. There is no second Doxygen run for PDF. Collection libraries can split the published API tree into root/module documentation scopes without changing this rule.

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

A variable should describe the current logical library unless the contract intentionally references another library's physical tree. ACE/TAO is such an explicit case: TAO can refer to ACE's known versioned producer path because the upstream products intentionally share `ACE_wrappers`; this must remain visible in the XML rather than being hidden behind an unrelated BuildEngine-wide abstraction.

## Logical persistent state

BuildEngine does not decide that a phase is current merely because its output files exist. Persistent state belongs to the logical library scope represented by the scheduler DAG.

A completed state has this form:

```text
timestamp=<library timestamp>
upstream=<library>|<version>|<scope>|<upstream library timestamp>|<upstream completedAt>
upstream=...
completedAt=<completion timestamp of this scope>
state=completed
```

The state is stored below:

```text
<ProductionRoot>/.buildengine/libraries/<library>/<version>/<safe-scope>.state
```

The important rules are:

1. The library's own `timestamp` is the local contract-change token.
2. Every direct upstream **library** scope contributes its library timestamp and `completedAt` value.
3. If the own timestamp differs, a required upstream state is missing, or one upstream timestamp/`completedAt` differs, the logical scope is not current and executes again.
4. Technical Actions inside the scope do not each create persistent state.
5. Existing artifacts, hashes, or output fingerprints are not an alternative persistent state authority.
6. Multiple direct dependencies produce multiple `upstream=` records.
7. `source` has no upstream library scope.
8. `--build` deliberately bypasses current-state reuse.

This directly links incremental reuse to the DAG that expresses the real execution dependencies. A changed upstream completion invalidates its downstream chain without requiring a global aggregate fingerprint that would rebuild unrelated libraries.

A shared physical producer tree does not merge logical state. After the ACE/TAO split, ACE and TAO retain separate state identities even though TAO continues in ACE's native output tree. TAO's state chain references ACE as upstream; the filesystem layout is not used as a substitute for that dependency relationship.

Legacy per-Action state files can still be recognized as migration evidence where their exact scope, timestamp, and expected Action count match. They are not the format written by new logical state commits.

## Change rules

When changing a library contract:

1. Change only the library that is actually affected.
2. Update that library's `timestamp` when its technical contract changes.
3. Keep `metadata/@description` as a concise purpose statement and update `libraries.md` with the same semantic description.
4. Do not duplicate version numbers unnecessarily in path constants. Explicit cross-library producer references are allowed only where the upstream physical layout genuinely requires them and must be documented.
5. Do not silently replace the BCC64X integration target with an alternative compiler/toolchain.
6. Do not disable tests without analysis.
7. Update source hashes, patch binding, and security evidence when the source version changes.
8. Extend this Markdown documentation when semantics or XML vocabulary change.

## Maintenance rule

`build-libraries.xml`, its XSD, [libraries.md](libraries.md), and this document are maintained together. A new XML feature is considered fully integrated only when its semantics are documented here as well.