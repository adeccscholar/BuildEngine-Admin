# Library Extensions

[TOC|Content]

This document defines the BuildEngine **library extension** model. A library extension is a logically independent library that is built, installed, documented and described as its own component while deliberately extending the physical source, producer and installation tree of another BuildEngine library.

The first concrete use case is the split of **ACE 8.0.6** and **TAO 4.0.6**. ACE is independently usable. TAO is an independently versioned middleware component built on ACE, but the DOCGroup release deliberately places TAO below `ACE_wrappers\TAO` and uses the same `ACE_wrappers\bin` and `ACE_wrappers\lib` producer directories. BuildEngine must represent both facts at the same time instead of either merging both components into one logical library or inventing a physical separation that does not exist upstream.

## Design goals

The extension model has the following goals:

- keep the logical component boundary independent from the physical upstream layout;
- represent the base library as an explicit graph dependency of the extension;
- avoid duplicate source acquisition and extraction when both components are delivered by one upstream archive;
- let an extension continue inside a base library's variant-specific producer tree;
- declare exactly which producer subtrees are intentionally shared;
- install the extension as an overlay into the base library's installation root when that is the upstream product shape;
- keep state, metadata, license information, SBOM, documentation and library inventory separate;
- preserve variant identity: `Release` extends `Release`, `Debug` extends `Debug`;
- keep artifact ownership explicit in install and publish mappings even when files physically share directories;
- make the model generic rather than adding ACE/TAO-specific code paths.

## Normal dependency versus extension

A normal dependency and an extension are deliberately different relationships.

| Property | Normal dependency | Library extension |
| --- | --- | --- |
| Logical library identity | independent | independent |
| Version | independent | independent |
| State scopes | independent | independent |
| Metadata / SBOM component | independent | independent |
| Source acquisition | normally independent | may be owned by the base library |
| Source root | own workspace/source root | relative subtree of base source root |
| Variant producer tree | independent | may continue in base producer tree |
| `bin` / `lib` producer directories | independent | explicitly shareable |
| Installation root | own package root | may overlay base package root |
| Graph relationship | dependency | special base dependency |
| Transitive SBOM dependency | yes | yes, through base component |

A library must not declare the same base component both as a normal `<dependency>` and as `<extension>`. The extension already is the dependency relationship.

## XML contract

Schema version 15 introduces an optional `<extension>` element on a library. A non-extension library continues to own a non-empty `<source>` contract. An extension does not declare its own `<source>`; its source is derived from the base library.

The generic form is:

```xml
<library id="extension-id"
         version="extension-version"
         category="..."
         timestamp="...">
   <extension library="base-id"
              version="base-version"
              source="relative/source/subtree"
              producer="relative/producer/root"
              install=".">
      <shared path="bin"/>
      <shared path="lib"/>
   </extension>

   <metadata name="..."
             description="One-line description ..."
             supplier="..."
             homepage="..."/>

   <build>
      ...
   </build>
</library>
```

### `extension/@library`

The logical BuildEngine library ID of the base library. The ID must resolve to exactly one library in the same `build-libraries.xml` contract and must not refer to the extension itself.

### `extension/@version`

The exact base-library version required by the extension. The resolved base library must have the same version. BuildEngine does not silently substitute another base version.

### `extension/@source`

A relative path below the **base source root**. The extension source root is:

```text
ExtensionSourceRoot = BaseSourceRoot / extension/@source
```

The path must be relative, must not contain `..`, and must exist after the base `source` scope completes.

An extension has no `download` and no `extract` actions of its own. The base library is the source owner.

### `extension/@producer`

A relative path below the variant-specific base build directory identifying the producer root that the extension continues to use:

```text
ExtensionBuildRoot    = BuildRoot/packages/<base>/<base-version>/<Configuration>
ExtensionProducerRoot = ExtensionBuildRoot / extension/@producer
```

The path must be relative and must not contain `..`.

