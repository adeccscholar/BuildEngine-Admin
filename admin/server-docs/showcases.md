# Showcases and Extended Tests

## Central workspace view

The BuildEngine central server exposes **[Showcases & Extended Tests](/showcases/)** alongside Libraries, Tools, Packages and News. Projects are discovered from `<production-root>/tests`; for the active Windows workspace this is `D:\local\embarcadero\test_v3\tests`. No second checkout or GitHub API access is required by the server.

The catalog is grouped by source directory. A project directory is recognized when it contains `Description.md` and either an independent `CMakeLists.txt` or a C++Builder `.cbproj`. Directories `bootstrap`, `cmake`, `cache`, `docs`, build output and Git metadata are excluded from enumeration. The `docu-server` project is included as an independent test application. Nested groups such as `database/mssql` remain visible.

## Complete test metadata

The previously documented **26 standalone BuildEngine-Tests applications** declare a `showcase.json` manifest. Group-only `database/` is not an executable. The `docu-server/` documentation application is included as a standalone extended test. Every manifest provides a concise functional description and a full list of managed package roots referenced by the test, distinguishing `direct: true` for directly consumed APIs from `direct: false` for supporting runtime closure packages.

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

## Independent executables and operator controls

A standalone test directory may contain several separately runnable programs. The manifest's optional `applications[]` identifies each executable by `name`, concise `summary`, exact one-line `command`, and `interactive` status. The central showcase detail page renders **Individual applications and viewers**, distinguishing GUI applications from headless numerical tests and exposing a separate copyable CMD launch command for each.

The Mandelbrot group now distinguishes `openmp_mandelbrot_benchmark.exe` from `openmp_mandelbrot_viewer.exe`. Its viewer documentation explicitly covers Mandelbrot/Julia switching, scheduling, zoom, pan, iteration and OpenMP thread controls. Similarly, the Galaxy group separates benchmark from OpenGL viewer, and Geospatial lists its benchmark, 2D Skia viewer and 3D terrain viewer. Other interactive examples including Earth, Skia, Physics Arena, PDFium SDL2 and VTK likewise describe their own controls and entry points.

The standalone `Description.md` remains the full technical and operational explanation, and `build.md` keeps one complete CMD command per copyable code block. Applications are **metadata for locally built binaries**, not remotely executed commands; the central server does not start executables over HTTP.

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

## Native interactive graphics laboratories

The three graphics examples are now native SDL2 applications rather than static SVG-first showcases:

- **Dear ImGui Neon Laboratory:** Four animated procedural scenes, native ImGui sliders/buttons and a real SDL2 renderer that uploads the dynamic ImGui 1.93 font atlas. Text is drawn as textured glyphs rather than exported SVG triangle outlines.
- **ImPlot Real-Time Signal Theater:** Four continuously updated signal models with native ImPlot graphs, axes, legend, signal visibility controls, frequency/amplitude/speed sliders and playback controls.
- **Matplot++ Mathematical Discovery Gallery:** Six selectable native thumbnail scenes, progressive curve animation, zoom and playback controls. Matplot++ provides numeric sample sets; ImGui and SDL2 present them interactively. Optional SVG export remains a separate route for browser or central-server presentation.

Their standalone per-project `build.md` guides use copyable one-line CMake commands and independent Release/Debug build directories. The project manifests and declared CycloneDX inventories now include the respective SDL2 and ImGui runtime dependencies. **[not verified]** The revised GUI execution, frame rate, font quality and rendering output require Windows BCC64X testing before any show-readiness claim.

## New native discovery laboratories (20 November 2026)

Four projects are registered in the existing BuildEngine-Tests tree and are discovered by the **same** `/showcases/` route. They are not browser emulations or static SVG demonstrations. Each project contains an independent CMake target, `showcase.json`, a native interactive executable, a separate `--verify` CTest, `Description.md`, `build.md`, `LICENSE.md` and `SBOM.md`.

| Showcase | Core native algorithm | Interactive experience | Project route |
| --- | --- | --- | --- |
| **Vector Galaxy — The Shape of Similarity** | `hnswlib@0.9.0` HNSW search over 851 3D feature vectors; `libigl@2.6.4` and `eigen@5.0.1` compute actual triangle-mesh normals | Orbiting luminous starfield, highlighted live nearest-neighbour results, query position, neighbour count and zoom | [Open Vector Galaxy](/showcase/graphics/vector-galaxy/) |
| **Barcode Signal Lab — Decode the Invisible** | `zxing-cpp@3.1.1` reader DLL decodes a grayscale EAN-13 barcode independently generated from the published parity/checksum rules | Native scan chamber, adjustable barcode data, scan action, quality mode and live decoded output | [Open Barcode Signal Lab](/showcase/graphics/barcode-signal-lab/) |
| **Navigation Reactor — Terrain Intelligence** | `recastnavigation@1.6.0` classifies triangle walkability and rasterizes a real 3D terrain into `rcHeightfield` spans | Turquoise walkable terrain versus magenta blocked slopes, live agent slope and terrain relief controls | [Open Navigation Reactor](/showcase/graphics/navigation-reactor/) |
| **Pulse Engine — The Shape of Sound** | `miniaudio@0.11.26` synthesizes actual PCM samples using native `ma_waveform` and optional `ma_device` playback | Four oscillator shapes, glowing oscilloscope, pitch/gain sliders and sound only after the operator presses Play | [Open Pulse Engine](/showcase/graphics/pulse-engine/) |

