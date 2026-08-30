# Administration Data

This directory contains the synchronized **administration contracts** used by BuildEngine as part of the C++Builder Third-Party Integration project.

It is not an application and contains no executable BuildEngine logic. Its purpose is to describe required tools, third-party library builds, publish mappings, library consumer smoke tests, schemas, and generic shared build configuration.

The parent repository README defines the complete synchronization, ownership, version-range, and evidence contract.

Machine-local state such as `tools.xml` and `machine-state.xml` is not authoritative Git content and must not be overwritten by repository synchronization once created locally.

## Managed portable tool archives

Managed tools may declare an external extractor in `build-tools.xml`. The extractor runs only after the archive SHA256 was verified and writes exclusively below the configured BuildEngine tools root.

The LLVM 20.1.7 coverage tool is sourced from the official portable Windows `tar.xz` archive. `programs/extract-tar-xz.py` extracts only `llvm-cov.exe`, adjacent DLL dependencies, and the license into the managed tool directory. No installer is executed. The operation does not modify the registry, the system or user `PATH`, file associations, installed-program records, or files outside the configured target.

This Python extractor is the bootstrap bridge until the native libarchive package can be consumed by BuildEngine itself. The managed-tool XML contract is intentionally backend-neutral and remains unchanged when extraction moves into BuildEngine. The current libarchive artifact contract produces `libarchive.dll` with the `libarchive.lib` import library and the unambiguously named `libarchive_static.lib`; it does not produce a separate `tar.lib`.

## Source backpatches

Repository-maintained compatibility patches reside below `patches/<library>/<version>`. A patch is never a source substitute: the XML contract first downloads and completely extracts the verified upstream source, then checks the patch against that exact tree before applying it.

For libarchive 3.8.9, `bcc64x-modern-borland-compat.patch` limits legacy `__BORLANDC__` compiler workarounds to non-Clang compilers. BCC64X also defines `__clang__` and therefore uses the modern Clang-compatible paths for inline handling, integer types and literals, Windows `lseek`/`mbstate_t` handling, `open` mode arguments, and the corresponding tests. The patch additionally backports the upstream Windows/Clang fix from commit `f75ed3a20526ef4ec2f46ee94216b6efd01eab0a` for `filter_fork_windows.c` and avoids the BCC64X MinGW `ftruncate` macro redefinition in `test_read_data_large.c` without disabling Debug `-Werror`.

The repository copy remains the authoritative input. Before validation and application, the source contract copies it to `{Workspace}\\.buildengine\\patches\\<version>`, outside the synchronized `admin` tree. The generic copy action uses `preserveCurrentArtifact="true"`, so `{CurrentArtifact}` continues to identify the completely extracted upstream source.

## Publish and consumer smoke pipeline

Schema 11 keeps publish and simple consumer smoke tests in the same per-library process contract and extends that contract to multiple consumer gates per library. Release and Debug now progress independently through build/test/validation/install. The selected publish configuration (currently Release) continues through `publish -> smoke -> ready:Release`; Debug reaches `ready:Debug` after its own verified install and the shared `install:common` SDK state. `install:common` depends only on the selected publish configuration, so Release never waits for Debug. The aggregate `library:<id>:ready` remains the final library status but no longer serializes the two variants before Release downstream work can start.

The versioned package tree below `install/packages/<id>/<version>` remains the authoritative producer result. The `<publish>` node maps the selected Release artifacts into the shared `install/Win64x` consumer tree. Headers, import libraries, runtime binaries, pkg-config data and CMake package entry points are published without requiring downstream consumers to know the package version directory.

Installed CMake package configurations remain in their versioned package directory. Publish creates relative forwarding config files below `Win64x/lib/cmake`, so their original relative target references continue to resolve correctly. `cmake/consumer/BuildEngineConsumer.cmake` adds the shared prefix, include, library, program and module search paths for normal CMake consumers.

Downstream build variants depend on the matching `ready:<Configuration>` state. Existing successful schema-10 aggregate install/ready markers are migrated to the new intermediate state files so a scheduler-only DAG change does not unnecessarily rebuild already verified packages.

Producer CMake builds receive only their declared versioned dependencies. BuildEngine supplies each package root through `CMAKE_PREFIX_PATH`, each `include` directory through `CMAKE_INCLUDE_PATH`, and each `lib/win64/<Configuration>` directory through `CMAKE_LIBRARY_PATH`. This keeps upstream `find_package()`/`find_library()` calls compatible with the versioned package layout without exposing the published consumer-only `Win64x/cmake` module overlay.

Simple `<smoke>` nodes are usability tests, not additional producer regression suites. Each smoke configures a fresh C++23 consumer with normal `find_package()` calls and compiles/links it against the selected consumer scope. Optional `<run executable="..."/>` children execute one or more real consumer programs first; every run must return zero before the final validator executable is started. The validator then emits the strict `SMOKE|...` protocol. Published scopes execute with `Win64x/bin` on the child-process `PATH`; package scopes use the exact versioned runtime path of their configuration. BuildEngine preserves the complete raw stdout/stderr process log and also writes a full validation log containing every observed line plus the parsed checks and final validation status.

Complex multi-package integration tests and demos do not belong to these per-library smoke nodes. They are maintained in the existing `BuildEngine-Tests` repository. That repository therefore contains only the larger integration and demo scenarios; the simple package usability smokes live exclusively in this administration repository.

## Incremental library timestamp and machine state

Library schema 6 and later require one ISO 8601 `library/@timestamp`. It describes when that complete library contract was provided or changed and must be advanced for every change to its source, build, test, installation, publish, smoke, patch, or artifact requirements.

After each successful phase, BuildEngine writes machine-local state beside the corresponding download, variant-build, package, publish, smoke, or ready area. Missing directories, missing states, contract mismatches, or missing published files schedule only the affected phase again. A changed dependency timestamp also invalidates dependent build and package states. `BuildEngine --rebuild` bypasses all current states.

## Boost 1.92.0 component evidence contract

Schema 11 allows more than one package consumer smoke per library. Boost uses this intentionally to reproduce the seven target-machine evidence areas in both configurations while still producing one coherent upstream CMake graph per configuration.

The production build selects the frozen 148-module set and does not merge historical stage outputs. Release and Debug install into the one versioned package. Boost sets `publish/@requiresAllVariants="true"` because both configuration installs touch the common Boost header tree; publication therefore starts only after the complete package is stable.

Release gates use the normal published `Win64x` consumer view. Debug gates use `scope="package"`, resolve the actual `BoostConfig.cmake` below `lib/win64/Debug/cmake`, and add the exact package/dependency runtime, include, library and prefix paths to the child CMake process. No global Debug SDK tree is created.

The seven gate IDs are `closure`, `math-numerics`, `state-parsing-meta`, `algorithms-containers-data`, `concurrency-async`, `system-io-runtime`, and `foundation-language`. Their source-module mapping and the six external ecosystem exclusions are recorded in `cmake/boost/components.json`.

Boost.Config remains upstream source. The BCC64X policy layer below `cmake/boost/bcc64x-native-clang` only routes Clang-based CodeGear builds to upstream `clang.hpp`. The three compile-only preflights execute before the expensive Boost CMake graph. The generic BCC64X toolchain also exposes BCC64X as the ASM compiler, matching the already verified Context/Coroutine/Fiber toolchain path.
