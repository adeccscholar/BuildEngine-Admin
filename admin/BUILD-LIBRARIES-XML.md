# `build-libraries.xml` – Current Contract Guide

**Status date:** September 3, 2026  
**Schema:** 13  
**Documentation language:** English

This document describes the semantics of `admin/build-libraries.xml` at the documented Schema-13 state. The XSD file `admin/schemas/build-libraries.xsd` is the formal structural definition; this document explains the technical meaning. For the current Schema-14 contract, also see `server-docs/build-libraries.md`.

## 1. Core idea

`build-libraries.xml` is the project's single normative library and dependency contract.

```xml
<buildLibraries schemaVersion="13">
   <library ...>
      ...
   </library>
</buildLibraries>
```

Each library declaratively describes:

- ID and version,
- modification timestamp of the complete contract,
- dependencies,
- metadata/licenses,
- upstream source,
- build/variants,
- install/packaging,
- publish,
- small package smokes.

## 2. Library node

Example:

```xml
<library id="example" version="1.2.3" timestamp="2026-09-03T12:00:00Z">
   ...
</library>
```

`timestamp` is advanced when the effective library contract changes, for example source, build options, patch, install layout, require gates, publish, or smoke behavior.

## 3. Dependencies

```xml
<dependency library="zlib" version="1.3.2"/>
```

Dependencies are modeled only where a real technical dependency exists. They are not used to force artificial scheduler ordering.

BuildEngine supplies managed package paths to dependent producer builds through environment/CMake search paths.

## 4. Metadata and licensing

Libraries can declare upstream and license information. This data feeds package metadata, `LICENSE-INFO.txt`, and CycloneDX SBOM evidence.

## 5. Source

A source contract consists of generic actions, typically:

```xml
<source>
   <download .../>
   <extract ...>
      <require path="CMakeLists.txt"/>
      <require path="include\library.h"/>
   </extract>
</source>
```

### Download

```xml
<download url="..." archive="..." sha256="..."/>
```

`sha256` is used where a stable upstream hash is available or the contract requires a fixed archive identity.

### Extract

```xml
<extract format="zip" root="library-{LibraryVersion}">
   <require path="CMakeLists.txt"/>
</extract>
```

Supported formats include ZIP and libarchive-based archives.

Important: the **nested** `<extract><require>` is deliberately file evidence inside the extracted upstream artifact. It is not identical to the general later `<require>` action.

## 6. Build

A typical build contract contains:

```xml
<build>
   <environment name="..." value="..."/>
   <argument value="..."/>
   <cmake mode="configure" .../>
   <cmake mode="build" .../>

   <variant name="Release">...</variant>
   <variant name="Debug">...</variant>

   <install>...</install>
</build>
```

Release and Debug use separate build directories. Shared arguments live on the main node; variants contain only their differences.

## 7. CMake

```xml
<cmake mode="configure"
       executable="{Tool:cmake}"
       source="..."
       build="..."/>

<cmake mode="build"
       executable="{Tool:cmake}"
       build="..."/>

<cmake mode="install"
       executable="{Tool:cmake}"
       build="..."/>
```

The CMake path does not replace the upstream CMake project. BuildEngine only creates the reproducible invocation and dependency environment.

## 8. Execute

```xml
<execute name="test"
         executable="..."
         workingDirectory="..."
         successExitCode="0"
         showOutput="false">
   <environment name="..." value="..."/>
   <argument value="..."/>
</execute>
```

`execute` is generic and is used for real upstream tools, test drivers, or necessary build-system commands.

## 9. Copy

### Simple copy

```xml
<copy source="..." target="..." overwrite="true"/>
```

### Recursive copy

```xml
<copy source="..." target="..." recursive="true" overwrite="true"/>
```

### Filtered copy

```xml
<copy source="..."
      target="..."
      recursive="true"
      overwrite="true"
      flatten="true"
      cleanTarget="true">
   <include pattern="**/*.dll"/>
   <include pattern="**/*.pdb"/>
   <exclude pattern="**/tests/**"/>
</copy>
```