These demos use the already managed **SDL2 + Dear ImGui 1.93** native rendering adapter. Native graphics, input and backend DLLs remain explicitly declared in the manifests. No alternative compiler, prebuilt third-party packages or online rendering service is permitted.

### Verification boundary and terminology

The BuildEngine library FSM and the BuildEngine-Tests showcase verification statuses are different evidence domains. Successful source, build, publish and library consumer tests **do not** prove that a newly written interactive showcase compiles or renders. All four new manifests carry `status: "not-verified"` pending actual native CTest and GUI execution. Their detail pages must preserve that status and display exact active version links resolved against the catalog.

The newly registered Jolt Physics 5.7.0 DLL is intentionally **not** presented as a completed showcase: its last operator-provided test ended in a process failure at runtime. A Jolt-dependent scene would need matching ABI feature definitions, passing native runtime tests and stable per-configuration imports before it could honestly be advertised.

The server serves project metadata and Markdown documentation only; launching an executable remains an operator action on the local Windows machine.

## Verified bidirectional library relationships for the new applications

Every new project carries an explicit `dependencies[]` array in `showcase.json`. Both rendering backends must resolve **`active` to exactly one non-superseded catalog version** and must produce real versioned library routes, never `/library/<id>/active/`.

| Showcase | Direct managed library relationships |
| --- | --- |
| [Vector Galaxy](/showcase/graphics/vector-galaxy/) | [hnswlib](/library/hnswlib/0.9.0/), [libigl](/library/libigl/2.6.4/), Eigen, Dear ImGui, SDL2 |
| [Barcode Signal Lab](/showcase/graphics/barcode-signal-lab/) | [ZXing-C++](/library/zxing-cpp/3.1.1/), Dear ImGui, SDL2 |
| [Navigation Reactor](/showcase/graphics/navigation-reactor/) | [Recast Navigation](/library/recastnavigation/1.6.0/), Dear ImGui, SDL2 |
| [Pulse Engine](/showcase/graphics/pulse-engine/) | [miniaudio](/library/miniaudio/0.11.26/), Dear ImGui, SDL2 |
| [Neon Physics Arena](/showcase/neon-physics-arena/) including the differential solver | [Jolt Physics](/library/jolt-physics/5.7.0/), Box2D, SDL3 |

In the showcase detail page, each direct dependency links to its concrete library version. In the reverse direction, that exact library-version page lists all matching showcase projects. The file-backed renderer resolves the manifest against the loaded XML catalog; the PostgreSQL renderer resolves both forward and reverse references via `PostgresCatalogReader::ActiveVersion`. An unresolved or ambiguous active coordinate must remain explicitly unresolved and must not point to an invented version. Persisted showcase cache rows must be refreshed after new manifests are synchronized.

## Validation

The established graphics showcases, four newly registered discovery laboratories, and the standalone documentation-server test have their own versioned manifests in BuildEngine-Tests. The 120 native library FSM integrations are user-confirmed complete; **[not verified]** the newly added graphical demonstrations and central HTTP rendering require Windows host execution after rebuilding BuildEngine-Common and BuildEngine-Server.

## Cross-engine physics laboratory: Jolt Physics versus Box2D

The existing **[Neon Physics Arena](/showcase/neon-physics-arena/)** now declares a third native executable, `neon-physics-duel.exe`, in its `showcase.json` and an additional CTest named `NeonPhysicsDifferential`. This test is intentionally deeper than the single-library consumer smokes: it independently creates and advances Jolt Physics 5.7 (3D) and Box2D 3.2 (2D) worlds, with equivalent sphere-on-floor scenarios at 120 Hz for 720 steps.

Both engines are executed twice from independent initial worlds. Assertions check repeatability **within each engine**, correct gravity and floor behaviour, and finite, bounded trajectories. The program outputs six sampled heights per solver and elapsed runtime metrics. It does not require their numerical trajectories or timing results to match each other: different solvers and dimensions have different contact models. `DUEL|CHECK|...`, `DUEL|METRIC|...` and `DUEL|RESULT|...` are visible in CTest output.

The Jolt DLL ABI is checked before any physics registration, and all third-party dependencies resolve from the active BuildEngine package catalog. The operator has confirmed the 126-library FSM completed successfully; nevertheless, **[not verified]** the newly authored differential application must separately pass native CMake/CTest, DLL-closure inspection and the test's own assertions. The Showcase status must never be marked verified by copying the library FSM status.
