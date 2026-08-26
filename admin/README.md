# Administration Data

This directory contains the synchronized **administration contracts** used by BuildEngine as part of the C++Builder Third-Party Integration project.

It is not an application and contains no executable BuildEngine logic. Its purpose is to describe required tools, third-party library builds, smoke-test mappings, schemas, and generic shared build configuration.

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

## Incremental library timestamp and machine state

Library schema 6 requires one ISO 8601 `library/@timestamp`. It describes when that complete library contract was provided or changed and must be advanced for every change to its source, build, test, installation, patch, or artifact requirements.

After each successful phase, BuildEngine writes a machine-local state beside the corresponding download, variant-build, or versioned package area. The state contains the library contract timestamp, phase identity, configuration and relevant dependency timestamps plus `completedAt`, the UTC time at which that phase completed on the machine.

Missing directories, missing states, or a contract mismatch schedule only the affected phase again. A changed dependency timestamp also invalidates dependent build and package states. `BuildEngine --rebuild` bypasses all current states.
