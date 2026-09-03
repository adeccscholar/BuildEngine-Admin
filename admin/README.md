# BuildEngine-Admin – technische Administrationsdaten

Dieser Ordner enthält die synchronisierten Administrationsverträge, die BuildEngine zur Laufzeit für Toolbereitstellung, Bibliotheksbau, Paketierung, Publish und kleine Consumer-Smokes verwendet.

Die normative Bibliotheksdefinition liegt ausschließlich in `build-libraries.xml`. Aktive Bibliotheken werden nicht auf XML-Fragmente verteilt.

## Aktueller Vertragsstand

- `build-libraries.xml`: Schema 13
- `build-tools.xml`: verwaltete Toolbereitstellung
- `smoke-tests.xml`: nur noch Übergangs-/Kompatibilitätsdatei
- `schemas/`: XSD-Verträge
- `cmake/`: generische und bibliotheksbezogene CMake-/Toolchain-Adapter
- `patches/`: versionsgebundene, reproduzierbare Source-Patches
- `programs/`: nur noch technisch begründete Hilfsprogramme
- `smokes/`: kleine paketbezogene Consumer-Smokes

Der aktuelle vollständige Bibliotheksvertrag enthält 19 Einträge: pugixml, zlib, brotli, zstd, xz, libzip, libarchive, openssl, curl, boost, nlohmann-json, ace-tao, bzip2, glew, opengl, raylib, sdl2, sqlite und xerces-c.

## Generische technische Aktionen

BuildEngine hält Bibliothekswissen aus dem C++-Kern heraus. XML beschreibt Bibliotheken, C++ implementiert generische Mechanismen.

Wesentliche aktuelle Aktionen sind:

```text
download
extract
copy
cmake
execute
require
```

Hinzu kommen Publish-, Metadata-/SBOM- und Smoke-Orchestrierung auf Library-Ebene.

### Erweiterte Copy-Semantik

`<copy>` kann neben dem einfachen Datei-/Verzeichnis-Kopieren auch gefilterte Paketierungsaufgaben ausdrücken:

```xml
<copy source="..." target="..."
      recursive="true"
      overwrite="true"
      flatten="true"
      cleanTarget="true"
      singleFile="false">
   <include pattern="**/*.dll"/>
   <include pattern="**/*.pdb"/>
   <exclude pattern="**/tests/**"/>
</copy>
```

Bedeutung:

- `recursive`: rekursive Quellsuche bzw. rekursives Kopieren,
- `overwrite`: bestehende Zieldateien dürfen ersetzt werden,
- `flatten`: Verzeichnisstruktur wird nicht übernommen; ausgewählte Dateien landen direkt im Ziel,
- `cleanTarget`: Zielbaum wird vor dem Kopieren bereinigt,
- `singleFile`: exakt eine passende Datei muss gefunden werden; sie kann dabei auf den exakten Zielnamen kopiert werden,
- `<include>` / `<exclude>`: glob-artige Filter; `**` darf Verzeichnisgrenzen überqueren.

Diese generische Funktion hat den früheren eigenen ACE/TAO-Python-Installationspfad ersetzt. Der reale Zielmaschinenlauf vom 3. September 2026 endete nach dieser Umstellung mit 295/295 PASS.

### `<require>` und Pfadtypen

Eigenständige `<require>`-Aktionen unterstützen drei Pfadtypen:

```xml
<require path="..."/>
<require path="..." kind="file"/>
<require path="..." kind="directory"/>
<require path="..." kind="any"/>
```

`file` ist der kompatible Default. `directory` verlangt ein echtes Verzeichnis; `any` akzeptiert jeden existierenden Filesystem-Eintrag.

Wichtig: Das verschachtelte `<extract><require path="..."/></extract>` ist davon getrennt. Es bleibt ein Dateinachweis innerhalb des vollständig extrahierten Upstream-Archivs und dient der Source-/Archivvalidierung.

## Upstream und Patches

Repositoryverwaltete Kompatibilitätspatches liegen unter:

```text
patches/<library>/<version>/
```

Sie sind keine Source-Ersatzpakete. Der normale Ablauf ist:

```text
Upstream herunterladen
-> Identität prüfen
-> vollständig extrahieren
-> Patch gegen genau diesen Stand prüfen
-> Patch anwenden
-> originales Buildsystem ausführen
```

Ein Patch muss versionsgebunden und reproduzierbar sein. Allgemeine Toolchainprobleme sollen nicht als zufällige bibliotheksspezifische Patches verteilt werden.

## Native Archive-Verarbeitung

