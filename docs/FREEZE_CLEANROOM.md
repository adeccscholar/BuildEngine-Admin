# BuildEngine-Admin – eingefrorener Vertragsstand vor abschließendem Clean-Room-Test

**Stand:** 8. September 2026  
**Status:** CONTRACT FREEZE / Clean-Room-Kandidat  
**Primärsprache:** Deutsch

## 1. Zweck

Dieses Dokument friert den aktuellen deklarativen Admin-Vertrag vor einem abschließenden vollständigen Clean-Room-Test ein. Der Admin-Stand soll bis zum Abschluss dieses Tests funktional unverändert bleiben.

Dokumentiert werden:

- die Rolle des Admin-Repositories,
- der verifizierte gemeinsame BuildEngine-/Admin-Stand,
- der aktuelle Schema-/Bibliotheksvertrag,
- die verbindlichen Regeln für Toolchain, Patches, Tests und Produktformen,
- die Freeze-Grenzen,
- die nach dem Clean-Room-Test fortzusetzenden Aufgaben,
- die Evidence-Anforderungen des Clean-Room-Abschlusses.

## 2. Verifizierte gemeinsame Basis

Der letzte unveränderte Zielmaschinen-Folgelauf der BuildEngine mit diesem funktionalen Admin-Vertrag lieferte:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0

Machine state summary
---------------------
jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

Damit ist verifiziert:

- 451/451 Library-Tasks wurden als `CURRENT` erkannt,
- kein Library-Task wurde im unveränderten Folgelauf unnötig neu ausgeführt,
- keine Fehler, Blockierungen oder unvollständigen Jobs blieben zurück.

Funktionaler BuildEngine-Stand:

```text
268504010b54245124005fde968400f57b6514b5
```

Funktionaler Admin-Stand vor diesem reinen Dokumentationsblock:

```text
f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
Allow graph metadata on technical actions
```

Die nachfolgenden Commits des Freeze-Blocks dürfen ausschließlich Dokumentation ändern.

## 3. Rolle des Admin-Repositories

`BuildEngine-Admin` ist die deklarative Administrationsschicht des Projekts. Der BuildEngine-Kern implementiert generische Mechanismen; Bibliothekswissen wird soweit möglich im Admin-Vertrag gehalten.

Autoritative Rollen:

```text
BuildEngine
   C++23-Kern, Scheduler, Actions, State, Repository-Sync, Publish-Orchestrierung

BuildEngine-Admin
   Tool-/Library-Verträge, XSD, Patches, CMake-/Buildsystem-Adapter,
   Security-Metadaten, kleine Package-Smokes

BuildEngine-Tests
   komplexere Integrations-, Demo- und Lernwelt
```

## 4. Normativer Bibliotheksvertrag

Der einzige normative Bibliotheks- und Dependency-Vertrag ist:

```text
admin/build-libraries.xml
```

Aktueller Vertragsstand:

```text
schemaVersion = 14
22 Bibliotheks-/Plattformverträge
```

Aktuell enthalten:

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

OpenCL und GoogleTest sind bewusst noch nicht Teil des eingefrorenen Vertrags.

## 5. Technische Action-Semantik

Der Admin-Vertrag verwendet generische Actions, unter anderem:

```text
download
extract
copy
cmake
execute
require
target
```

Schema 14 erlaubt optionale technische Graph-Metadaten wie `id` und `dependsOn`. Diese Metadaten sind generisch; sie begründen keine bibliotheksspezifische Schedulerlogik.

Backwards-Kompatibilität:

- ohne `dependsOn` bleibt die historische serielle Action-Reihenfolge,
- ein explizit leeres `dependsOn` kann eine unabhängige Action deklarieren,
- explizite Dependencies referenzieren derzeit vorher definierte Actions mit stabiler ID.

Bis nach dem Clean-Room-Test werden keine neuen DAG-Strukturen in den Library-Verträgen eingeführt.

## 6. BCC64X-Vertrag

Das Projekt dient ausdrücklich der Evidence für C++Builder 13 / BCC64X.

Verbindlich:

1. BCC64X bleibt reale Zieltoolchain.
2. Kein stiller Ersatz durch MSVC, clang-cl oder MinGW.
3. Upstream-Buildsysteme werden bevorzugt erhalten.
4. Alternative Toolchains sind höchstens getrennte Workaround-/Vergleichspfade.
5. Ein Buildproblem wird analysiert und dokumentiert, nicht durch Compilerwechsel verdeckt.

## 7. Source- und Patchvertrag

Der Standardpfad lautet:

```text
Upstream herunterladen
-> Source-Pin / Hash / Identität prüfen
-> vollständig extrahieren
-> versionsgebundenen Patch prüfen
-> Patch anwenden
-> originales Buildsystem ausführen
```

Patches liegen versionsbezogen unter `admin/patches/` und müssen reproduzierbar sein.

Source-Bäume sind regenerierbare Artefakte. Bei ungültigem Source-State wird der alte Source-Baum ersetzt. Temporäre Extract-Bäume dürfen nicht als gültige Quelle sichtbar werden.

## 8. Produktform

