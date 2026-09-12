# BuildEngine

BuildEngine is a declarative build orchestration system for reproducible C and C++ third-party library builds. Its current Windows integration is centered on Embarcadero C++Builder and the modern BCC64X toolchain. Library knowledge belongs in synchronized XML contracts, while the executable evaluates those contracts, prepares tools, creates a technical dependency graph, executes technical steps, and records the resulting state.

## Main responsibilities

BuildEngine separates configuration, orchestration, execution, evidence, and presentation:

- **Configuration** resolves the production root, tools, repositories, build contracts, concurrency, tests, and documentation settings.
- **Repository synchronization** updates the administration content before build evaluation.
- **Tool preparation** discovers or installs tools described by the administration contract.
- **Library contracts** define source acquisition, build variants, installation, publication, smoke tests, metadata, and documentation.
- **The process scheduler** executes the technical dependency graph with a bounded worker count.
- **Technical step state** is the authoritative incremental state for library work.
- **Metadata generation** creates license information and CycloneDX SBOM data.
- **Documentation generation** publishes library information and, where enabled, Doxygen API documentation.
- **The server** presents the resulting packages, documentation, SBOMs, usage information, and security evidence without becoming a second build-state authority.

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
   H --> I[Execute required technical actions]
   I --> J[Install and publish artifacts]
   J --> K[Generate metadata and documentation]
   K --> L[Run configured smoke tests]
   L --> M[Write machine-state summary]
```

The diagram is intentionally part of this document so the server can verify Mermaid selection independently from plain Markdown rendering.

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

## Incremental state

BuildEngine distinguishes technical state from generated evidence. A library is not considered current merely because a file happens to exist. Technical steps are evaluated against their recorded state and fingerprints; output validation is evidence about the result, not an alternative state machine.

This distinction is especially important for large builds because a changed library timestamp, source input, build contract, tool version, or other fingerprint input can invalidate only the work that must actually run again.

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
| `CentralDocumentationJob` | Library documentation and Doxygen integration |
| `SmokeJobBuilder` | Consumer/integration smoke tests |
| `BuildEngine-Common` | Shared repository, HTTP, security, package, and utility services |

## Design principle

The system is intentionally contract-driven. Compiler-specific integration remains explicit and testable, while reusable orchestration logic should stay generic. In the current third-party project, alternative compiler paths are not silently substituted for BCC64X: compatibility problems must remain visible so the integration result is meaningful.
