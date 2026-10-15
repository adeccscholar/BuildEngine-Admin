# curl 8.23.0 / OpenSSL 3.5.9: isolated uptake and migration plan

Date: 2026-10-15
Status: **source contracts committed, BCC64X builds NOT yet tested**.

## Phase 1: isolated versions

- Existing `curl/8.21.0` and `openssl/3.5.8` are unchanged.
- New `openssl/3.5.9`: original OpenSSL 3.5.8 BCC64X Configure/build/Debug/Release/install/publish/smoke contract copied as independent version; the five source patches are versioned separately under `admin/patches/openssl/3.5.9`. Copying patches **does not prove applicability**; the existing `git apply --check` guards should reject incompatible upstream changes.
- New `curl/8.23.0`: original curl CMake/Debug/Release/install/publish/smoke contract copied as independent version; curl still depends **only on OpenSSL 3.5.8** and zlib 1.3.2.
- SHA256 pinned from GitHub release asset digests:
  - `curl-8.23.0.tar.xz`: `8dadd7d2536a314f982ad05a651b713f426bf7d02cc2c76cffb9796f2218e6c5`
  - `openssl-3.5.9.tar.gz`: `603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a`
- Both retain their permissive upstream licensing (curl license, OpenSSL Apache-2.0), and their present BuildEngine metadata.
- Note that a new version is *not* an update of existing installed files; package and build paths are version-separated.

## Build gates for phase 1

1. Parse/schema-validate `admin/build-libraries.xml` and compile the FSM with both new versions.
2. `openssl/3.5.9`: source download hash; patch-check all 5; build Release+Debug; run original consumer smoke; inspect import libraries, provider/loadable modules and DLL names; collect LICENSE and SBOM. Verify all patches against 3.5.9 rather than treating their provenance as proof.
3. `curl/8.23.0`: download hash; configure with existing `openssl/3.5.8`; build Release+Debug; smoke against the installed OpenSSL runtime; check symbol imports / find_package CURL / TLS handshake.
4. Verify StackBuilder, Bootstrap, CMake export target paths and versioned runtime staging.
5. No dependency migration until independent builds and tests pass.

## Phase 2: separately revise dependency closure

First test `curl/8.23.0` against `openssl/3.5.9` in Debug and Release using explicit versioned paths; then migrate consumers one by one. Candidate consumers explicitly mentioning OpenSSL in the XML (2026-10-15 inventory):

| Library | Currently targeted TLS package | Migration |
|---|---|---|
| curl (new and old) | OpenSSL 3.5.8 | migrate new curl first; keep old stable |
| libarchive 3.8.9 | OpenSSL | test/adjust |
| boost 1.92.0 | OpenSSL | test/adjust |
| ACE 8.0.6 | OpenSSL | test/adjust |
| TAO 4.0.6 | OpenSSL | test/adjust |
| libpq 18.6 | OpenSSL | test/adjust |
| libpqxx 8.0.2 | OpenSSL | test/adjust (including transitive libpq) |
| PoDoFo 1.1.1 | OpenSSL | test/adjust |

Also inspect non-XML consumers and bootstrap templates in BuildEngine, BuildEngine-Common, BuildEngine-Server, BuildEngine-Manager and DeckKernel for hard-coded versions and runtime DLL staging. The XML list is **not** a proven complete runtime dependency graph.

Before switching, create a closure report for the executable/DLL boundaries: no accidental mix of OpenSSL 3.5.8 and 3.5.9 in one process, especially when their DLL filenames match; make installed CMake targets / runtime package references consistent. Update versioned dependency pins and all absolute package paths together per library. Retest real TLS and certificate chain verification, optional OpenSSL providers, QUIC (if enabled), and the normal smoke protocol. Preserve any old version required by still-unmigrated consumers.

## Source provenance
- https://github.com/curl/curl/releases/tag/curl-8_23_0
- https://github.com/openssl/openssl/releases/tag/openssl-3.5.9