Historische eigene Python-Extraktion für `.tar.xz` ist nicht mehr Teil der aktiven Architektur. BuildEngine verarbeitet relevante Archive über die intern verlinkte libarchive-/xz-Funktionalität.

## Technisch notwendige Spezialprogramme

Nicht jedes Hilfsprogramm ist automatisch unerwünschte Orchestrierung. `programs/opengl/meson_bootstrap.py` bleibt bewusst erhalten: Es bildet konkrete Meson/BCC64X-Kompatibilitätsanforderungen für den erfolgreichen Mesa/OpenGL-Build ab und ist keine generische Paketierungslogik.

`programs/ace-tao/install.py` ist dagegen nach dem erfolgreichen 295/295-Lauf obsolet und soll als nächster Cleanup-Schritt entfernt werden.

## Paket- und Publish-Vertrag

Versionierte Producer-Pakete liegen unter:

```text
install/packages/<id>/<version>/
```

Der `<publish>`-Knoten bildet die ausgewählte Consumer-Konfiguration in den gemeinsamen Baum ab:

```text
install/Win64x
```

Downstream-Consumer sollen nicht wissen müssen, in welchem versionsbezogenen Producer-Verzeichnis eine Bibliothek gebaut wurde.

Release und Debug bleiben getrennte Producer-Varianten. Publish verwendet die im Bibliotheksvertrag festgelegte Konfiguration; bei Paketen, die einen gemeinsamen Header-/SDK-Baum aus mehreren Varianten berühren, kann der Vertrag auf alle Varianten warten.

## Kleine Consumer-Smokes

Die Verzeichnisse unter `smokes/<library>/...` enthalten kleine Package-Acceptance-Tests. Ein Smoke soll den Bibliotheksvertrag beweisen, nicht die spätere Integrations- und Demonstrationswelt duplizieren.

Typischer Umfang:

```text
frisches Consumer-Projekt
-> find_package / Include-Pfade
-> compile
-> link
-> kleine Runtime-Funktion
-> PASS/FAIL-Protokoll
```

Komplexe Integrationstests und Demonstrationen gehören in das getrennte Repository `BuildEngine-Tests`.

### ACE/TAO-Smoke

Der geplante ACE/TAO-Smoke bleibt bewusst kompakt. Sinnvoll sind:

- eine kleine `.idl`-Datei,
- Aufruf des paketierten `tao_idl`,
- Kompilieren/Linken des erzeugten Codes gegen den veröffentlichten ACE/TAO-SDK,
- `ORB_init`,
- optional kurzer Start des paketierten Naming Service,
- einen Namen registrieren und wieder auflösen bzw. dessen Erreichbarkeit nachweisen,
- Naming Service sauber beenden.

Eine größere CORBA-Mehrprozess-/Service-Testwelt gehört ausdrücklich nach `BuildEngine-Tests`.

## Incremental State

Ein Bibliotheks-`timestamp` beschreibt den Änderungsstand des vollständigen Vertrags. Erfolgreiche Phasen schreiben maschinenlokalen Zustand. Ein unveränderter Vertrag mit passenden Artefakten soll nicht unnötig neu ausgeführt werden; `--rebuild` erzwingt dagegen die erneute Ausführung.

Abhängigkeitstimestamps invalidieren abhängige Producerzustände. Tests, Konfigurationsauswahl und weitere relevante Vertragsparameter werden dort berücksichtigt, wo sie die Gültigkeit eines Ergebnisses tatsächlich beeinflussen.

## Metadata, Lizenz und SBOM

BuildEngine erzeugt aus den im Admin-Vertrag deklarativ hinterlegten Upstream-/Lizenzinformationen Paketmetadaten und CycloneDX-SBOM-Evidence. Der erfolgreiche ACE/TAO-Lauf vom 3. September 2026 erzeugte beispielsweise `LICENSE-INFO.txt` und eine CycloneDX-1.7-SBOM mit sechs Komponenten.

## Nächster Evidenzblock

Nach Abschluss des aktuellen Cleanup-/Smoke-Schritts werden historische, bereits positiv erprobte BCC64X-Kandidaten in den heutigen BuildEngine-Vertrag überführt:

1. SOIL2,
2. OpenCL,
3. VTK,
4. GoogleTest.

GoogleTest wird hinsichtlich Shared-vs-Static bewusst separat bewertet. Da GoogleTest Testinfrastruktur und keine typische Runtime-Abhängigkeit einer Anwendung ist, kann eine statische Bibliothek als dokumentierte Ausnahme akzeptabel sein, wenn dies technisch und upstream-seitig sinnvoll ist.
