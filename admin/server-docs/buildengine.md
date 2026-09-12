# BuildEngine

[TOC|Content]

BuildEngine is a declarative build orchestration system for reproducible C and C++ third-party library builds. Its current Windows integration is centered on Embarcadero C++Builder and the modern BCC64X toolchain. Library knowledge belongs in synchronized XML contracts, while the executable evaluates those contracts, prepares tools, creates a technical dependency graph, executes technical steps, and records the resulting state.

Related reference documents:

- [Configuration and command line](configuration.md)
- [Tool contract `build-tools.xml`](build-tools.md)
- [Library contract `build-libraries.xml`](build-libraries.md)
- [Documentation contract](documentation.md)
- [Tool overview](tools.md)
- [BuildEngine Server](server.md)

## Main responsibilities

BuildEngine separates configuration, orchestration, execution, evidence, and presentation:

1. **Configuration** resolves the production root, tools, repositories, build contracts, concurrency, tests, and documentation settings.
2. **Repository synchronization** updates the administration content before build evaluation.
3. **Tool preparation** discovers or installs tools described by the administration contract.
4. **Library contracts** define source acquisition, build variants, installation, publication, smoke tests, metadata, and documentation.
5. **The process scheduler** executes the technical dependency graph with a bounded worker count.
6. **Technical step state** is the authoritative incremental state for library work.
7. **Metadata generation** creates license information and CycloneDX SBOM data.
8. **Documentation generation** publishes library information and, where enabled, one central Doxygen run that produces HTML and optionally LaTeX; MiKTeX compiles the already generated LaTeX tree to PDF as a separate downstream state.
9. **The server** presents resulting packages, documentation, SBOMs, usage information, and security evidence without becoming a second build-state authority.
10. **Project Markdown documentation** is maintained together with the code and XML contracts and is rendered live from the synchronized Admin repository.

## Execution flow

```mermaid
flowchart TD
   A[Read BuildEngine configuration] --> B[Prepare bootstrap tool]
   B --> C[Synchronize administration repositories]
   C --> D[Load managed tool contract]
   D --> E[Prepare required tools]
   E --> F[Load library build contracts]
   F --> G[Create library DAG]
   G --> H[Evaluate technical step state]
   H --> I{Current?}
   I -- yes --> J[Validate evidence]
   I -- no --> K[Execute required technical actions]
   K --> L[Install and publish artifacts]
   L --> M[Generate metadata and documentation]
   M --> N[Run configured smoke tests]
   J --> N
   N --> O[Write machine-state summary]
```

## Dependency relationships

```mermaid
graph LR
   Admin[Admin XML contracts] --> Engine[BuildEngine]
   Engine --> Scheduler[ProcessScheduler]
   Scheduler --> Jobs[Technical jobs]
   Jobs --> Install[Installed packages]
   Install --> Metadata[SBOM and license metadata]
   Metadata --> Server[BuildEngine Server]
   Install --> Server
   Admin --> Manual[Project Markdown]
   Manual --> Server
```

## Core orchestration

The central execution path first prepares Git, synchronizes administration data, prepares the remaining required tools, processes the configured build files, and optionally runs smoke tests.

```cpp
int BuildEngine::Run() {
   PrintConfiguration();
   theEvents.AttachMachineState(theMachineState, theConfiguration.WorkerCount());

   theToolRegistry.Load(theConfiguration.ToolsFile());
   BuildVariables const theVariables { theConfiguration, theEnvironment, theToolRegistry };

   // Bootstrap, administration synchronization, managed tools and library DAGs
   // are evaluated before the final machine-state summary is emitted.

   return 0;
}
```

The production implementation contains the complete error handling, tool preparation, scheduler execution, heartbeat handling, metadata, documentation, and smoke-test logic. The fragment above is deliberately shortened for documentation.

## Heartbeat semantics

The heartbeat deliberately separates **worker utilization** from **logical job state**. A logical job can remain in the scheduler's `running` state while its technical steps move through transitions; that job-state count is therefore not a worker count and may legitimately be larger than the configured worker pool.

The first heartbeat field instead counts active technical `:action:` activities, which correspond to worker-executed steps. It is therefore bounded by the configured scheduler worker count. `open` is the separate count of all non-terminal logical jobs (`pending + ready + job-running`).

For example:

```text
[HEARTBEAT] running=4/4, open=32, ready=0, pending=12, current=406, passed=22, failed=0, blocked=0
```

means that all four workers are currently occupied while 32 logical jobs have not yet reached a terminal state. `open` and `running` intentionally answer different questions and must not be derived from the same counter.

The following activity lines identify the actual worker operations and retain elapsed time, latest activity age, and process-reported progress where available.

## Modern C++23 direction

BuildEngine prefers modern C++23 facilities over older idioms whenever the compiler and target library support them. Examples include ranges, concepts, `std::span`, `std::string_view`, designated initializers, structured bindings, `std::optional`, `std::format`, and value-oriented APIs.

A small C++23 example:

```cpp
#include <concepts>
#include <functional>
#include <ranges>
#include <span>

template<typename value_ty>
concept integral_value = std::integral<value_ty> && !std::same_as<value_ty, bool>;

template<integral_value value_ty>
[[nodiscard]] value_ty Sum(std::span<value_ty const> const spValues) {
   return std::ranges::fold_left(spValues, value_ty {}, std::plus {});
}
```

## Incremental state

BuildEngine distinguishes technical state from generated evidence. A library is not considered current merely because a file happens to exist. Technical steps are evaluated against their recorded state and fingerprints; output validation is evidence about the result, not an alternative state machine.

> **State rule:** technical step state is authoritative. Evidence validation can reject an otherwise current result, but it does not create a second independent state authority.

