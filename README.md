# BuildEngine-Admin

Dieses Repository ist die **deklarative Administrationsschicht** des C++Builder-Third-Party-Integrationsprojekts. Ziel des Gesamtprojekts ist der reproduzierbare Nachweis, in welchem Umfang aktuelle C- und C++-Bibliotheken mit **Embarcadero C++Builder 13 / BCC64X** gebaut, getestet, paketiert und von normalen Consumer-Projekten verwendet werden können.

Die normative Primärdokumentation des Projekts wird auf Deutsch geführt.

## Rolle im Gesamtprojekt

Die drei Repositories haben bewusst unterschiedliche Aufgaben:

```text
adeccscholar/BuildEngine
   private C++23-Anwendung
   Scheduler, generische technische Aktionen, Repository-Sync,
   Incremental State, Publish und Smoke-Orchestrierung

adeccscholar/BuildEngine-Admin        <-- dieses Repository
   deklarative Tool- und Bibliotheksverträge
   XSD-Schemata, CMake-/Toolchain-Adapter, versionsgebundene Patches,
   kleine paketbezogene Consumer-Smokes

adeccscholar/BuildEngine-Tests
   komplexere Integrations-, Demonstrations- und Lernwelt
   bewusst getrennt von den kleinen Package-Acceptance-Smokes
```

Diese Trennung ist verbindlich. Ein kleiner Bibliotheks-Smoke soll lediglich belegen, dass ein veröffentlichtes Paket als Consumer tatsächlich benutzbar ist. Mehrprozess-Szenarien, umfangreiche Demonstrationen und fachlich größere Integrationstests gehören in `BuildEngine-Tests`.

## Autoritativer Bibliotheksvertrag

`admin/build-libraries.xml` ist der **einzige normative Bibliotheks- und Dependency-Vertrag**. Aktive Bibliotheksdefinitionen werden nicht in XML-Fragmente aufgeteilt.

Der aktuelle Vertrag verwendet `schemaVersion="13"` und enthält 19 verwaltete Bibliotheks-/Plattformverträge:

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
```

Bibliotheksspezifisches Wissen gehört nach Möglichkeit in diesen XML-Vertrag. Der BuildEngine-Kern implementiert nur **generische** Mechanismen.

## Grundprinzip

Der bevorzugte Pfad lautet:

```text
offizieller Upstream
   -> reproduzierbarer Download / Source-Pin
   -> Extraktion
   -> ggf. expliziter versionsgebundener Patch
   -> originales Buildsystem
   -> BCC64X Build
   -> Upstream-Tests soweit sinnvoll und technisch möglich
   -> versioniertes Paket
   -> explizite Require-Gates
   -> Publish in den Consumer-Baum
   -> kleiner Consumer-Smoke
```

Ein alternativer Compiler oder ein verstecktes Ersatz-Buildsystem darf BCC64X nicht stillschweigend ersetzen. Workarounds mit anderen Toolchains können untersucht und dokumentiert werden, sind aber keine automatische Projektentscheidung.

## Aktuelle BuildEngine-Aktionen

Die XML-Verträge nutzen unter anderem generische Aktionen für:

- Download und Hashprüfung,
- Archive-Extraktion,
- CMake-Aufrufe,
- allgemeine Prozessausführung,
- Kopieren von Dateien und Verzeichnissen,
- gefiltertes rekursives Kopieren mit Include-/Exclude-Patterns,
- Flattening von Verzeichnisbäumen,
- Bereinigung eines Zielbaums vor dem Kopieren,
- Auswahl genau einer passenden Datei,
- Datei-/Verzeichnis-/Pfadprüfung über `<require>`,
- Publish und Consumer-Smokes.

Die erweiterte Copy-Aktion erlaubt insbesondere, bisherige eigene Python-Paketierungslogik durch deklarative XML-Aktionen plus generische C++-Implementierung zu ersetzen.

## `<require>`: Datei, Verzeichnis oder beliebiger Pfad

Ein eigenständiger `<require>`-Knoten prüft standardmäßig weiterhin eine reguläre Datei:

```xml
<require path="{PackageRoot}\include\library.h"/>
```

Das ist kompatibel zu allen bisherigen Verträgen und entspricht implizit:

```xml
<require path="{PackageRoot}\include\library.h" kind="file"/>
```

Zusätzlich sind möglich:

```xml
<require path="{PackageRoot}\include" kind="directory"/>
<require path="{PackageRoot}\generated-object" kind="any"/>
```

`kind="directory"` verlangt tatsächlich ein Verzeichnis. `kind="any"` verlangt lediglich einen existierenden Filesystem-Eintrag.

Das innerhalb von `<extract>` verwendete `<require path="..."/>` bleibt bewusst ein **Dateinachweis innerhalb des extrahierten Upstream-Archivs**. Diese Semantik ist vom eigenständigen Install-/Dokumentations-`<require>` getrennt.

## Paketbezogene Smokes und BuildEngine-Tests

Kleine Smokes liegen unter `admin/smokes/<library>/...` und werden durch `<smoke>`-Knoten in `build-libraries.xml` an den jeweiligen Bibliotheksvertrag gebunden. Sie sollen typischerweise nur:

1. ein frisches Consumer-Projekt konfigurieren,
2. Header finden,
3. gegen die veröffentlichten Import-/statischen Bibliotheken linken,
4. bei sinnvoller Runtime einen kleinen Funktionspfad ausführen.

Für ACE/TAO soll dieser Rahmen ausdrücklich klein bleiben. Ein geeigneter Smoke darf beispielsweise eine kleine IDL mit dem paketierten `tao_idl` übersetzen, einen ORB initialisieren und optional den paketierten Naming Service kurz starten, einen Namen registrieren/auflösen und wieder beenden. Eine umfassende CORBA-Testwelt gehört dagegen in `BuildEngine-Tests`.

`admin/smoke-tests.xml` ist nur noch eine Übergangs-/Kompatibilitätsdatei. Komplexe Integrations- und Demo-Szenarien werden nicht zurück in diesen Legacy-Vertrag verschoben.

## Repository-Struktur

```text
BuildEngine-Admin/
|-- README.md
|-- TODO.md
|-- BOOST_1_92_BCC64X_RUNTIME_STATUS.md
|-- docs/
|   `-- library-license-sbom.md
`-- admin/
    |-- README.md
    |-- build-tools.xml
    |-- build-libraries.xml
    |-- smoke-tests.xml
    |-- schemas/
    |-- cmake/
    |-- patches/
    |-- programs/
    `-- smokes/
```

