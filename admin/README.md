# BuildEngine-Admin – technische Administrationsdaten

**Stand:** 8. September 2026  
**Status:** eingefrorener Vertragsstand vor abschließendem Clean-Room-Test

Dieser Ordner enthält die Administrationsverträge, die BuildEngine zur Laufzeit für Toolbereitstellung, Bibliotheksbau, Paketierung, Publish und kleine Consumer-Smokes synchronisiert und verwendet.

Die normative Bibliotheksdefinition liegt ausschließlich in:

```text
build-libraries.xml
```

Aktive Bibliotheken werden nicht auf XML-Fragmente verteilt.

## Aktueller Vertragsstand

```text
build-libraries.xml : Schema 14
Library-Verträge    : 22
```

Enthalten:

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

Weitere zentrale Inhalte:

```text
build-tools.xml    verwaltete Toolbereitstellung
smoke-tests.xml    Übergangs-/Kompatibilitätsdatei
schemas/           XSD-Verträge
cmake/             generische und bibliotheksbezogene CMake-/Toolchain-Adapter
patches/           versionsgebundene Source-Patches
programs/          technisch begründete Spezialbrücken
smokes/            kleine paketbezogene Consumer-Smokes
```

## Verifizierter Freeze-Stand

Der letzte unveränderte Zielmaschinen-Folgelauf mit diesem funktionalen Admin-Vertrag lieferte:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0
Machine state: jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

Damit wurden 451/451 Library-Tasks als `CURRENT` bestätigt.

Funktionale Baselines vor den reinen Dokumentationsänderungen:

```text
BuildEngine       268504010b54245124005fde968400f57b6514b5
BuildEngine-Admin f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
```

Ausführliche Freeze-Regeln stehen in:

```text
../docs/FREEZE_CLEANROOM.md
../TODO.md
```

## Grundprinzip

BuildEngine hält Bibliothekswissen aus dem C++-Kern heraus. XML beschreibt Bibliotheken; C++ implementiert generische Mechanismen.

Bevorzugter Ablauf:

```text
Upstream / Source-Pin
-> Download und Hash-/Identitätsprüfung
-> Extraktion
-> ggf. versionsgebundener Patch
-> originales Buildsystem
-> BCC64X Build
-> Tests / Validation soweit sinnvoll
-> Install
-> Require-Gates
-> Publish
-> Package-Smoke
-> Ready
```

## Generische technische Actions

Aktuell verwendete generische Action-Typen umfassen unter anderem:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 14 erlaubt zusätzlich optionale Graph-Metadaten:

```text
id
dependsOn
```

Semantik:

- fehlt `dependsOn`, bleibt die historische serielle Vorgängerbeziehung bestehen,
- explizit leeres `dependsOn` bedeutet keine lokale Vorgängerabhängigkeit,
- explizite Dependencies referenzieren derzeit vorher definierte technische Actions mit stabiler ID.

Bis nach dem Clean-Room-Test werden keine neuen Parallelisierungsstrukturen in den Library-Verträgen eingeführt.

## Erweiterte Copy-Semantik

`<copy>` unterstützt neben einfachen Datei-/Verzeichnis-Kopien auch generische Paketierungsoperationen:

- `recursive`,
- `overwrite`,
- Include-/Exclude-Patterns,
- `flatten`,
- `cleanTarget`,
- `singleFile`.

Diese generische Funktion ersetzt die frühere eigene ACE/TAO-Python-Paketierung.

## `<require>` und Pfadtypen

Eigenständige `<require>`-Actions unterstützen:

```xml
<require path="..."/>
<require path="..." kind="file"/>
<require path="..." kind="directory"/>
<require path="..." kind="any"/>
```

`file` bleibt der kompatible Default. `directory` verlangt ein echtes Verzeichnis; `any` akzeptiert jeden existierenden Filesystem-Eintrag.

Das verschachtelte `<extract><require path="..."/></extract>` bleibt ein Dateinachweis des extrahierten Upstream-Artefakts.