The producer root does not transfer logical ownership of the base component to the extension. It only declares that the extension deliberately continues within that physical producer tree.

### `extension/@install`

A relative path below the base library package root. `.` means the exact base package root:

```text
BaseInstallRoot      = InstallRoot/packages/<base>/<base-version>
ExtensionInstallRoot = BaseInstallRoot / extension/@install
```

This allows an extension to be delivered **inside** the base installation instead of creating an artificial sibling package directory.

The path must be relative and must not contain `..`.

### `extension/shared/@path`

Declares a producer subtree that both the base and the extension intentionally use. The declaration is relative to `ExtensionProducerRoot`.

For ACE/TAO the shared producer directories are:

```xml
<shared path="bin"/>
<shared path="lib"/>
```

The declaration is important because a logical library split must not accidentally cause BuildEngine to create separate physical output roots for directories that upstream deliberately shares.

A shared directory does **not** imply shared logical artifact ownership. ACE-owned and TAO-owned files remain distinguished by their install/publish mappings.

## Runtime variables

For an extension BuildEngine exposes the following additional variables to technical actions:

| Variable | Meaning |
| --- | --- |
| `{ExtensionLibraryId}` | base library ID |
| `{ExtensionLibraryVersion}` | base library version |
| `{ExtensionWorkspace}` | base library workspace |
| `{ExtensionSourceRoot}` | base source root plus `extension/@source` |
| `{ExtensionBuildRoot}` | variant-specific base build directory |
| `{ExtensionProducerRoot}` | base build directory plus `extension/@producer` |
| `{ExtensionInstallRoot}` | base package root plus `extension/@install` |

For non-extension libraries these variables are not part of the contract and must not be referenced.

## Source ownership

The base library owns source acquisition and extraction.

For ACE/TAO the source graph becomes:

```text
ace:source
   |
   +--> Workspace/ace/ACE_wrappers
            |
            +--> ace/...
            +--> TAO/...     <- TAO source subtree
```

TAO therefore has a logical `tao:source` state but no independent source actions. The logical source job:

1. depends on `ace:source`;
2. resolves `ExtensionSourceRoot` to `...\ACE_wrappers\TAO`;
3. requires the subtree to exist;
4. commits its own logical source state with the ACE source state as direct upstream evidence.

This gives TAO an independently invalidatable source state without downloading or extracting the same release twice.

## Variant producer semantics

A base library first establishes its variant-specific producer tree. The corresponding extension build then continues in that tree.

For ACE and TAO:

```text
BuildRoot/packages/ace/8.0.6/Release/
   ACE_wrappers/
      ace/
      TAO/
      bin/
      lib/
```

and separately:

```text
BuildRoot/packages/ace/8.0.6/Debug/
   ACE_wrappers/
      ace/
      TAO/
      bin/
      lib/
```

The graph is variant-preserving:

```text
ace:build:Release -> tao:build:Release
ace:build:Debug   -> tao:build:Debug
```

TAO must never create a second physical producer tree merely because it has its own logical library ID.

## Shared output directories and ownership

A declared shared path authorizes the physical producer overlap. It does not make the whole directory an indivisible package.

For example:

```text
ACE_wrappers\bin\ace_gperf.exe     owner: ACE
ACE_wrappers\bin\tao_idl.exe       owner: TAO
ACE_wrappers\lib\ACE*.dll          owner: ACE
ACE_wrappers\lib\TAO*.dll          owner: TAO
```

The exact file ownership remains expressed through the extension's install and publish mappings. An extension must only copy the artifacts it owns.

This has two practical consequences:

- extension install actions targeting shared directories must not clean the entire shared target directory;
- publish manifests remain per logical library, so a later update can distinguish files contributed by ACE from files contributed by TAO.

The extension declaration therefore controls *where sharing is legal*, while the existing include/exclude mappings control *which files the extension owns*.

## Installation overlay

The extension package is physically overlaid onto the base installation.

The desired result for ACE with TAO is conceptually:

