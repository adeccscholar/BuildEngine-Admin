# Administration Data

This directory contains the synchronized **administration contracts** used by BuildEngine as part of the C++Builder Third-Party Integration project.

It is not an application and contains no executable BuildEngine logic. Its purpose is to describe required tools, third-party library builds, smoke-test mappings, schemas, and generic shared build configuration.

The parent repository README defines the complete synchronization, ownership, version-range, and evidence contract.

Machine-local state such as `tools.xml` and `machine-state.xml` is not authoritative Git content and must not be overwritten by repository synchronization once created locally.

## Managed portable tool archives

Managed tools may declare an external extractor in `build-tools.xml`. The extractor runs only after the archive SHA256 was verified and writes exclusively below the configured BuildEngine tools root.

The LLVM 20.1.7 coverage tool is sourced from the official portable Windows `tar.xz` archive. `programs/extract-tar-xz.py` extracts only `llvm-cov.exe`, adjacent DLL dependencies, and the license into the managed tool directory. No installer is executed. The operation does not modify the registry, the system or user `PATH`, file associations, installed-program records, or files outside the configured target.
