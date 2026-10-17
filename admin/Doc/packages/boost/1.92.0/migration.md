# boost 1.92.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `boost@1.92.0` · **Category:** foundation · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

**Package purpose:** Comprehensive collection of portable C++ libraries covering containers, algorithms, networking, concurrency, and more.

Use the official Boost CMake distribution with C++23 and a broad explicitly selected module closure, rather than a separately improvised Boost.Build binary tree. Early compiler-model preflights detect BCC64X/Boost.Config disagreements. Exclude dependency-heavy MPI/Python ecosystems; treat Boost.Random's random_device as interface/header use, as documented in the contract.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `openssl@3.5.9`
- `zlib@1.3.2`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `BOOST_IOSTREAMS_ENABLE_ZLIB` | `ON` |
| `BUILD_SHARED_LIBS` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BOOST_ENABLE_MPI` | `OFF` |
| `BOOST_ENABLE_PYTHON` | `OFF` |
| `BOOST_RANDOM_ENABLE_RANDOM_DEVICE` | `OFF` |
| `BUILD_TESTING` | `OFF` |
| `OPENSSL_USE_STATIC_LIBS` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `BOOST_INSTALL_LAYOUT` | `system` |
| `BOOST_INSTALL_CMAKEDIR` | `lib/win64/{Configuration}/cmake` |
| `BOOST_INCLUDE_LIBRARIES` | `filesystem;system;program_options;regex;thread;chrono;json;asio;beast;accumulators;decimal;endian;geometry;histogram;integer;math;multi_array;multiprecision;numeric/conversion;numeric/interval;numeric/odeint;numeric/ublas;polygon;qvm;random;ratio;rational;safe_numerics;units;utility;bind;callable_traits;core;describe;function_types;fusion;hana;hof;lambda;lambda2;local_function;metaparse;mp11;mpl;msm;parser;pfr;phoenix;preprocessor;proto;spirit;statechart;static_assert;tti;type_traits;typeof;vmd;yap;algorithm;any;array;bimap;bloom;circular_buffer;foreach;gil;graph;heap;icl;intrusive;iterator;multi_index;optional;poly_collection;property_map;property_tree;ptr_container;range;sort;tuple;unordered;uuid;variant;variant2;cobalt;context;coroutine;coroutine2;fiber;interprocess;lockfree;mqtt5;mysql;redis;assign;charconv;contract;convert;dll;exception;format;io;iostreams;leaf;lexical_cast;locale;log;nowide;process;serialization;stacktrace;static_string;test;timer;tokenizer;type_erasure;type_index;url;wave;winapi;xpressive;align;assert;compat;concept_check;config;container_hash;conversion;crc;detail;dynamic_bitset;flyweight;function;functional;hash2;logic;move;openmethod;outcome;parameter;pool;predef;scope;scope_exit;signals2;smart_ptr;stl_interfaces;throw_exception` |
| `OPENSSL_INCLUDE_DIR` | `{DependencyRoot:openssl}\include` |
| `ADECC_BOOST_OPENSSL_SSL_LIBRARY` | `{DependencyRoot:openssl}\lib\win64\{Configuration}\libssl.lib` |
| `ADECC_BOOST_OPENSSL_CRYPTO_LIBRARY` | `{DependencyRoot:openssl}\lib\win64\{Configuration}\libcrypto.lib` |
| `ZLIB_INCLUDE_DIR` | `{InstallRoot}\packages\zlib\1.3.2\include` |
| `ZLIB_LIBRARY` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibImportLibrary}` |
| `BOOST_GATE` | `closure` |
| `BOOST_PACKAGE_CMAKE_ROOT` | `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\Debug\cmake` |
| `ADECC_BOOST_MANAGED_INCLUDE_DIR` | `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include` |
| `BOOST_GATE` | `math-numerics` |
| `BOOST_GATE` | `state-parsing-meta` |
| `BOOST_GATE` | `algorithms-containers-data` |
| `BOOST_GATE` | `concurrency-async` |
| `BOOST_GATE` | `system-io-runtime` |
| `BOOST_GATE` | `foundation-language` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_PROJECT_INCLUDE:FILEPATH={BuildFileDir}\cmake\boost\BuildEngineBoostProject.cmake`
- `-DCMAKE_CXX_STANDARD:STRING=23`
- `-DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON`
- `-DCMAKE_CXX_EXTENSIONS:BOOL=OFF`
- `-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DCMAKE_PROJECT_INCLUDE:FILEPATH={BuildFileDir}\cmake\boost\bcc64x-native-clang\project-include.cmake`

**Versioned source locations:**

- `https://github.com/boostorg/boost/releases/download/boost-{LibraryVersion}/boost-{LibraryVersion}-cmake.zip`

**Declared extraction profiles:**

- `zip / boost-{LibraryVersion}`

**Package-path convention:** `{InstallRoot}/packages/boost/1.92.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

A consumer must place the **managed Boost include directory first**, before any other Boost installation, otherwise it can mix header revisions with different compiled Boost libraries. Avoid normalizing this important include-search-order requirement away.

**No active version-bound `.patch` file is referenced by this library's present XML.** That is a meaningful result: source changes must not be invented merely because porting was difficult. Additional Admin-side adapters, generated configuration or post-install repairs may still be used.

### Declarative repairs and preparation actions

- `remove-installed-bin`
- `remove-installed-lib`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Canonical Boost model from the target-machine evidence: - one official upstream CMake source tree; - one coherent Release graph and one coherent Debug graph; - 148 selected source modules covering the 171 verified logical libraries; - OpenCL/MPI/Python-dependent external ecosystems remain explicitly excluded; - component separation happens at the consumer/evidence gates, never by merging binaries 
- R189/R196 evidence classifies Boost.Random as header/INTERFACE. Upstream CMake otherwise enables the optional random_device binary by default.
- Exact managed dependency coordinates used by the evidence build.
- Evidence preflight: fail before the large Boost graph if BCC64X or Boost.Config no longer satisfies the proven native-Clang contract.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{Workspace}\LICENSE_1_0.txt -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE_1_0.txt`

**Declared smoke tests:**

- `closure (Release, scope published; source smokes\boost\component-gate)`
- `closure (Debug, scope package; source smokes\boost\component-gate)`
- `math-numerics (Release, scope published; source smokes\boost\component-gate)`
- `math-numerics (Debug, scope package; source smokes\boost\component-gate)`
- `state-parsing-meta (Release, scope published; source smokes\boost\component-gate)`
- `state-parsing-meta (Debug, scope package; source smokes\boost\component-gate)`
- `algorithms-containers-data (Release, scope published; source smokes\boost\component-gate)`
- `algorithms-containers-data (Debug, scope package; source smokes\boost\component-gate)`
- `concurrency-async (Release, scope published; source smokes\boost\component-gate)`
- `concurrency-async (Debug, scope package; source smokes\boost\component-gate)`
- `system-io-runtime (Release, scope published; source smokes\boost\component-gate)`
- `system-io-runtime (Debug, scope package; source smokes\boost\component-gate)`
- `foundation-language (Release, scope published; source smokes\boost\component-gate)`
- `foundation-language (Debug, scope package; source smokes\boost\component-gate)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

**Mandatory Boost include precedence:** The managed versioned Boost include root must precede any other directory containing `boost/` headers. Resolve this in the CMake target or consumer's target-specific include ordering, never by copying headers over other packages.

## Verification and evidence boundaries

The contract includes one or more upstream tests, validation actions or consumer smokes. Their mere declaration does not establish a successful execution; use the FSM state evidence and process logs for the specific version and configuration.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.
