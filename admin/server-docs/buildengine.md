# BuildEngine

[TOC|Content]

**Status:** current architecture contract as of 20 September 2026. The Library-FSM architecture is active; individual integration capabilities can still be marked not verified until their real target-machine run is complete.

BuildEngine is a declarative C++23 orchestration system centered on Embarcadero C++Builder 13 / BCC64X. The current architecture separates the declarative library contract, the runtime Library-FSM, persistent logical scope state, technical WorkItems/Actions, artifact evidence, Common repository semantics and read-only presentation.

## Main principles

1. `admin/build-libraries.xml` and its schemas are the declarative library/dependency contract.
2. BCC64X is the intended toolchain; compiler substitutions are not silently introduced.
3. Every active `Library/Version` is controlled by a runtime state machine.
4. Persistent Current-State belongs to logical library scopes, not to the runtime FSM and not to technical Actions.
5. Technical Actions are execution/ordering/diagnostic units inside already released work.
6. Artifact inventories are evidence/ownership, not Current-State.
7. BuildEngine-Common is the leading shared interpretation for logical libraries, extensions, repository paths, security and package semantics.
8. A physical directory is never automatically a logical library identity.

## From the first DAG to the current FSM

The project started with a task/job model and a global dependency DAG. That model remains historically important and local technical Action graphs are still useful, but the global DAG is no longer the domain model of a library.

The current runtime compiles declarative contracts into one state machine per Library/Version:

```text
Source -> Build -> Test -> Validation -> Install -> Metadata -> Publish -> Documentation -> Ready
```

```mermaid
flowchart TD
    XML[Admin XML/XSD] --> COMP[DeclarativeStateCompiler]
    COMP --> DEF[LibraryDefinition]
    PERSIST[ScopeStateStore] --> EVAL[LibraryProgressEvaluator]
    DEF --> FSM[LibraryMachine]
    EVAL --> FSM
    FSM --> ORCH[LibraryOrchestrator]
    ORCH --> WORK[released WorkItems]
    WORK --> EXEC[technical ProcessJobs / workers]
    EXEC --> PERSIST
```

The FSM decides **whether** a state may progress and which logical scopes need work. The technical execution layer decides **how** that released work runs.

## Dependency hierarchy

For every direct dependency and every state `S >= Build`:

```text
downstream may work on S only when dependency Completed(S)
```

Examples:

```text
ACE Build requires OpenSSL > Build
TAO Build requires ACE > Build
```

The transitive ordering emerges from the direct XML edges and the individual FSMs. There is no second global transitive runtime closure.

## Orchestrator and technical workers

`LibraryOrchestrator` is the semantic single writer for Library-FSM state. It advances all machines to a fixpoint, releases work for reached states and consumes technical completions.

The existing ProcessScheduler remains useful as technical queue/worker infrastructure for already released jobs and for tool provisioning. It is not the persistent state authority and no longer defines the library lifecycle.

Local `id`/`dependsOn` graphs are allowed inside WorkItems where technical ordering needs them.

## Project import before CMake configure

Schema 16 adds a project-import layer directly to the existing CMake configure action. This is not a new FSM state and not a second build system. It is a technical preparation step inside already released Build work.

Supported input formats are currently:

- `vcxproj`: `ClCompile` and `ResourceCompile`;
- `cbproj`: `CppCompile`, `ResourceCompile` and `RcCompile`.

The imported file is read-only. BuildEngine does not modify `.vcxproj` or `.cbproj` files and does not invoke MSBuild or the C++Builder project system as part of the import.

```text
VCXPROJ / CBPROJ
      -> project inventory import
      -> neutral target model
      -> generated CMakeLists.txt
      -> existing CMake configure
      -> Ninja / BCC64X
```

Toolchain, variant selection, installation paths, dependencies and common CMake arguments remain owned by the surrounding BuildEngine contract. The imported project contributes source/resource inventory and target identity. Explicit `include`, `define` and `link` additions can be declared on the import where BuildEngine must supply target-specific information.

