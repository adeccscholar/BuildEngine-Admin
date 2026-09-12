# BuildEngine Configuration and Command Line

BuildEngine combines a local XML parameter file with synchronized administration contracts. The local file selects the production tree, concurrency, repositories, feature switches, and contract locations; the Admin repository supplies the detailed tool, library, documentation, smoke-test, schema, and security definitions.

For the complete documentation profile, Doxygen, MathJax, LaTeX and MiKTeX contract, see [BuildEngine Documentation Contract](/manual/documentation.md).

## BuildEngine.xml

The executable uses `BuildEngine.xml` next to the executable unless another configuration file is supplied as the final command-line argument.

A representative structure is:

```xml
<buildEngine schemaVersion="5">
   <parameters
      companyName="adecc Systemhaus GmbH"
      rsvars="C:\Program Files (x86)\Embarcadero\Studio\37.0\bin\rsvars.bat"
      root="D:\local\embarcadero\test_v3"
      heartbeatSeconds="20"
      workers="4"
      queueLength="8"
      testJobs="4"
      serverLanguage="en"
      managerLanguage="en"
      WithTests="true"
      WithSmokeTests="true"
      WithDoc="true"
      WithDoxygen="true"
      WithLatex="true"
      configurations="All"
      bootstrapTools="build-tools.xml"
      buildTools="admin\build-tools.xml"
      toolsState="admin\tools.xml"
      machineState="admin\machine-state.xml"
      toolsRoot="tools"
      downloadsRoot="tools\downloads"
      sourceRoot="src"
      buildRoot="build"
      installRoot="install"
      workspaceRoot="build\thirdparty-test"
      repositoriesRoot="repositories"
      smokeTests="admin\smoke-tests.xml"
      smokeRoot="smoketests"
      demoRoot="demos"
      logsRoot="logs\buildengine"/>

   <repository id="admin"
               url="https://github.com/adeccscholar/BuildEngine-Admin.git"
               branch="main"
               checkout="BuildEngine-Admin">
      <sync source="admin" target="admin">
         <preserve path="libraries.xml"/>
         <preserve path="tools.xml"/>
         <preserve path="machine-state.xml"/>
      </sync>
   </repository>

   <build file="admin\build-libraries.xml"/>
</buildEngine>
```

The exact local worker counts and feature switches are deployment choices; the example above explains the structure rather than prescribing one machine configuration.

## Important parameter groups

| Parameter | Purpose |
| --- | --- |
| `companyName` | Branding used by generated documentation |
| `rsvars` | C++Builder environment initialization script |
| `root` | BuildEngine production root |
| `heartbeatSeconds` | Heartbeat interval for long-running work |
| `workers` | Maximum concurrent scheduler workers |
| `queueLength` | Scheduler queue capacity |
| `testJobs` | Parallelism made available to supported test runners |
| `WithTests` | Enables upstream/library tests |
| `WithSmokeTests` | Enables BuildEngine consumer smoke tests |
| `WithDoc` | Enables generated library information documentation |
| `WithDoxygen` | Permits Doxygen API documentation |
| `WithLatex` | Local default for the independent Doxygen LaTeX/MiKTeX PDF branch; default is `true` |
| `configurations` | Default build variants, for example `All` |
| `buildTools` | Synchronized managed-tool contract |
| `toolsState` | Generated effective tool state |
| `machineState` | Generated machine/job state |
| `sourceRoot` | Downloaded/extracted source area |
| `buildRoot` | Build and generated-work area |
| `installRoot` | Installed package root |
| `repositoriesRoot` | Local administration repository checkouts |
| `logsRoot` | BuildEngine log hierarchy |

`WithDoxygen=false` is a hard stop for the central Doxygen pipeline. `WithLatex`, however, is an inheritable local default. `admin/build-documentation.xml` may override it for the project, and a library may independently override it again with `latex="true"` or `latex="false"`. MiKTeX is requested only if at least one Doxygen-enabled library resolves to effective `latex=true`.

## Concurrency model

The scheduler worker count defines an upper bound for simultaneously executing worker actions. In compact form:

$$
N_{active} \leq N_{workers}
$$

For example, with `workers="4"`, the expected invariant is:

\[
0 \leq N_{active} \leq 4
\]

The queue length is a separate capacity. A simple conceptual upper bound for queued plus active work is:

$$
N_{resident} \leq N_{workers} + N_{queue}
$$

For a four-worker scheduler, $N_{workers}=4$.

## Administration XML files

### `admin/build-tools.xml`

Defines reproducible tools and browser assets. A tool may be discovered from an existing installation or managed by BuildEngine through download, extraction/generation, launcher, and probe steps. Examples include Git, CMake, Ninja, Doxygen, Graphviz, MiKTeX, compiler tools, and the managed JavaScript resources used by the documentation server.

MiKTeX is a `when-used` tool. It is provisioned only when the effective documentation configuration enables LaTeX/PDF for at least one library.

### `admin/build-libraries.xml`

The primary library build contract. Each library entry can define metadata, source acquisition, extraction requirements, patches, build arguments, variants, install operations, publication, smoke consumers, security identity, and documentation metadata. Build knowledge belongs here rather than in library-specific C++ branches inside the engine.

