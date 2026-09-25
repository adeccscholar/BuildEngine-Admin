# BuildEngine-Admin

This repository is the **declarative administration layer** of the C++Builder third-party integration project. The overall project aims to provide reproducible evidence of the extent to which current C and C++ libraries can be built, tested, packaged, and consumed by normal consumer projects with **Embarcadero C++Builder 13 / BCC64X**.

The project documentation is maintained in English so the project is accessible to the wider C and C++ community.

## Current status

BuildEngine has moved from its original task/global-DAG orchestration to a **Library-FSM architecture**. Each active Library/Version follows the fachlich lifecycle:

```text
Source -> Build -> Test -> Validation -> Install -> Metadata -> Publish -> Documentation -> Ready
```

Technical jobs and local Action graphs execute only work released by that state machine. `ProcessScheduler` remains technical queue/worker infrastructure; it is no longer the fachlich model of a library.

The Admin contract has continued to evolve beyond the old September freeze baseline. In particular, managed `libarchive 3.8.9` now depends explicitly on `bzip2 1.0.8` so BuildEngine's private libarchive runtime can gain native in-process `.tar.bz2` support.

That BZip2/private-libarchive transition is **not verified** until the rebuilt BuildEngine runtime reports:

```text
[LIBARCHIVE] filter bzip2      : in-proc ...
```

Git and Bash now share one pinned official PortableGit payload. The bootstrap resolves `cmd\\git.exe` from that managed root, while the logical `bash` tool resolves `usr\\bin\\bash.exe` from the same root. This removes the separate MinGit payload and avoids reconstructing MSYS virtual links from the raw TAR archive while preserving distinct logical tool IDs.


## Role in the overall project

```text
adeccscholar/BuildEngine
   private C++23 application
   Library-FSM + Orchestrator, logical scope state, generic technical execution,
   repository synchronization, publish, documentation, and smoke orchestration

adeccscholar/BuildEngine-Admin        <-- this repository
   declarative tool and library contracts
   XSD schemas, CMake/toolchain adapters, version-bound patches,
   security metadata, and small package-related consumer smokes

adeccscholar/BuildEngine-Tests
   more complex integration, demonstration, and learning environment
```

This separation is binding.

## Authoritative library contract

`admin/build-libraries.xml` is the **single normative library and dependency contract**.

Current state:

```text
schemaVersion = 16
current synchronized logical library catalog
```

Included (44 logical libraries):

```text
pugixml, zlib, brotli, zstd, xz, libzip, libarchive, openssl, curl,
boost, nlohmann-json, ace, tao, bzip2, glew, opengl, raylib, sdl2,
sqlite, libpq, libpqxx, xerces-c, soil2, vtk, opencv, cmark-gfm,
googletest, catch2, bitmapplusplus, libjpeg-turbo, libpng, harfbuzz,
freetype, libtiff, skia, graphite2, expat, teckit, icu, libxml2,
qpdf, podofo, win-iconv, poppler
```

The catalog now covers database clients (SQLite and PostgreSQL), XML, PDF/document processing, text shaping/Unicode, test frameworks, graphics, compression, networking and middleware.

## Core principle

```text
official upstream
-> reproducible download / source pin
-> extraction
-> explicit version-bound patch when required
-> original build system
-> BCC64X build
-> upstream tests where meaningful
-> versioned package
-> explicit require gates
-> publish into the consumer tree
-> small consumer smoke
-> central Doxygen documentation when enabled
```

An alternative compiler must never silently replace BCC64X.

BCC64X is treated as **Clang/LLVM with Embarcadero simulation and GNU-compatible target/header conventions**, not as a MinGW/GCC compiler. MinGW compatibility macros or paths must never be used as proof that the active compiler is MinGW.

**Runtime policy:** every BCC64X Windows executable, DLL and module must use the **dynamic runtime** (`-tR`). Static BCC64X runtime linkage is forbidden. This is a hard CMake/toolchain invariant because C++ ownership, STL objects and allocator activity cross DLL boundaries in real packages such as PoDoFo.

## Generic technical actions

The current XML contract uses actions including:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 16 XML uses optional technical graph metadata (`id`, `dependsOn`). These dependencies order Actions inside technical work; they do not replace the Library-FSM or create persistent state. The XML and active XSD contract are aligned on Schema 16.

## Publish manifest and Doxygen

Besides their ownership/incremental role, publish manifests now have another generic use in the BuildEngine core: with `WithDoxygen=true`, they form the **authoritative file list for public API documentation**.

For a published library, BuildEngine reads:

```text
<PublishRoot>\.buildengine\manifests\<library>.manifest
```

and passes the documentable, actually published header, IDL, and C++ module files from that manifest to Doxygen. This prevents the entire shared consumer tree from accidentally being attributed to one library.

Central output resides in the production root:

```text
documentation\index.html
documentation\<library>\<version>\html\index.html
```

`admin/build-tools.xml` already contains the managed Doxygen contract (currently 1.18.0); no tool/schema change was required for this extension.

## Copy and require

`<copy>` supports simple and filtered packaging operations, including recursive, overwrite, include/exclude, flatten, cleanTarget, and singleFile.

Standalone `<require>` nodes support:

```xml
<require path="..."/>
<require path="..." kind="file"/>
<require path="..." kind="directory"/>
<require path="..." kind="any"/>
```

The `<require>` nested inside `<extract>` remains file-based source evidence.

## Package-related smokes

Small smokes live below `admin/smokes/<library>/...`. They prove only the published consumer contract. Complex multi-process and integration scenarios belong in `BuildEngine-Tests`.

## Technically necessary special programs

`admin/programs/opengl/meson_bootstrap.py` remains a deliberately accepted Mesa/Meson/BCC64X compatibility bridge. Generic orchestration belongs in the BuildEngine core instead.

## Reproducibility and evidence

A credible clean-room proof should identify at least:

- BuildEngine commit,
- BuildEngine-Admin commit,
- BuildEngine-Tests commit where used,
- schema version,
- library version and source pin,
- compiler and tool versions,
- effective build parameters,
- applied patches,
- package/publish results,
- test/smoke results,
- central Doxygen output,
- `buildlog_*.log` as performance evidence,
- machine-state summary,
- an unchanged second run proving full CURRENT state.

## Next verification milestone

The private-libarchive/BZip2 gate has been crossed in the active contract: Bash is restored as a pinned managed tool and TECkit/ICU consume it explicitly instead of reaching through Git's private `usr/bin` tree. A complete Clean-Room run remains the verification milestone for this combined path.

A complete Clean-Room run remains a later, separately recorded evidence milestone. Admin contracts are no longer frozen; each material contract change must update its logical timestamp and the maintained documentation together.

## Important documents

```text
README.md
TODO.md
docs/FREEZE_CLEANROOM.md
docs/bcc64x-library-integration-findings.md
docs/bcc64x-ucrt-runtime-link-bug.md
docs/catch2-bcc64x-integration.md
docs/library-license-sbom.md
admin/README.md
```

The live project documentation served by BuildEngine Server is maintained under `admin/server-docs/` and uses the `[TOC|Content]` navigation directive.

## License

Project-owned content in this public Admin repository is licensed under the MIT License unless an individual file states otherwise. Third-party sources and their license texts retain their respective upstream licenses.
