# BuildEngine-Admin

Dieses Repository ist die **deklarative Administrationsschicht** des C++Builder-Third-Party-Integrationsprojekts. Ziel des Gesamtprojekts ist der reproduzierbare Nachweis, in welchem Umfang aktuelle C- und C++-Bibliotheken mit **Embarcadero C++Builder 13 / BCC64X** gebaut, getestet, paketiert und von normalen Consumer-Projekten verwendet werden können.

Die normative Primärdokumentation wird auf Deutsch geführt.

## Aktueller Status

Der frühere Freeze vor dem Clean-Room-Test wurde am 8. September 2026 im BuildEngine-Kern gezielt für zwei Abschlussfunktionen wieder geöffnet:

1. zentrale Doxygen-Dokumentation auf Basis der tatsächlich publizierten Dateien,
2. Task-basierte Performance-Evidence (`buildlog_*.log`).

Der **Admin-Vertrag selbst bleibt funktional unverändert**. Es wurden dafür keine Library-Timestamps, Source-Pins, Patches, Buildparameter, Smokes oder Schemas geändert.

Letzte verifizierte Runtime-Evidence vor diesem Core-Funktionsblock:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0
Machine state: jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

Diese Evidence ist die Basis des vorherigen Funktionsstands und muss nach der neuen Doxygen-/Performance-Erweiterung erneut bestätigt werden.

## Rolle im Gesamtprojekt

```text
adeccscholar/BuildEngine
   private C++23-Anwendung
   Scheduler, generische technische Aktionen, Repository-Sync,
   Incremental State, Publish, Dokumentation und Smoke-Orchestrierung

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

OpenCL und GoogleTest bleiben Nachfolgearbeit nach dem Clean-Room-Abschluss.

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
-> zentrale Doxygen-Dokumentation, wenn aktiviert
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

Schema 14 erlaubt optionale technische Graph-Metadaten (`id`, `dependsOn`). Ohne `dependsOn` bleibt die historische serielle Vorgängerbeziehung bestehen.

## Publish-Manifest und Doxygen

Die Publish-Manifeste haben neben ihrer Ownership-/Incremental-Rolle jetzt eine weitere generische Verwendung im BuildEngine-Kern: Bei `WithDoxygen=true` bilden sie die **autoritative Dateiliste für die öffentliche API-Dokumentation**.

Für eine publizierte Bibliothek liest BuildEngine:

```text
<PublishRoot>\.buildengine\manifests\<library>.manifest
```

und übergibt daraus die dokumentierbaren tatsächlich veröffentlichten Header-, IDL- und C++-Moduldateien an Doxygen. Dadurch wird nicht versehentlich der gesamte gemeinsame Consumer-Baum einer Library zugerechnet.

Die zentrale Ausgabe liegt im Production Root:

```text
documentation\index.html
documentation\<library>\<version>\html\index.html
```

`admin/build-tools.xml` enthält bereits den verwalteten Doxygen-Vertrag (aktuell 1.18.0); für diese Erweiterung war keine Tool-/Schemaänderung erforderlich.

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
- zentrale Doxygen-Ausgabe,
- `buildlog_*.log` als Performance-Evidence,
- Machine-State-Zusammenfassung,
- unveränderten zweiten Lauf mit vollständigem CURRENT-Nachweis.

## Nächster Freeze

Vor dem vollständigen Clean-Room-Test wird der neue Core-Stand lokal mit BCC64X kompiliert und mit `WithDoxygen=true` verifiziert. Danach wird ein neuer gemeinsamer Freeze-Basispunkt dokumentiert. Bis dahin bleiben Admin-Library-Verträge funktional unverändert.

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