```text
packages/ace/8.0.6/
   include/
      ace/
      tao/
      orbsvcs/
   bin/
      win64/
         Release/
         Debug/
   lib/
      win64/
         Release/
         Debug/
   tools/
      bin/
   services/
      bin/
   TAO/
      ... optional preserved upstream layout ...
```

There is no requirement for a physical sibling tree such as:

```text
packages/tao/4.0.6/
```

for the actual runtime/development payload. TAO's logical identity is retained in BuildEngine state, metadata, documentation and publish manifests.

The overlay order is explicit:

```text
ace:install:<Configuration>
        |
        +--> tao:install:<Configuration>
```

and the same rule applies to common installation content.

## State model

The established logical state model is retained.

ACE owns states such as:

```text
ace:source
ace:build:Release
ace:test:Release
ace:install:Release
ace:install:common
ace:install
```

TAO owns its own states:

```text
tao:source
tao:build:Release
tao:test:Release
tao:install:Release
tao:install:common
tao:install
```

Extension edges become normal direct upstream references for persistent state purposes. Therefore a TAO state becomes non-current when the referenced ACE library timestamp or relevant ACE scope `completedAt` changes.

The physical sharing of a producer or install directory is **not** used as state evidence. The DAG and logical timestamps remain authoritative.

## Metadata and SBOM

An extension is a full logical SBOM component.

ACE:

```text
ACE 8.0.6
 +-- OpenSSL
 +-- Xerces-C
 +-- zlib
```

TAO:

```text
TAO 4.0.6
 +-- ACE 8.0.6
      +-- OpenSSL
      +-- Xerces-C
      +-- zlib
```

The extension edge is therefore added to the metadata dependency graph exactly once. ACE does not need to be repeated as a normal `<dependency>` on TAO.

`metadata/@description` is emitted as the CycloneDX component `description` when present.

## Documentation

The logical split also defines separate documentation products:

```text
documentation/ace/8.0.6/...
documentation/tao/4.0.6/...
```

ACE documentation must no longer absorb TAO merely because both source trees arrived in the same archive. TAO documentation resolves its source root from `ExtensionSourceRoot`.

The generated library main page uses the contract `metadata/@description`. The description is intended to answer *what the library is*, while integration notes answer *how BuildEngine builds it*.

For TAO, Doxygen may later consume an ACE tag file so that TAO API documentation links to ACE API symbols rather than reparsing and duplicating ACE documentation. That optimization is compatible with the extension graph but is not required for the base extension contract.

## ACE / TAO contract example

The intended logical contract is:

```xml
<library id="ace" version="8.0.6" category="middleware" timestamp="...">
   <dependency library="openssl" version="3.5.8"/>
   <dependency library="xerces-c" version="3.3.0"/>
   <dependency library="zlib" version="1.3.2"/>

   <metadata
      name="ACE"
      description="Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction."
      supplier="DOC Group"
      homepage="https://www.dre.vanderbilt.edu/~schmidt/ACE.html"/>

   <source>
      <download
         url="https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-8_0_6/ACE%2BTAO-{LibraryVersion}.zip"
         archive="ACE+TAO-{LibraryVersion}.zip"
         sha256="e741c8b0ec0c7d6747b184d674567b1e73a13bdf2c902485d1299bbf267b3fba"/>
      <extract format="zip" root="ACE_wrappers">
         <require path="COPYING"/>
         <require path="VERSION.txt"/>
         <require path="ace\config-win32.h"/>
         <require path="bin\mwc.pl"/>
         <require path="MPC\config\zlib.mpb"/>
         <require path="TAO\VERSION.txt"/>
      </extract>
      ... ACE/source-wide version-bound repairs ...
   </source>

   <build>
      ... create and build the ACE producer tree ...
   </build>
</library>

<library id="tao" version="4.0.6" category="middleware" timestamp="...">
   <extension
      library="ace"
      version="8.0.6"
      source="TAO"
      producer="ACE_wrappers"
      install=".">
      <shared path="bin"/>
      <shared path="lib"/>
   </extension>

   <metadata
      name="TAO"
      description="CORBA object request broker and middleware services built on top of ACE."
      supplier="DOC Group"
      homepage="https://www.dre.vanderbilt.edu/~schmidt/TAO.html"/>

   <build>
      <environment name="ACE_ROOT" value="{ExtensionProducerRoot}"/>
      <environment name="TAO_ROOT" value="{ExtensionProducerRoot}\TAO"/>
      <environment name="PATH" value="{ExtensionProducerRoot}\lib;{ExtensionProducerRoot}\bin;{ENV:PATH}"/>

      ... generate TAO workspace, build TAO and services ...

      <install>
         <perVariant>
            ... copy only TAO-owned libraries/DLLs into {ExtensionInstallRoot} ...
         </perVariant>
         <common>
            ... overlay tao/orbsvcs headers into {ExtensionInstallRoot}\include ...
         </common>
      </install>
   </build>
</library>
```

