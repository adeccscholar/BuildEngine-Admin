# BuildEngine-Admin

Dieses Repository ist die **deklarative Administrationsschicht** des C++Builder-Third-Party-Integrationsprojekts. Ziel des Gesamtprojekts ist der reproduzierbare Nachweis, in welchem Umfang aktuelle C- und C++-Bibliotheken mit **Embarcadero C++Builder 13 / BCC64X** gebaut, getestet, paketiert und von normalen Consumer-Projekten verwendet werden können.

Die normative Primärdokumentation wird auf Deutsch geführt.

## Aktueller Status: Contract Freeze vor Clean-Room-Test

Der funktionale Admin-Vertrag ist vor einem abschließenden vollständigen Clean-Room-Test eingefroren.

Verifizierter unveränderter Folgelauf:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0
Machine state: jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

Damit sind **451/451 Library-Tasks als CURRENT** bestätigt.

Funktionale Baselines vor den reinen Dokumentationsänderungen:

```text
BuildEngine       268504010b54245124005fde968400f57b6514b5
BuildEngine-Admin f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
```

Verbindliches Freeze-Dokument:

```text
docs/FREEZE_CLEANROOM.md
```

Bis zum Clean-Room-Abschluss werden keine Laufzeitverträge, Patches, Tools, Smokes, Source-Pins oder Library-Timestamps geändert.

## Rolle im Gesamtprojekt

```text
adeccscholar/BuildEngine
   private C++23-Anwendung
   Scheduler, generische technische Aktionen, Repository-Sync,
   Incremental State, Publish und Smoke-Orchestrierung

adeccscholar/BuildEngine-Admin        <-- dieses Repository
   deklarative Tool- und Bibliotheksverträge
   XSD-Schemata, CMake-/Toolchain-Adapter, versionsgebundene Patches,
   Security-Metadaten und kleine paketbezogene Consumer-Smokes

adeccscholar/BuildEngine-Tests
   komplexere Integrations-, Demonstrations- und Lernwelt
```

Diese Trennung ist verbindlich.

## Autoritativer Bibliotheksvertrag

`admin/build-libraries.xml` ist der **einzige normative Bibliotheks- und Dependency-Vertrag**.

Aktueller Stand:

```text
schemaVersion = 14
22 Bibliotheks-/Plattformverträge
```

Enthalten sind:

```text
pugixml
zlib
brotli
zstd
xz
libzip
libarchive
openssl
curl
boost
nlohmann-json
ace-tao
bzip2
glew
opengl
raylib
sdl2
sqlite
xerces-c
soil2
vtk
opencv
```

OpenCL und GoogleTest bleiben bewusst Nachfolgearbeit nach dem Freeze.

## Grundprinzip

```text
offizieller Upstream
-> reproduzierbarer Download / Source-Pin
-> Extraktion
-> ggf. expliziter versionsgebundener Patch
-> originales Buildsystem
-> BCC64X Build
-> Upstream-Tests soweit sinnvoll
-> versioniertes Paket
-> explizite Require-Gates
-> Publish in den Consumer-Baum
-> kleiner Consumer-Smoke
```

Ein alternativer Compiler darf BCC64X nicht stillschweigend ersetzen.

## Generische technische Aktionen

Der aktuelle XML-Vertrag nutzt unter anderem:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 14 erlaubt optionale technische Graph-Metadaten (`id`, `dependsOn`). Ohne `dependsOn` bleibt die historische serielle Vorgängerbeziehung bestehen; neue Parallelisierung wird vor dem Clean-Room-Test nicht mehr in die Library-Verträge eingebracht.

## Copy und Require

`<copy>` unterstützt einfache und gefilterte Paketierungsoperationen, unter anderem recursive, overwrite, include/exclude, flatten, cleanTarget und singleFile.

Eigenständige `<require>`-Knoten unterstützen:

```xml
<require path="..."/>
<require path="..." kind="file"/>
<require path="..." kind="directory"/>
<require path="..." kind="any"/>
```

Das in `<extract>` verschachtelte `<require>` bleibt dateibezogene Source-Evidence.

## Paketbezogene Smokes

Kleine Smokes liegen unter `admin/smokes/<library>/...`. Sie sollen nur den veröffentlichten Consumer-Vertrag beweisen. Komplexe Mehrprozess- und Integrationsszenarien gehören in `BuildEngine-Tests`.

## Technisch notwendige Spezialprogramme

`admin/programs/opengl/meson_bootstrap.py` bleibt eine bewusst akzeptierte Mesa/Meson/BCC64X-Kompatibilitätsbrücke. Generische Orchestrierung gehört dagegen in den BuildEngine-Kern.

## Reproduzierbarkeit und Evidence

Ein belastbarer Clean-Room-Nachweis soll mindestens identifizieren:

- BuildEngine-Commit,
- BuildEngine-Admin-Commit,
- BuildEngine-Tests-Commit soweit verwendet,
- Schema-Version,
- Bibliotheksversion und Source-Pin,
- Compiler- und Toolversionen,
- wirksame Buildparameter,
- angewendete Patches,
- Paket-/Publish-Ergebnisse,
- Test-/Smoke-Ergebnisse,
- Machine-State-Zusammenfassung,
- unveränderten zweiten Lauf mit vollständigem CURRENT-Nachweis.

## Freeze-Regel

Bis zum Clean-Room-Abschluss sind ausschließlich Dokumentations- und Evidence-Änderungen erlaubt. Findet der Clean-Room-Test einen funktionalen Fehler, wird nur die minimal notwendige Korrektur durchgeführt; danach beginnt die vollständige Freeze-Verifikation erneut.

## Wichtige Dokumente

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

## Lizenz

Projekt-eigene Inhalte dieses öffentlichen Admin-Repositories stehen unter der MIT-Lizenz, soweit in einzelnen Dateien nichts Abweichendes angegeben ist. Drittanbieterquellen und deren Lizenztexte behalten ihre jeweiligen Upstream-Lizenzen.