This distinction is especially important for large builds because a changed library timestamp, source input, build contract, tool version, or other fingerprint input should invalidate only the work that actually has to run again.

The documentation pipeline follows the same rule. Enabling LaTeX changes the Doxygen output contract and therefore the Doxygen state. Changing only MiKTeX changes the PDF state but does not rerun Doxygen.

## Build variants

Release and Debug are variants of the same library contract. Common arguments belong to the shared build node, while variant-specific arguments refine optimization level, binary names, installation directories, and other configuration-dependent values.

A typical conceptual shape is:

```xml
<build>
   <argument value="-G"/>
   <argument value="Ninja"/>
   <variant name="Release">
      <argument value="-DCMAKE_BUILD_TYPE:STRING=Release"/>
   </variant>
   <variant name="Debug">
      <argument value="-DCMAKE_BUILD_TYPE:STRING=Debug"/>
   </variant>
</build>
```

## Documentation pipeline

Central API documentation has one Doxygen analysis/execution per library state:

```mermaid
flowchart LR
   G[Generate documentation inputs + Doxyfile] --> D[Doxygen - one run]
   D --> H[HTML]
   D -->|effective latex=true| L[LaTeX]
   R[One MiKTeX runtime preflight] --> M[MiKTeX texify]
   L --> M
   M --> P[PDF]
```

The key distinction is between **Doxygen output selection** and **PDF compilation**:

- HTML is always requested for a Doxygen-enabled profile.
- LaTeX is an additional output of the same Doxygen invocation when the effective `latex` setting is true.
- MiKTeX does not rerun Doxygen; it consumes `latex/refman.tex` and related generated files.
- Before per-library PDF jobs can run, one shared MiKTeX preflight initializes common runtime state such as the `pdflatex` format; independent PDF jobs remain parallel after that prerequisite.
- The PDF state contains the MiKTeX version, while the Doxygen state does not.

This avoids duplicate parsing of large source trees such as Boost or ACE/TAO, prevents first-use MiKTeX runtime races across parallel PDF jobs, and keeps invalidation aligned with the actual technical dependency.

The complete contract is documented in [documentation.md](documentation.md).

## Important components

| Component | Responsibility |
| --- | --- |
| `BuildEngine` | Top-level orchestration and build-file processing |
| `BuildEngineConfiguration` | Effective local configuration and paths |
| `RepositorySync` | Synchronization of administration repositories |
| `ToolConfiguration` / `ToolJobs` | Managed tool contracts and preparation |
| `LibraryConfiguration` | Declarative library contracts |
| `LibraryJobBuilder` | Translation of library contracts into executable jobs |
| `ProcessScheduler` | Bounded concurrent DAG execution |
| `LibraryStepState` | Authoritative technical incremental state |
| `MetadataJob` | License and CycloneDX metadata |
| `CentralDocumentationJob` / `DocumentationPipelineJob` | Shared documentation generation plus single-pass HTML/LaTeX Doxygen pipeline and downstream PDF job |
| `SmokeJobBuilder` | Consumer/integration smoke tests |
| `BuildEngine-Common` | Shared repository, HTTP, security, package, Markdown, and utility services |

## Shared architecture

BuildEngine is no longer only one console executable. The project now has several presentation surfaces: the command-line application, the VCL-based manager, and the local HTTP/REST server. They must not develop separate interpretations of libraries, packages, security findings, or risk state.

Shared domain and infrastructure functionality therefore belongs in `BuildEngine-Common`. The same classes can be consumed by the console application, the graphical VCL application, and the server. Risk assessment is an important example: the assessment model should have one implementation even though its results can be presented in a console table, a native Windows UI, JSON, or an HTML page.

```mermaid
graph TD
   Common[BuildEngine-Common DLL]
   Console[BuildEngine Console] --> Common
   Manager[VCL Manager] --> Common
   Server[HTTP / REST Server] --> Common
   Common --> Repository[Repository and package model]
   Common --> Risk[Risk assessment]
   Common --> Markdown[Markdown rendering]
   Common --> Export[Package export]
```

This separation keeps presentation technology replaceable while the technical interpretation remains consistent.

## Self-documenting project contract

The Markdown files under `admin/server-docs` are part of the engineering contract, not an after-the-fact manual. They are synchronized with the Admin repository and rendered directly by the server.

The maintenance rule is therefore:

- changes to local configuration semantics update [configuration.md](configuration.md),
- changes to `build-tools.xml` update [build-tools.md](build-tools.md) and, when tools or roles change, [tools.md](tools.md),
- changes to `build-libraries.xml` update [build-libraries.md](build-libraries.md),
- changes to documentation generation update [documentation.md](documentation.md),
- changes to HTTP/Markdown behavior update [server.md](server.md),
- architectural consequences are reflected in this document.

Relative links between these Markdown files are used so the documentation remains navigable inside the server's `/manual/` namespace. Live project documents use `[TOC|Content]` after the document title so the renderer can generate consistent in-document navigation.

## Failure model

A useful conceptual distinction is:

- a **technical action failure** means the action itself did not complete successfully;
- a **dependency failure** means the job could not run because a prerequisite failed;
- a **validation failure** means recorded/current technical state exists but required evidence is missing or invalid;
- a **not-current state** means the scheduler must execute the affected technical step again.

Nested example:

- Library
  - Source
    - Download
    - Extract
  - Build
    - Release
    - Debug
  - Install
  - Metadata
  - Documentation
    - Doxygen HTML + optional LaTeX
    - shared MiKTeX runtime preflight when PDF is required
    - optional MiKTeX PDF

## Design principle

The system is intentionally contract-driven. Compiler-specific integration remains explicit and verifiable, while reusable orchestration logic should stay generic. In the current third-party project, alternative compiler paths are not silently substituted for BCC64X: compatibility problems must remain visible so the integration result is meaningful.