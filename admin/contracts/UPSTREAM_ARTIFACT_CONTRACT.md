# BuildEngine-Admin — Upstream Artifact Contract

**Contract ID:** `BE-ADMIN-UPSTREAM-ARTIFACTS-001`  
**Status:** Binding  
**Version:** 1  
**Date:** 27 August 2026  
**Scope:** Third-party libraries in `admin/build-libraries.xml`, Windows x64, C++Builder 13 / BCC64X

## 1. Purpose

BuildEngine must demonstrate how far modern third-party projects can be built and consumed with BCC64X using their original ecosystem. The objective is therefore not to impose an internal file-naming convention, but to retain upstream sources, upstream build systems and their artifact contracts as far as possible.

The normal route is:

```text
Original source -> original build system -> BCC64X build -> Install -> Require -> Evidence
```

## 2. Authoritative artifact names

1. The names produced by the original upstream build system with the active BCC64X/CMake toolchain are authoritative.
2. BuildEngine describes and verifies those names; it does not invent an additional naming convention for third-party products.
3. Prefixes such as `lib`, upstream suffixes, version tokens and distinctions between shared, import and static-library names are retained whenever produced by upstream and the active toolchain.
4. Release and Debug are primarily separated by installation directories:

```text
bin/win64/Release
bin/win64/Debug
lib/win64/Release
lib/win64/Debug
```

BuildEngine does not invent an additional `d`, `debug` or other name suffix. A suffix is used only when upstream defines one or a documented technical necessity requires it.

## 3. Producer before consumer

A downstream consumer must not change the producer's artifact contract without a demonstrated technical requirement.

For example, if BCC64X produces Brotli as `libbrotlicommon.lib`, the file must not be renamed `brotlicommon.lib` merely because another upstream consumer expects that name on a different Windows toolchain. Consumer integration must accommodate the real producer output and must be independently verified.

## 4. Build first, then define deviations

For a new library, follow this sequence:

1. Acquire official, pinned upstream sources.
2. Use the unmodified upstream build system.
3. Configure and compile with BCC64X.
4. Examine actual artifacts and test results.
5. Define install and `require` contracts matching the real upstream/toolchain outputs.
6. Consider a patch, CMake adjustment or other deviation only when a reproducible failure warrants one.

Problems known from older C++Builder toolchains are not automatically findings for BCC64X. They must be reproduced with the current toolchain.

## 5. Permitted exceptions

Deviation from the upstream artifact contract is permitted only when at least one of these conditions has been demonstrated:

- A necessary BCC64X ABI/COFF64 adaptation
- A reproduced upstream build-system defect or unsupported case
- A mandatory technical conflict between a shared import library and an independent static library
- A verified platform requirement that upstream does not correctly model for BCC64X

Every exception must be visible in the library contract, minimal in scope, technically justified, supported by build/test evidence, and designed to allow a future return to upstream behavior. Internal naming preferences alone are not a valid justification.

## 6. Source archives and transport formats

The transport format of an official upstream source artifact is not part of the binary artifact-naming contract. Switching between official archive types for the same tag/commit is permitted if the source identity remains unambiguous, the selected archive has a pinned SHA-256 checksum, the source contents remain unchanged, and the change serves reproducible provisioning only.

For example, an official GitHub tag ZIP may replace a TAR archive if the current extractor cannot yet process certain TAR entry types. Such a replacement is not a source patch.

## 7. Historical applications of the contract

### Brotli 1.2.0

At the time of this contract, the unmodified upstream CMake build had completed successfully with BCC64X in Release and Debug. Shared outputs followed the `lib` prefix under the active BCC64X/CMake configuration, for example `libbrotlicommon.dll`. The install/require contract therefore retained these real names instead of renaming products for OpenSSL or other consumers.

### Zstd 1.5.7

The first run did not reach compilation because TAR extraction stopped on a non-regular archive entry. The contract switched to the official GitHub tag ZIP for the same v1.5.7 revision with a pinned SHA-256 checksum. Zstd sources remained unmodified, including `lib/common/mem.h`. The next required evidence was an actual BCC64X/CMake build.

### zlib 1.3.2

The existing CMake special handling for the independent static-library name predates this contract. It was deliberately not changed during the Brotli/Zstd verification work, to avoid invalidating the previously verified zlib/libarchive chain. This exception still required separate comparison against the official upstream `zs` name and Debug postfix, followed either by removal or a documented technical justification.

## 8. Acceptance criterion

An integration satisfies this contract when its documented installation records preserve the artifacts actually produced by the upstream build system under BCC64X, or when every deviation is demonstrably justified under Section 5.