The example is intentionally explicit about the shared producer and install roots. No special `ace-tao` runtime branch is required in BuildEngine.

## Validation rules

Schema 15 and the runtime loader enforce the following rules:

1. a library may have at most one `<extension>`;
2. an extension requires `@library`, `@version`, `@source` and `@producer`;
3. `@install` defaults to `.`;
4. source, producer, install and shared paths must be relative and must not contain `..`;
5. the base library must exist in the same contract;
6. the base version must match exactly;
7. a library must not extend itself;
8. an extension must not also declare the same base as a normal dependency;
9. an extension must not declare its own `<source>` contract;
10. a non-extension library still requires a non-empty `<source>` contract;
11. extension cycles are invalid;
12. shared paths must be unique within an extension;
13. each extension source job requires the resolved source subtree to exist after the base source job;
14. each extension build variant depends on the same variant of the base build;
15. each extension install variant overlays only after the same base install variant is complete.

## Description field

Every logical library should carry a concise human-facing one-line description in `metadata/@description`.

The field is not an integration status and must not contain BuildEngine-specific build details. Those remain in `libraries.md` integration notes.

The description is consumed by:

- generated library documentation;
- the central library/documentation presentation where contract metadata is available;
- package/component metadata;
- CycloneDX `component.description`;
- human-facing inventory tables.

### Current library descriptions