Für normale Runtime-Bibliotheken bleibt Shared DLL + Import-Library der bevorzugte Standard, soweit Upstream und Bibliothekszweck dies sinnvoll unterstützen.

Static ist eine zulässige, zu dokumentierende Ausnahme. Für GoogleTest wird diese Entscheidung erst nach Aufhebung des Freeze getroffen.

## 9. Tests und Smokes

Upstream-Tests werden nicht ohne technische Analyse deaktiviert.

Package-Smokes unter `admin/smokes/` bleiben klein und prüfen den tatsächlichen Consumer-Vertrag:

```text
configure
-> compile
-> link
-> kleiner Runtime-Pfad
-> PASS
```

Komplexe Mehrprozess-, Integrations-, Demo- und Lernfälle gehören in `BuildEngine-Tests`.

Der ACE/TAO-Smoke enthält bereits den kleinen realistischen IDL-/ORB-/Naming-Pfad und ist kein Ersatz für eine umfassende CORBA-Testwelt.

## 10. OpenGL/Mesa-Spezialbrücke

`admin/programs/opengl/meson_bootstrap.py` bleibt eine ausdrücklich zulässige technische Kompatibilitätsbrücke für Mesa/Meson/BCC64X.

Sie ist keine generische Orchestrierungslogik und wird während des Freeze nicht verändert.

## 11. Incremental-State-Bezug

Der Incremental State wird vom BuildEngine-Kern verwaltet. Für den Admin-Vertrag ist entscheidend:

- der Library-`timestamp` repräsentiert den Änderungsstand des Vertrags,
- Timestamps dürfen während des Freeze nicht geändert werden,
- reine Dokumentationsänderungen außerhalb des Laufzeitvertrags dürfen keine Library-Neubauten auslösen,
- der aktuelle unveränderte Folgelauf hat 451/451 Library-Tasks als `CURRENT` bestätigt.

## 12. Freeze-Regel

Bis zum Clean-Room-Abschluss sind im Admin-Repository nur Änderungen ohne Laufzeitwirkung erlaubt.

Erlaubt:

- Dokumentation,
- Evidence-Protokolle,
- TODO-/Handoff-Klarstellungen.

Nicht erlaubt:

- Änderungen an `admin/build-libraries.xml`,
- Änderungen an `admin/build-tools.xml`,
- XSD-/Schemaänderungen,
- neue oder geänderte Patches,
- neue oder geänderte CMake-/Toolchain-Adapter,
- Änderungen an `admin/programs/`,
- neue oder geänderte Smokes,
- neue Bibliotheken,
- geänderte Library-Timestamps,
- geänderte Source-Pins/Hashes,
- geänderte Build-/Test-/Install-/Publishparameter.

Wenn der Clean-Room-Test einen Fehler findet, ist nur die minimal notwendige Korrektur zulässig. Danach wird die neue funktionale Admin-Basis dokumentiert und der vollständige Clean-Room-Test wiederholt.

## 13. Eingefrorene Nachfolgearbeit

Erst nach erfolgreichem Clean-Room-Test wieder aufnehmen:

1. OpenCL aus historischer BCC64X-Evidence in den aktuellen Vertrag überführen.
2. GoogleTest aus historischer Evidence in den aktuellen Vertrag überführen.
3. GoogleTest Static-vs-Shared bewusst entscheiden und dokumentieren.
4. Security-Repository-Identitäten dort ergänzen, wo automatische Ableitung nicht belastbar genug ist.
5. Publish-/Consumer-Ownership gemeinsam mit dem BuildEngine-Kern weiterentwickeln.
6. neue DAG-Parallelisierung erst nach Messung und Scheduler-Ressourcenmodell nutzen.
7. weitere Bibliotheken nur aufnehmen, wenn sie zusätzlichen Erkenntniswert für BCC64X, ABI, RTL oder Buildsystemintegration liefern.

## 14. Clean-Room-Test – Anforderungen an den Admin-Nachweis

Der Clean-Room-Abschluss soll mindestens dokumentieren:

```text
BuildEngine commit
BuildEngine-Admin commit
BuildEngine-Tests commit, soweit verwendet
Schema-Version
Liste/Anzahl der Library-Verträge
Toolversionen
Source-Pins und angewendete Patches
SUMMARY des vollständigen ersten Laufs
Machine state summary des ersten Laufs
SUMMARY des unveränderten zweiten Laufs
Machine state summary des zweiten Laufs
```

Der zweite Lauf muss den unveränderten Library-Vertrag vollständig wiederverwenden und darf keine ungerechtfertigten Source-/Build-/Test-/Install-/Publish-/Smoke-Wiederholungen zeigen.

## 15. Aufhebung des Freeze

Der Contract Freeze wird erst aufgehoben, wenn:

1. der vollständige Clean-Room-Lauf erfolgreich ist,
2. alle Library-Gates erfolgreich sind,
3. 0 `failed`, 0 `blocked`, 0 `incomplete` vorliegen,
4. ein unmittelbar folgender unveränderter Lauf die Library-Tasks als `CURRENT` bestätigt,
5. die vollständige Evidence in BuildEngine und BuildEngine-Admin dokumentiert wurde.