## Upstream, Sources und Patches

Repositoryverwaltete Kompatibilitätspatches liegen versionsbezogen unter:

```text
patches/<library>/<version>/
```

Der Ablauf bleibt:

```text
Upstream herunterladen
-> Identität prüfen
-> vollständig extrahieren
-> Patch gegen genau diesen Stand prüfen
-> Patch anwenden
-> originales Buildsystem ausführen
```

Source-Bäume sind regenerierbare Artefakte. Ein temporärer Extract-Baum darf nie als gültige Source sichtbar werden.

## Native Archive-Verarbeitung

Historische eigene generische Python-Extraktion für `.tar.xz` ist nicht mehr Teil der aktiven Architektur. BuildEngine nutzt die intern verfügbare libarchive-/xz-Funktionalität.

## Technisch notwendige Spezialprogramme

`programs/opengl/meson_bootstrap.py` bleibt bewusst erhalten. Es bildet konkrete Mesa/Meson/BCC64X-Kompatibilitätsanforderungen ab und ist keine allgemeine Paketierungslogik.

Der frühere ACE/TAO-Python-Installer ist nicht mehr Teil des aktiven Vertrags.

## Paket- und Publish-Vertrag

Versionierte Producer-Pakete liegen unter:

```text
install/packages/<id>/<version>/
```

Publish bildet daraus den gemeinsamen Consumer-Baum:

```text
install/Win64x
```

Der versionierte Producer-Baum bleibt autoritativ. Eine vollständige Ownership-/Manifest-Trennung des gemeinsamen Consumer-Baums ist Nachfolgearbeit nach dem Clean-Room-Test.

## Kleine Consumer-Smokes

`smokes/<library>/...` enthält kleine Package-Acceptance-Tests. Ziel ist nur der Beweis des veröffentlichten Consumer-Vertrags:

```text
configure
-> compile
-> link
-> kleiner Runtime-Pfad
-> PASS
```

Komplexe Mehrprozess-, Integrations- und Demo-Szenarien gehören in `BuildEngine-Tests`.

Der ACE/TAO-Smoke ist bereits als kleiner IDL-/ORB-/Naming-Pfad umgesetzt.

## Produktform

Für normale Runtime-Bibliotheken ist Shared DLL + Import-Library der bevorzugte Standard, soweit technisch sinnvoll.

Static ist als dokumentierte Ausnahme zulässig. GoogleTest wird erst nach dem Freeze bewusst hinsichtlich Static-vs-Shared bewertet.

## Incremental State

Der Library-`timestamp` beschreibt den Änderungsstand des wirksamen Vertrags. Während des Freeze werden Library-Timestamps nicht verändert.

Die fortlaufende Incremental-State-Autorität liegt im BuildEngine-Kern bei technischen Step-Markern. Der verifizierte Folgelauf mit 451/451 `CURRENT` bestätigt den aktuellen Vertrag.

## Freeze-Regel

Bis zum Abschluss des vollständigen Clean-Room-Tests sind in diesem `admin/`-Baum keine funktionalen Änderungen zulässig.

Insbesondere nicht ändern:

- `build-libraries.xml`,
- `build-tools.xml`,
- XSDs,
- Patches,
- CMake-/Toolchain-Adapter,
- Programme,
- Smokes,
- Source-Pins,
- Hashes,
- Library-Timestamps,
- Build-/Test-/Install-/Publishparameter.

Findet der Clean-Room-Test einen funktionalen Fehler, wird nur die minimal notwendige Korrektur vorgenommen; anschließend muss der vollständige Clean-Room-Test mit einer neuen dokumentierten Freeze-Basis wiederholt werden.

## Nach dem Clean-Room-Test

Erst danach wieder aufnehmen:

- OpenCL,
- GoogleTest und Static-vs-Shared-Entscheidung,
- weitere Security-Identitäten,
- Publish-Ownership,
- neue explizite DAG-Parallelisierung,
- weitere Bibliotheken nur bei zusätzlichem Erkenntniswert.