| Library ID | Display name | Description |
| --- | --- | --- |
| `pugixml` | pugiXML | Lightweight C++ XML processing library with DOM-style parsing and XPath support. |
| `zlib` | zlib | General-purpose DEFLATE compression library used as a foundational compression dependency. |
| `brotli` | Brotli | Lossless compression library and codec optimized for web and general data compression. |
| `zstd` | Zstandard | Fast lossless compression library offering a wide compression-speed trade-off. |
| `xz` | XZ / liblzma | LZMA/LZMA2 compression library and XZ container implementation. |
| `libzip` | libzip | C library for reading, creating, and modifying ZIP archives. |
| `libarchive` | libarchive | Multi-format archive and compression library backing tar/cpio-style workflows. |
| `openssl` | OpenSSL | TLS/SSL and general-purpose cryptography toolkit and provider library. |
| `curl` | curl | Client-side URL transfer library with HTTP(S) and related protocol support. |
| `boost` | Boost | Large collection of portable C++ libraries extending the standard library ecosystem. |
| `nlohmann-json` | nlohmann-json | Header-only modern C++ JSON parser, serializer, and data model. |
| `ace` | ACE | Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction. |
| `tao` | TAO | CORBA object request broker and middleware services built on top of ACE. |
| `bzip2` | bzip2 | Lossless block-sorting compression library implementing the bzip2 format. |
| `glew` | GLEW | OpenGL extension loading library exposing modern OpenGL entry points. |
| `opengl` | OpenGL / Mesa | Mesa software OpenGL implementation providing the managed Windows desktop OpenGL runtime. |
| `raylib` | raylib | Small C game-programming library for graphics, input, audio, and windowing. |
| `sdl2` | SDL2 | Cross-platform low-level multimedia library for windows, input, audio, and graphics integration. |
| `sqlite` | SQLite | Embedded transactional SQL database engine delivered as a self-contained library. |
| `xerces-c` | Xerces-C | C++ XML parser and validation library implementing DOM/SAX and XML Schema support. |
| `soil2` | SOIL2 | Small OpenGL texture-loading library for common image formats. |
| `vtk` | VTK | Visualization Toolkit for scientific data structures, processing, and visualization pipelines. |
| `opencv` | OpenCV | Computer vision and image-processing library with broad algorithm and matrix support. |
| `cmark-gfm` | cmark-gfm | GitHub-flavored CommonMark parser and renderer used for BuildEngine documentation. |
| `googletest` | GoogleTest | C++ unit-testing and mocking framework from the GoogleTest project. |
| `catch2` | Catch2 | Modern C++ unit-testing framework with self-contained test registration and assertions. |
| `bitmapplusplus` | BitmapPlusPlus | Header-only C++ bitmap utility for reading, writing, and manipulating BMP images. |
| `libjpeg-turbo` | libjpeg-turbo | High-performance JPEG codec using the libjpeg API and TurboJPEG interface. |
| `libpng` | libpng | Reference PNG image encoding and decoding library. |
| `harfbuzz` | HarfBuzz | OpenType text shaping engine converting Unicode text into positioned glyphs. |
| `freetype` | FreeType | Font rasterization engine for loading and rendering scalable and bitmap fonts. |
| `libtiff` | libtiff | TIFF image file reading, writing, and image metadata library. |
| `skia` | Skia | 2D graphics engine for raster, vector, text, image, and GPU-backed rendering. |

These descriptions are the canonical wording to copy into the corresponding `metadata/@description` attributes unless later library-specific review changes a description.

## Required implementation surfaces

The extension contract is not complete when only the XSD accepts it. Every runtime surface that derives physical paths or component relationships must understand the extension.

| Surface | Required behavior |
| --- | --- |
| XML schema | define `<extension>`, its paths and shared producer subtrees; make `<source>` optional only for extension libraries |
| Library configuration parser | validate and resolve the base library/version and extension paths |
| Tool-reference scan | continue scanning the complete library node; extension attributes may contain no tool references in schema 15 |
| Source job | depend on base `source`, require `ExtensionSourceRoot`, write own logical source state |
| Build job | depend on base build of same variant and expose extension variables |
| Test / validation | use the same extension producer context for that variant |
| Install | wait for base install of same variant and overlay into `ExtensionInstallRoot` |
| Publish | resolve extension payload from the effective overlay root while retaining a per-library manifest |
| State | keep the extension's own scopes and record base scopes as direct upstream references |
| Metadata graph | treat extension base as a direct dependency in addition to normal dependencies |
| CycloneDX | emit the extension as its own component and emit `metadata/@description` as `description` |
| Source metadata extraction | inspect `ExtensionSourceRoot` for an extension instead of requiring its own extract action |
| Doxygen/source discovery | use `ExtensionSourceRoot` for extension project text/source discovery |
| Generated library main page | show the one-line description and extension relationship |
| Central inventory | present ACE and TAO as separate logical libraries while preserving the extension relation |

## ACE/TAO migration result

After migration the old aggregate `ace-tao` logical library disappears. The contract contains:

```text
ace 8.0.6
  dependencies: openssl 3.5.8, xerces-c 3.3.0, zlib 1.3.2

tao 4.0.6
  extension of: ace 8.0.6
```

Both still use the same upstream ACE+TAO release and the same physical `ACE_wrappers` producer tree. TAO does not download, extract or recreate that tree. TAO extends it and overlays its owned files into the ACE installation.

This is the central rule of the model:

> **Logical component separation does not imply physical tree separation. A library extension is independently versioned and tracked, but deliberately continues inside a declared base library tree.**
