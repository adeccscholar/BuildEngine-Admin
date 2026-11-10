# Cumulative License Information and SBOM

## Purpose

For every library, BuildEngine produces two package metadata artifacts after successful installation:

- `install/packages/<LibraryId>/<LibraryVersion>/LICENSE-INFO.txt`
- `install/packages/<LibraryId>/<LibraryVersion>/sbom.cdx.json`

Both files are generated from the same dependency graph resolved by BuildEngine. Therefore the license report and SBOM refer to the same concrete package versions and their direct and transitive dependencies.

License identities are primarily derived from the actual extracted and verified upstream source revision. The library build contract is not intended as a license database and normally does not need manual license declarations.

Original license files remain part of the installed package. `LICENSE-INFO.txt` is a cumulative report, not a substitute for upstream license texts.

## Automatic source analysis

The metadata job examines the upstream source tree identified by `<source>/<extract root="...">` at runtime, after source, build and installation jobs. This also works in a clean-room environment where source trees do not exist at the time the job graph is created.

Evidence sources are considered in this order:

1. Existing standardized upstream JSON SBOM files
2. Original license files in the upstream release
3. Explicit optional `metadata` attributes from the build contract as overrides

Current detection supports CycloneDX JSON and SPDX JSON; typical files such as `LICENSE`, `LICENSE.txt`, `LICENSE.md`, `COPYING`, `COPYRIGHT`, `NOTICE`, and `UNLICENSE`; and recognition of common license texts through characteristic content patterns.

When scanning automatically, third-party vendor directories such as `vendor`, `third_party`, `external`, `deps` and similar paths are not interpreted as the root library's license source. This avoids misattributing a bundled dependency's license to the main project.

If a license file is found but cannot be mapped unambiguously to an SPDX identifier, it remains an unclassified upstream license. If no reliable information can be obtained, the field explicitly reports that automatic detection was unsuccessful. BuildEngine must not invent licensing data.

## Existing upstream SBOMs

When the source tree contains a CycloneDX or SPDX JSON SBOM, BuildEngine uses reliable component, supplier, homepage and license data from it as metadata evidence.

The first recognized root-library upstream SBOM is also preserved unchanged as:

```text
install/packages/<LibraryId>/<LibraryVersion>/sbom.upstream.json
```

This remains separate from BuildEngine's generated cumulative `sbom.cdx.json`. The latter's dependency graph is derived exclusively from the actual BuildEngine build contract and dependency resolution. Optional, test-only, tooling or other upstream relationships in a supplied SBOM are not automatically claimed to have been built or shipped.

## 21 October 2026: upstream license scanner diagnosis and repair

The generic BuildEngine source metadata scanner previously formed the source root as `WorkspaceRoot / library ID / archive extraction root`. This was incorrect: ExtractJob **promotes** the declared archive root to `WorkspaceRoot / library ID / version`. The scanner therefore failed to find evidence for many successfully built packages despite verified license files existing.

The C++ implementation now uses `library_extension::SourceRoot()`, matching the FSM's source-root convention including shared-source extensions, and produces a concrete missing-path error rather than silently reporting `[not detected automatically]`.

Version-specific recognition covers FreeType's `FTL OR GPL-2.0-or-later` choice, requiring both original texts; libpng's PNG Reference Library License v2 (`libpng-2.0`); and notices with an isolated `(C)` line after their copyright heading, as with zlib. CycloneDX now writes a real license `expression` for FreeType's OR choice rather than misclassifying it as a single license ID.

Explicit XML license entries still override detected identity, while matching upstream provenance or author details may be retained when the override lacks them. Unsupported or missing upstream license documents remain visible instead of being guessed.

The C++ regression file is `test/UpstreamMetadataTest.cpp`. At the time of the repair, native BCC64X compilation and a fresh full-catalog metadata run were **[not verified]**. Metadata state schema advanced to 6 to invalidate stale outputs. Run metadata again and inspect `LICENSE-INFO.txt` and `sbom.cdx.json` for FreeType and dependencies. A root library's license does not establish the licenses of every bundled subcomponent.

## Versioned component and provenance manifests

Libraries may supply a versioned `sbom/bundled-components.json` component manifest. It supplements the **authoritative BuildEngine dependency graph**, and must not describe a second bundled copy of a library already installed as an external BuildEngine dependency.