Conditional source inventory is deliberately not guessed. If an imported compile item contains a condition or `ExcludedFromBuild` semantics, the importer fails until that decision is represented explicitly. This keeps the generated build deterministic instead of silently emulating only part of MSBuild or the C++Builder project evaluator.

The implementation exists for both formats, but the capability remains **not verified** until the new BuildEngine source is rebuilt with BCC64X and real VCXPROJ/CBPROJ imports complete successfully. ICU is the first intended VCXPROJ proof.
## Logical scopes and Current-State

Typical scopes include:

```text
source
build:Release
build:Debug
test:Release
validation:Release
install:Release
install:Debug
install
metadata
publish
doxygen
doxygen:root
doxygen:<module>
ready:Release
ready:Debug
ready
```

Canonical state:

```text
timestamp=<library timestamp>
upstream=<library>|<version>|<scope>|<upstream library timestamp>|<upstream completedAt>
...
completedAt=<completion timestamp>
state=completed
```

A scope is current only when its own contract timestamp and the exact expected set of direct upstream completion identities still match.

Fingerprints, output files, command hashes, tree hashes, technical step markers and runtime FSM state are not Current-State authorities.

## Failure-safe lifecycle

For work that is actually submitted:

```text
invalidate old scope success
-> execute technical work
-> verify evidence/upstreams
-> commit new logical scope state on success
```

A failure leaves the scope stale. Old success is not retained as if the rebuild had never failed.

## Read-only check

`--check` uses the same declarative definitions and Library-FSM/orchestrator semantics read-only. It does not submit technical library ProcessJobs, invalidate scopes or commit state.

Dynamic collection documentation scopes are materialized through the same discovery when the reached Documentation state requires them.

## Documentation state

Documentation is a first-class Library-FSM state with hierarchical runtime substates.

```text
Documentation
  -> Standard
  -> Collection
  -> Linked
  -> Doxygen/PDF stages as applicable
```

This keeps dynamic Boost collection modules and linked ACE/TAO tagfile relationships inside the library lifecycle without creating a second persistent state hierarchy.

## Native archive capabilities

BuildEngine links a private libarchive runtime for native archive extraction.

The explicit compressed TAR filters are:

```text
gzip  : .tar.gz, .tgz
xz    : .tar.xz, .txz
bzip2 : .tar.bz2, .tbz2, .tbz
```

Only an in-process filter is accepted. `ARCHIVE_WARN` is treated as unavailable so libarchive cannot silently delegate decompression to an external program.

The startup banner exposes the effective runtime:

```text
[LIBARCHIVE] filter gzip       : in-proc ...
[LIBARCHIVE] filter bzip2      : in-proc ...
[LIBARCHIVE] filter xz         : in-proc ...
```

The BZip2 code path and managed libarchive dependency are implemented. The private BuildEngine libarchive world still has to be rebuilt and promoted before this capability is considered verified.

Until then the new pinned Bash `.tar.bz2` tool contract remains intentionally hidden in `build-tools.xml`.

## Library extensions

ACE/TAO remains the reference case for separate logical machines with deliberately shared physical producer/payload structures.

The extension relation supplies physical source/producer/install semantics. State, metadata, SBOM, documentation and ownership remain separate logical identities.

## Artifact evidence and Publish

Artifact evidence records what a logical component created or changed. Publish materializes a consumer view. Neither is a second Current-State system.

A base file included in an extension's consumer closure does not become extension-owned merely because it is published together.

## Common repository model

BuildEngine-Common resolves logical identity separately from physical payload and metadata placement. Server and Manager consume this common model instead of inferring library identity from directory names.

## Heartbeat and telemetry

Heartbeat output is library/FSM-oriented. Technical job activity remains available for diagnosis, but job counts are not treated as the domain model.

## Verification status

The FSM architecture, WorkItem integration and library-oriented runtime reporting are active source. Individual parts have already been compiled and exercised with BCC64X.

The newly added private-libarchive BZip2 path is **not verified** until the private runtime is rebuilt, linked and reports `filter bzip2 : in-proc` during a real run.