Semantics:

- `recursive`: recursive search/copy,
- `overwrite`: replace existing target files,
- `flatten`: place selected files in the target without their original subdirectories,
- `cleanTarget`: clean the target before the operation,
- `singleFile`: require exactly one selected source file and copy it to the specified target path,
- `<include pattern="...">`: positive selection,
- `<exclude pattern="...">`: subsequent exclusions.

Pattern behavior:

- `*` does not cross a directory boundary,
- `**` may cross subdirectories,
- `?` represents one non-separator character,
- Windows semantics are treated ASCII-case-insensitively.

With `flatten`, colliding file names produce an error rather than a silent overwrite.

The extended copy function is proven by the ACE/TAO packaging path. The complete target-machine run after removal of the Python installer ended with 295/295 PASS.

## 10. `preserveCurrentArtifact`

Several actions normally modify `{CurrentArtifact}`. Where supported, a technical helper operation that should not take over this context can set `preserveCurrentArtifact="true"`.

## 11. Require

### Historical and still-valid default

```xml
<require path="...\file.dll"/>
```

is identical to:

```xml
<require path="...\file.dll" kind="file"/>
```

### Directory

```xml
<require path="...\include" kind="directory"/>
```

The path must exist and actually be a directory.

### Any filesystem entry

```xml
<require path="...\generated" kind="any"/>
```

The path only has to exist.

Allowed values:

```text
file
directory
any
```

## 12. Test-bound actions

Actions can be coupled to the global test selection with:

```xml
test="true"
```

Where supported by the action type, `phase` can additionally distinguish test and validation roles.

Upstream tests are not removed without analysis. If the product form logically excludes individual upstream tests, that decision is documented.

## 13. Install

For compiled libraries:

```xml
<install>
   <perVariant>
      ...
   </perVariant>
   <common>
      ...
   </common>
</install>
```

`perVariant` processes Release/Debug-specific artifacts. `common` processes shared headers, license texts, or other configuration-independent content.

## 14. Publish

```xml
<publish root="Win64x"
         configuration="Release"
         consumer="{BuildFileDir}\cmake\consumer">
   <tree .../>
   <files .../>
   <cmake .../>
</publish>
```

Publish projects the versioned producer package into the shared consumer tree. The versioned package remains authoritative.

`requiresAllVariants="true"` can be used when publishing must wait until the complete shared SDK tree is stable.

## 15. Smoke

Small smokes are registered directly on the library node:

```xml
<smoke id="consumer"
       source="smokes\example\consumer"
       configuration="Release"
       cmake="{Tool:cmake}"
       ninja="{Tool:ninja}"
       toolchain="{BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake"
       executable="example-consumer.exe">
   <environment name="..." value="..."/>
   <argument value="..."/>
   <run executable="optional-helper.exe"/>
</smoke>
```

Smokes are not a second upstream test suite. They prove usability of the installed/published package.

More complex integration tests and demos belong in `BuildEngine-Tests`.

## 16. ACE/TAO as a reference case for the extended contract

ACE/TAO uses the generic XML contract for complex packaging:

- Release/Debug,
- many import libraries and DLLs,
- recursive filtered copies,
- flattening,
- tools such as `tao_idl` and `ace_gperf`,
- canonically named service executables,
- header trees,
- license/version files,
- publishing into the shared SDK tree.

The former custom Python installer is therefore no longer technically required.

## 17. Current product policy

For normal runtime libraries, Shared DLL + import library is the preferred product form where upstream supports it meaningfully.

Static is permitted where technically justified. GoogleTest is deliberately assessed separately as test infrastructure and may receive a documented static exception.

## 18. Change discipline

When changing `build-libraries.xml`:

1. use the complete current XML state,
2. do not reconstruct it from partial fragments,
3. advance the library timestamp for an effective change,
4. keep XSD and implementation synchronized,
5. keep patches version-bound,
6. perform a real target-machine run,
7. only then document new artifact names/contracts as proven.