- `externalBuildEngineDependencies`: name, version and optional exact XML library ID (`libraryId`). All coordinates must be verified against **direct** XML `<dependency>` edges. When provided, the list must be complete and contain no duplicates. These external components already appear in the normal CycloneDX dependency graph.
- `bundledComponents`: source components actually distributed and built internally. These appear separately as bundled components in CycloneDX and the cumulative license report. Externally managed and bundled components must not share duplicate component identities.
- `runtimeDll` (optional for external dependencies): only the **declared DLL base name**, e.g. `libabseil_dll.dll`. CycloneDX records `adecc:buildengine:declared-runtime-dll`. This is **not** proof of the actual DLL imported in a PE binary.

**PDFium 2025.11.19:** Abseil is an external dependency declared directly as `abseil@20260817.0` and using the runtime name `libabseil_dll.dll`. The former Chromium component `third_party_abseil-cpp_absl.dll` must not be shipped or counted separately as `pdfium:bundled:Abseil` in CycloneDX.

The currently bundled `fast_float` source is also removed from the bundle by the newer PDFium migration and instead taken externally from the BuildEngine package `fast_float@8.2.3`.

The manifest is installed with the package and then evaluated by the Metadata FSM. Editing a source manifest in the repository **does not** retroactively repair an already-installed, unchanged SBOM: both the installation and metadata run must process the updated evidence.

## Optional metadata within the library contract

Schema 16 retains an optional `metadata` node:

```xml
<metadata name="Xerces-C++"
          supplier="Apache Software Foundation"
          homepage="https://xerces.apache.org/xerces-c/">
   <license name="Apache License 2.0"
            spdx="Apache-2.0"
            licensor="Apache Software Foundation and contributors"
            file="LICENSE">
      <summary>Concise factual summary of essential license conditions. This summary does not replace the original license text.</summary>
   </license>
</metadata>
```

The node is not a normal place for manually maintaining licenses. It is an explicit override for unusual cases where automated upstream evidence is incomplete or ambiguous.

Attributes:

- `name`: optional product/library name override
- `supplier`: optional publisher or supplier override
- `homepage`: optional project/product URL override
- `license/@name`: explicit license name
- `license/@spdx`: explicit SPDX identifier
- `license/@licensor`: explicit licensor or licensor group
- `license/@file`: relative installed original license file; BuildEngine verifies its existence
- `license/summary`: explicit factual license summary

When the override contains one or more `<license>` elements, they replace automatically detected licenses for that library. Individual `name`, `supplier` and `homepage` attributes replace only their respective automatically detected field.

## Cumulative license report

For the root library and every transitive dependency, `LICENSE-INFO.txt` contains at least:

- library ID and exact version
- relation to the root library: root, direct dependency or transitive dependency
- detected or explicitly overridden license name and SPDX identifier
- licensor where reliably identifiable
- upstream or installed license file where present
- origin and evidence of detected information
- a technical summary of the detection result

Transitive relationships come directly from BuildEngine's validated dependency graph, not from previously generated license reports. Changing a concrete dependency version cannot silently bypass the cumulative license report or SBOM.

## SBOM

BuildEngine's standard output is CycloneDX **1.7 JSON**, serialized using `nlohmann::json` while the internal metadata/dependency model remains separate.

When reliably available from upstream or explicit overrides, the SBOM includes:

- component name and version
- `bom-ref` and generic Package URL
- direct and transitive dependency relationships
- supplier
- license and SPDX identifier
- project homepage
- source download URL
- source archive SHA-256
- BuildEngine library ID and contract timestamp
- license-detection provenance as BuildEngine properties
- number of detected upstream SBOM files

The root library appears under `metadata.component`; all transitive dependencies appear as components and dependency relations. Components with no dependencies still have a dependency entry with an empty list, so an explicitly empty dependency set is not mistaken for unknown dependencies.

## Incremental state

The metadata job ID is:

```text
library:<LibraryId>:metadata
```

It depends on successful installation. State is stored under:

```text
install/packages/<LibraryId>/<LibraryVersion>/.buildengine/metadata.state
```

The state captures the root and transitive components, exact versions and contract timestamps, graph edges, and license, supplier, homepage, download and hash metadata derived from upstream SBOMs, license texts or overrides.

Metadata is regenerated when, for example, `LICENSE-INFO.txt` or `sbom.cdx.json` is absent, an exact dependency version or graph changes, the contract timestamp of a related library changes, or detected/overridden metadata changes.

Metadata jobs use the normal scheduler. They do not prevent an already built and installed SDK from satisfying a different library's build dependency, but the overall run is successful only when all scheduled metadata jobs also succeed.

## Maintenance principle

License information should be determined automatically from reliable primary sources in the exact upstream release whenever possible. Manual build-contract licensing is not the normal workflow. Explicit overrides are appropriate only for evidenced special cases or corrections to ambiguous automatic classification.