Unter `admin/programs/` dürfen nur technisch begründete Hilfsprogramme verbleiben. Generische Orchestrierung gehört in BuildEngine-C++. Ein konkretes Beispiel für eine weiterhin notwendige Spezialbrücke ist `admin/programs/opengl/meson_bootstrap.py`, das die reale Meson/BCC64X-Kompatibilität für den Mesa-Build herstellt. Dagegen ist `admin/programs/ace-tao/install.py` nach erfolgreicher Ablösung durch den generischen Copy-Vertrag nur noch ein zu entfernender Altbestand.

## Synchronisation

BuildEngine hält einen Git-Worktree des Admin-Repositories unter dem konfigurierten Repository-Root und synchronisiert dessen `admin/`-Baum in den produktiven Arbeitsbereich. Der Git-Checkout ist die autoritative Quelle; der synchronisierte Arbeitsbaum ist kein eigener Git-Checkout.

Repositoryverwaltete Dateien werden nach Inhalt synchronisiert. Maschinenlokale Zustandsdateien werden nicht als normative Repositorydaten behandelt.

## Reproduzierbarkeit und Evidence

Ein belastbarer Lauf soll mindestens identifizierbar machen:

- BuildEngine-Commit,
- BuildEngine-Admin-Commit,
- gegebenenfalls BuildEngine-Tests-Commit,
- Bibliotheksversion und Source-Pin,
- Compiler- und Toolversionen,
- wirksame Buildparameter,
- angewendete Patches,
- erzeugte Paketartefakte,
- Publish-Ergebnis,
- Test-/Smoke-Ergebnis,
- Logs und maschinenlokalen Abschlusszustand.

Am 3. September 2026 wurde nach der Umstellung der ACE/TAO-Paketierung auf die generische C++-/XML-Copy-Logik ein vollständiger Zielmaschinenlauf mit **295 Jobs, 295 PASS, 0 FAIL, 0 BLOCKED** abgeschlossen. ACE/TAO publizierte dabei 4086 Dateien in den gemeinsamen `Win64x`-Consumer-Baum. Dieser Stand ist die aktuelle funktionale Basis für die nächsten Arbeiten.

## Nächste Bibliotheken zur Abrundung des Evidenzfelds

Als nächste größere Aufnahmegruppe sind vorgesehen:

- SOIL2,
- OpenCL,
- VTK,
- GoogleTest.

Für diese Bibliotheken existiert bereits historische BCC64X-Evidenz aus dem früheren Evidenz-Test. Die Aufgabe ist deshalb nicht, ihre grundsätzliche Machbarkeit neu zu erfinden, sondern die damaligen Erkenntnisse in den aktuellen reproduzierbaren BuildEngine-Vertrag zu überführen.

GoogleTest besitzt eine andere Rolle als typische Runtime-Bibliotheken: Es ist Testinfrastruktur. Die bisherige statische Bereitstellung wird deshalb erneut geprüft. Wenn Upstream-Struktur, technische Zweckmäßigkeit oder die Vermeidung unnötiger Test-Runtime-DLL-Abhängigkeiten den statischen Vertrag sinnvoll machen, kann diese Ausnahme ausdrücklich akzeptiert und dokumentiert werden.

## Lizenz

Projekt-eigene Inhalte dieses öffentlichen Admin-Repositories stehen unter der MIT-Lizenz, soweit in einzelnen Dateien nichts Abweichendes angegeben ist. Drittanbieterquellen und deren Lizenztexte behalten selbstverständlich ihre jeweiligen Upstream-Lizenzen.
