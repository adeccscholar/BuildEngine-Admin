# BuildEngine Administration Contracts

This repository is one component of the **C++Builder Third-Party Integration project**. The overall project investigates, builds, packages, and documents third-party C and C++ libraries for **Embarcadero C++Builder / BCC64X** in a reproducible way.

This repository is **not a program** and it does not contain the BuildEngine executable. It contains the declarative administration and build contracts consumed by BuildEngine.

**License:** MIT

## Purpose

The repository is the authoritative, version-controlled administration source for the third-party build environment. Its job is to describe **what shall be built and how the generic BuildEngine infrastructure shall invoke the required tools**.

It contains, in particular:

- required build tools and their provisioning/probe contracts;
- third-party library source, build, install, and artifact contracts;
- version-independent smoke-test registrations and their applicable library-version ranges;
- XML schemas for these contracts;
- generic CMake/BCC64X toolchain and rule files that are shared across libraries.

The repository deliberately does **not** contain library-specific C++ job classes or replacement build systems merely to make a library fit the project. Third-party libraries should use their upstream build systems whenever possible.

## Role in the overall project

The project is larger than this repository:

```text
C++Builder Third-Party Integration Project
|
+-- BuildEngine program
|     Executes generic technical actions and schedules work
|
+-- BuildEngineAdmin repository        <-- this repository
|     Declarative tool/library/test contracts
|
+-- BuildEngineSmokeTests repository
|     Version-independent tests and evidence programs
|
+-- Third-party upstream sources
|     Downloaded and built by the workflow
|
+-- Installed packages, logs and evidence
      Results produced by the workflow
```

The administration repository therefore describes and controls part of the process; it is not the process implementation itself.

## Repository layout

```text
BuildEngineAdmin/
|-- README.md
|-- LICENSE
|-- .gitattributes
|-- .gitignore
|-- SHA256SUMS.txt
`-- admin/
    |-- build-tools.xml
    |-- build-libraries.xml
    |-- smoke-tests.xml
    |-- cmake/
    |   |-- overrides/
    |   |   `-- bcc64x-rules.cmake
    |   `-- toolchains/
    |       `-- bcc64x-buildengine-cxx.cmake
    `-- schemas/
        |-- build-tools.xsd
        |-- build-libraries.xsd
        `-- smoke-tests.xsd
```

`tools.xml` and `machine-state.xml` are machine-local runtime state. They are not authoritative repository content and must not be overwritten by an administration sync once they exist locally.

## Synchronization into a BuildEngine workspace

A local Git worktree is synchronized into the BuildEngine working directory before the full build configuration is evaluated:

```text
<AdminGitWorktree>\admin\
          |
          | compare by relative path and content hash
          v
<BuildWorkspace>\admin\
```

For repository-managed files:

- identical path and identical content: no change;
- new file: copy it into the workspace;
- changed file: replace it atomically;
- repository-managed file removed from Git: remove it from the synchronized target;
- machine-local state files: preserve them;
- timestamps are not used as content identity;
- the selected Git commit should be recorded later as build evidence.

This allows new libraries, changed library contracts, new tool requirements, or revised smoke-test mappings to become available without recompiling BuildEngine, as long as they use technical actions already supported by the program.

## Bootstrap and managed operation

The desired startup effort is intentionally small.

If `admin\build-tools.xml` does not yet exist in the workspace, BuildEngine uses only the minimal tool provision available in its working directory to make the administration content available. After synchronization, BuildEngine switches to the synchronized administration tree and performs the full tool phase from `admin\build-tools.xml`.

Conceptually:

```text
BuildEngine starts
   |
   +-- admin/build-tools.xml missing?
   |      |
   |      `-- use minimal startup provision required for repository sync
   |
   +-- synchronize administration content
   |
   +-- synchronize evidence-test content
   |
   +-- reload admin/build-tools.xml
   |
   +-- provision/probe complete tool set
   |
   +-- load admin/build-libraries.xml
   |
   +-- build and install third-party libraries
   |
   `-- load admin/smoke-tests.xml
          |
          `-- schedule applicable evidence tests
```

The synchronized `admin` tree becomes authoritative after the bootstrap transition.

## Library contracts

`build-libraries.xml` describes third-party libraries as data. A library is not represented by a dedicated BuildEngine C++ class.

Typical declarative information includes:

- library identifier and version;
- official upstream source location and archive metadata;
- required source files after extraction;
- CMake arguments and toolchain selection;
- Release/Debug or other variants;
- install layout;
- dependency relationships;
- expected installed artifacts.

A new library should require only XML changes when its complete workflow can be expressed through existing generic technical actions.

## Smoke-test registration and version ranges

Smoke-test **sources are version-independent** and live in the separate evidence repository. This repository only maps those stable tests to libraries and specifies when they apply.

Example:

```xml
<library id="pugixml">
   <smoke id="installed-consumer-charconv"
          source="pugixml\installed-consumer-charconv"
          enabled="true"
          minVersion="1.16">
      <variant name="Release"/>
      <variant name="Debug"/>
   </smoke>
</library>
```

Version-range semantics are inclusive:

- no `minVersion` and no `maxVersion`: valid for every version of that library;
- only `minVersion`: valid from that version onward;
- only `maxVersion`: valid up to and including that version;
- both: valid within the closed range `[minVersion, maxVersion]`.

A test version is never encoded in its source directory merely to match a current library release. If version ordering cannot be evaluated reliably, BuildEngine must fail rather than guess.

## Upstream-first integration policy

The purpose of the project is to determine how well current third-party libraries integrate with C++Builder/BCC64X, not to hide incompatibilities behind unrelated replacement build paths.

The preferred sequence is:

```text
upstream source
   -> upstream build system
   -> BCC64X toolchain
   -> install
   -> artifact checks
   -> independent consumer evidence
```

When real evidence demonstrates a problem, the correction should be classified explicitly as one of:

- a generic BCC64X/toolchain correction;
- a generic BuildEngine action or capability;
- a declarative library option;
- a documented and reproducible source patch when the problem is genuinely library-specific.

## Evidence and reproducibility

The administration data is part of the evidence chain. A reproducible run should be able to identify at least:

- the BuildEngine revision;
- the administration repository commit;
- the evidence-test repository commit;
- the third-party library version/source pin;
- the compiler/tool versions;
- the effective build parameters;
- produced package artifacts;
- raw build/test logs and final status.

## What this repository is not

This repository is not:

- the BuildEngine application;
- a fork or mirror of the third-party libraries;
- the smoke-test or demo source repository;
- an alternate compiler/toolchain distribution;
- a collection of library-specific scripts hidden behind XML.

It is the **declarative administration layer** of the larger C++Builder third-party integration and evidence project.

## License

Project-authored content in this repository is licensed under the MIT License. See `LICENSE`.