A reduced example illustrates the shared-contract/variant model:

```xml
<library id="example" version="1.2.3" category="test">
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
</library>
```

### `admin/build-documentation.xml`

Defines the shared documentation profile and library-specific overrides. It controls whether Doxygen and LaTeX/PDF are used, source visibility, public-only extraction, predefined macros, Doxygen options, and exclusion patterns.

The root profile is inherited by every library. For `latex`, omission at the root means inheritance from local `WithLatex`; a root `latex` value overrides that local default for the synchronized project, and a library node may override it again. A library node is an override, not an allow-list. The complete parameter reference and examples are documented in [BuildEngine Documentation Contract](/manual/documentation.md).

### `admin/smoke-tests.xml`

Defines smoke/integration tests that are not expressed directly as a library-local consumer smoke entry.

### `admin/tools.xml`

Generated tool state. It records the effective resolved tool roots and versions used by the running production tree. The server uses this information to map stable `/js/...` URLs to managed browser assets.

### `admin/machine-state.xml`

Generated machine/job state. It is not a replacement for the per-library technical step-state files; it provides the broader execution-state view used by BuildEngine.

### `admin/schemas/*.xsd`

XML schemas for the synchronized contracts. They make the accepted declarative vocabulary explicit and allow contract validation independently from C++ implementation details.

## Local versus synchronized configuration

The separation is intentional:

| Concern | Local configuration | Synchronized Admin contract |
| --- | --- | --- |
| Production root | Yes | No |
| Worker count | Yes | No |
| Repository locations | Yes | Repository content itself is synchronized |
| Tool definitions | No | Yes |
| Library versions and build contracts | No | Yes |
| Documentation capability/default settings | Yes | Project and library overrides |
| Documentation profiles | No | Yes |
| Smoke-test definitions | No | Yes |
| Security metadata | No | Yes |

This prevents local machine settings from becoming a hidden second source of library/build knowledge while still allowing the local machine to supply deployment defaults.

## Command line

General syntax:

```text
BuildEngine [command] [options] [configuration-file]
BuildEngine --help
BuildEngine --version
```

The configuration file, when supplied, must be the final command-line argument and must not start with `--`.

## Commands

### `--make`

Default command. Continues an incremental build from the first technical step that is not current for the configured library timestamp and fingerprint state.

```text
BuildEngine --make
```

### `--build`

Performs a clean build for the selected scope and ignores incremental technical state.

```text
BuildEngine --build --config=Release,Debug
```

### `--check`

Synchronizes administration repositories, inspects library/version state, and reports which phases would execute on the next `--make`. It does not perform library source/build/install work.

```text
BuildEngine --check --lib=ace-tao --libversion=8.0.6
```

### `--show`

Displays the effective settings read from the BuildEngine configuration file.

```text
BuildEngine --show D:\config\BuildEngine.xml
```

### `--monitor`

Synchronizes administration repositories and checks configured libraries for known security vulnerabilities. The security model is intended to evolve from pure finding collection toward structured product relevance, remediation candidates, and shared risk assessment.

```text
BuildEngine --monitor
```

### `--parse`

The command is recognized by the current CLI, but the current implementation reports that it is not implemented and performs no build action. It is reserved for checking configured upstream sources for newer versions.

## Options

### `--lib=<library>`

Selects one configured library. Library identifiers are case-sensitive.

### `--libversion=<version>`

Selects an exact version and requires `--lib`. Versions are case-sensitive.

### `--config=<variant>[,<variant>...]`

Selects configuration variants exactly as named in the library XML. `All` selects every declared variant.

### `--tests=on|off`

Overrides the configured test setting for the current invocation.

### `--help` and `--version`

Display CLI help or version information and exit.

## Current selection limitation

The CLI already parses `--lib` and `--libversion` for commands such as `--check` and `--monitor`. In the current build path, library selection is not yet wired into the `--make`/`--build` graph; attempting to use it there is rejected instead of silently building an unintended scope.

## Example command matrix

| Goal | Command |
| --- | --- |
| Incremental default build | `BuildEngine --make` |
| Clean Release + Debug build | `BuildEngine --build --config=Release,Debug` |
| Inspect ACE/TAO state | `BuildEngine --check --lib=ace-tao --libversion=8.0.6` |
| Show effective configuration | `BuildEngine --show D:\config\BuildEngine.xml` |
| Security monitor | `BuildEngine --monitor` |
| Display help | `BuildEngine --help` |

## Configuration validation checklist

- [ ] Production root points to the intended tree.
- [ ] Admin synchronization is configured.
- [ ] `build-tools.xml` and `build-libraries.xml` are available after synchronization.
- [ ] Worker and queue settings match the target machine.
- [ ] Required test/documentation switches are explicit.
- [ ] `WithLatex` has the intended local default and any project/library overrides are deliberate.
- [ ] `tools.xml` and technical state are treated as generated state rather than hand-authored library knowledge.

## Related documentation

The full documentation contract is available at `/manual/documentation.md`. Other project documents are available through the documentation buttons at the top of every server page. Generated library documentation starts at `/index.html`.
