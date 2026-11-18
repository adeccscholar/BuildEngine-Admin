# Showcases and Extended Tests

## Central workspace view

The BuildEngine central server exposes **[Showcases & Extended Tests](/showcases/)** alongside Libraries, Tools, Packages and News. Projects are discovered from `<production-root>/tests`; for the active Windows workspace this is `D:\local\embarcadero\test_v3\tests`. No second checkout or GitHub API access is required by the server.

The catalog is grouped by source directory. A project directory is recognized when it contains `Description.md` and either an independent `CMakeLists.txt` or a C++Builder `.cbproj`. Directories `bootstrap`, `cmake`, `cache`, `docs`, build output and Git metadata are excluded from enumeration. The `docu-server` project is included as an independent test application. Nested groups such as `database/mssql` remain visible.

## Complete test metadata

All **26 standalone BuildEngine-Tests applications** now declare a `showcase.json` manifest. Group-only `database/` is not an executable. The `docu-server/` documentation application is included as a standalone extended test. Every manifest provides a concise functional description and a full list of managed package roots referenced by the test, distinguishing `direct: true` for directly consumed APIs from `direct: false` for supporting runtime closure packages.

A manifest may specify `"version": "active"` when its CMake project uses `adecc_package_current_root`. The central server resolves this value against the **unique, non-superseded catalog version on every request**, then displays a version-specific hyperlink. Explicit pinned CMake versions remain pinned. An absent or ambiguous catalog coordinate is reported as unresolved, **never guessed and never linked to a different version**.

External dependencies such as adecc-core-lib, RAD Studio VCL, database servers, ODBC drivers, the Windows OpenGL driver and online weather services are displayed separately because they are not normal BuildEngine package coordinates.

The central showcase detail page embeds the full project's `Description.md`, in addition to links to `build.md`, `SBOM.md` and `LICENSE.md`. Short test descriptions are also present in the source project's own Markdown pages for the independent documentation server.

Temporary source-project provenance was removed from the OpenXlsxGrid, MSSQL and PostgreSQL descriptions; they now document the actual tests and their prerequisites.

## Versioned relationship model

Each project contains a `showcase.json` manifest with the following format:

```json
{
   "schemaVersion": 1,
   "kind": "showcase",
   "title": "ImPlot Signal Theater",
   "summary": "Two 768-point signals from native ImPlot draw lists",
   "dependencies": [
      { "library": "imgui", "version": "1.93.0", "direct": true },
      { "library": "implot", "version": "1.1", "direct": true }
   ],
   "status": "not-verified"
}
```

The central server validates exact `library + version` coordinates against `admin/build-libraries.xml`. It never substitutes the latest available version. A project's declared dependencies are linked to **`/library/<library>/<version>/`**. In the other direction, each matching library version page lists the Showcases and Extended Tests that use that coordinate.

As a fallback for future projects without `showcase.json`, the server recognizes simple literal `adecc_package_root(...)` or `adecc_package_current_root(...)` declarations in CMake. Those mappings are shown as **partial evidence**, not a complete dependency graph. Add a manifest for an exact and comprehensive relationship.

## Routes and presentation

| Route | Purpose |
| --- | --- |
| `/showcases/` | Complete grouped showcase and extended-test directory |
| `/showcase/<project-path>/` | Detailed project view with versioned library links |
| `/showcase/<project-path>/Description.md` | Test operation, technical purpose and demonstration |
| `/showcase/<project-path>/build.md` | Reproducible configure, build, CTest, install and run commands |
| `/showcase/<project-path>/SBOM.md` | Declared software inventory |
| `/showcase/<project-path>/LICENSE.md` | Project licensing information |

The separate BuildEngine-Tests documentation server remains available and independent. Both sites render documentation from the same tests workspace. The central server serves only a fixed set of recognized Markdown file names, and only for previously discovered standalone projects. It never executes CMake, projects or test programs via HTTP.

## Validation

The three graphics showcases and the standalone documentation-server test have their own versioned manifests in BuildEngine-Tests. The 120 native library FSM integrations are user-confirmed complete; **[not verified]** the newly added graphical demonstrations and central HTTP rendering require Windows host execution after rebuilding BuildEngine-Common and BuildEngine-Server.
