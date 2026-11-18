# Showcases and Extended Tests

## Central workspace view

The BuildEngine central server exposes **[Showcases & Extended Tests](/showcases/)** alongside Libraries, Tools, Packages and News. Projects are discovered from `<production-root>/tests`; for the active Windows workspace this is `D:\local\embarcadero\test_v3\tests`. No second checkout or GitHub API access is required by the server.

The catalog is grouped by source directory. A project directory is recognized when it contains `Description.md` and either an independent `CMakeLists.txt` or a C++Builder `.cbproj`. Directories `bootstrap`, `cmake`, `cache`, `docs`, `docu-server`, build output and Git metadata are excluded from enumeration. Nested groups such as `database/mssql` remain visible.

## Versioned relationship model

Each project may contain a `showcase.json` manifest with the following format:

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

For existing projects without `showcase.json`, the server recognizes simple literal `adecc_package_root(...)` or `adecc_package_current_root(...)` declarations in CMake. Those mappings are shown as **partial evidence**, not a complete dependency graph. Add a manifest for an exact and comprehensive relationship.

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

The three graphics showcases have their own versioned manifests in BuildEngine-Tests. The 120 native library FSM integrations are user-confirmed complete; **[not verified]** the newly added graphical demonstrations and central HTTP rendering require Windows host execution after rebuilding BuildEngine-Common and BuildEngine-Server.
