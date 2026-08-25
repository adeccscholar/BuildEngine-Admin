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

For libarchive 3.8.9, `bcc64x-modern-mbstate.patch` limits an upstream legacy `__BORLANDC__` workaround to non-Clang compilers. BCC64X uses the modern Clang-based runtime headers, which already provide `mbstate_t` and `wcrtomb`.

## Incremental phase timestamps

Library schema 6 requires independent ISO 8601 `timestamp` attributes on `source`, `build`, and `install`. A timestamp is part of the declarative contract and must be advanced whenever that phase's inputs or actions change. Changing a source timestamp invalidates downstream build and package tokens; changing a dependency installation timestamp invalidates dependent builds and packages.

Successful source jobs store their token below the downloads root, variant jobs inside their build directory, and installation jobs inside the versioned package directory. Missing directories, missing markers, or differing tokens schedule the affected phase again. `BuildEngine --rebuild` bypasses all current markers.
