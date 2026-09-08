# BuildEngine-Admin TODO

**Stand:** 8. September 2026  
**Status:** eingefrorene Nachfolgearbeit bis zum abschließenden Clean-Room-Test

## Verifizierte Freeze-Basis

Der aktuelle unveränderte Zielmaschinen-Folgelauf lieferte:

```text
[SUMMARY] jobs=471, current=451, passed=20, failed=0, blocked=0, incomplete=0

Machine state summary
---------------------
jobs=471, success=471, failed=0, blocked=0, incomplete=0
```

Damit ist für den derzeitigen Admin-Vertrag bestätigt:

- 451/451 Library-Tasks werden korrekt als `CURRENT` erkannt,
- kein Library-Task wird im unveränderten Folgelauf unnötig neu ausgeführt,
- 0 failed,
- 0 blocked,
- 0 incomplete.

Funktionale Baselines vor den reinen Dokumentationsänderungen:

```text
BuildEngine       268504010b54245124005fde968400f57b6514b5
BuildEngine-Admin f7c6183cf7dc4d2b56bbc7da8b5a963eb911e97f
```

Der aktuelle Vertrag verwendet:

```text
schemaVersion = 14
22 Bibliotheks-/Plattformverträge
```

Siehe `docs/FREEZE_CLEANROOM.md`.

## Freeze-Regel

Bis zum Abschluss des vollständigen Clean-Room-Tests werden keine funktionalen Änderungen am Admin-Vertrag vorgenommen.

Vor dem Clean-Room-Test nicht zulässig:

- Änderungen an `admin/build-libraries.xml`,
- Änderungen an `admin/build-tools.xml`,
- XSD-/Schemaänderungen,
- neue oder geänderte Patches,
- neue oder geänderte CMake-/Toolchain-Adapter,
- Änderungen an `admin/programs/`,
- neue oder geänderte Package-Smokes,
- neue Bibliotheken,
- geänderte Library-Timestamps,
- geänderte Source-Pins/Hashes,
- geänderte Build-/Test-/Install-/Publishparameter.

Erlaubt bleiben ausschließlich Dokumentations-, Evidence- und Handoff-Korrekturen.

Findet der Clean-Room-Test einen Fehler, wird nur die minimal notwendige funktionale Korrektur vorgenommen. Danach wird ein neuer Freeze-Basispunkt festgelegt und der vollständige Clean-Room-Test erneut durchgeführt.

## Noch vor Aufhebung des Freeze

- [x] 451/451-CURRENT-Folgelauf dokumentieren
- [x] Schema-/Library-Stand dokumentieren
- [x] Freeze-Regeln dokumentieren
- [x] Nachfolge-TODOs einfrieren
- [ ] vollständigen Clean-Room-Test auf frischer Zielumgebung durchführen
- [ ] ersten vollständigen Lauf inklusive Commits, Toolversionen, Patches und Machine-State protokollieren
- [ ] unmittelbar folgenden unveränderten zweiten `--make`-Lauf protokollieren
- [ ] bestätigen, dass alle Library-Tasks im zweiten Lauf `CURRENT` sind
- [ ] Clean-Room-Evidence anschließend in BuildEngine und BuildEngine-Admin aufnehmen

## Eingefrorene Bibliotheksarbeit nach dem Clean-Room-Test

### OpenCL

- [ ] historische BCC64X-Evidence als Ausgangspunkt verwenden,
- [ ] Header/Loader und Vendor-Runtime sauber trennen,
- [ ] exakten Source-Pin und Lizenz deklarieren,
- [ ] minimalen BCC64X Compile-/Link-Vertrag herstellen,
- [ ] Runtime-Gate so gestalten, dass fehlende Vendor-Plattform nicht fälschlich den Buildvertrag widerlegt.

### GoogleTest

- [ ] exakten historisch bewiesenen Upstream-/Versionsstand rekonstruieren,
- [ ] Shared-vs-Static bewusst neu bewerten,
- [ ] Static ausdrücklich zulassen, wenn dies für Testinfrastruktur technisch sinnvoller ist,
- [ ] kleinen Test-Consumer definieren.

### Evidenzfeld danach bewerten

Nach OpenCL und GoogleTest erfolgt keine automatische Aufnahme weiterer Bibliotheken. Zuerst wird bewertet, ob zusätzliche Pakete noch neuen Erkenntniswert zu BCC64X, ABI, RTL, Buildsystemintegration oder Toolchain-Kompatibilität liefern.

## Eingefrorene BuildEngine-/Admin-Gemeinschaftsthemen nach dem Clean-Room-Test

### Publish-/Consumer-Ownership

- [ ] Ownership im gemeinsamen `Win64x`-Consumer-Baum modellieren,
- [ ] Publish-Manifeste prüfen,
- [ ] fremde Paketdateien niemals als eigene Altdateien löschen,
- [ ] unnötige Publish-Wiederholungen vermeiden.

### Scheduler-/DAG-Nutzung

- [ ] neue explizite Action-DAGs erst nach erfolgreichem Clean-Room-Test und nach Einführung sinnvoller Scheduler-Messbarkeit einsetzen,
- [ ] keine Library-Verträge vorsorglich parallelisieren,
- [ ] keine typabhängigen Worker-Pools einführen,
- [ ] künftige Parallelität über generische Ressourcen (`cpuBudget`, `{JobSlots}`) steuern.

### Security

- [ ] Repository-Identitäten dort ergänzen, wo automatische Ableitung nicht belastbar genug ist,
- [ ] vollständigen Schema-14-Security-Monitoring-Lauf nach dem Freeze dokumentieren.

### Dokumentation

- [ ] Library-/Lizenz-/SBOM-Dokumentation nach dem Clean-Room-Lauf mit realer Evidence aktualisieren,
- [ ] neue offene Punkte aus dem Clean-Room-Test nur als TODO aufnehmen, sofern sie nicht für den PASS zwingend korrigiert werden müssen.

## Aktuell enthaltene Library-Verträge

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

Diese Liste ist für den Freeze funktional unverändert zu lassen.

## Eingefrorene Projektregeln

1. BCC64X bleibt tatsächliche Zieltoolchain.
2. Kein stiller Ersatz durch MSVC, clang-cl oder MinGW.
3. Upstream-Buildsysteme werden bevorzugt erhalten.
4. `admin/build-libraries.xml` bleibt einziger normativer Bibliotheks-/Dependency-Vertrag.
5. Generische Mechanik gehört in BuildEngine-C++; Bibliothekswissen in XML/Admin.
6. Patches sind versionsgebunden und reproduzierbar.
7. Tests werden nicht ohne Analyse deaktiviert.
8. Release und Debug bleiben getrennte Varianten, soweit für die Bibliothek sinnvoll.
9. Shared DLL + Import-Library ist Standard für normale Runtime-Bibliotheken, aber keine dogmatische Regel für Testinfrastruktur.
10. Kleine Package-Smokes und komplexe `BuildEngine-Tests` bleiben getrennt.
11. Primärdokumentation ist Deutsch.
12. Source-Bäume sind regenerierbare Artefakte; halb extrahierte Quellen dürfen nicht als gültiger Source-Baum sichtbar werden.
13. Alle technischen Actions werden schedulerseitig gleich behandelt; keine typabhängigen Scheduler-Kategorien.
14. Schema 14 bleibt während des Freeze unverändert.
15. Library-Timestamps bleiben während des Freeze unverändert.

## Clean-Room-Abschlusskriterien

Der Freeze wird erst aufgehoben, wenn:

```text
vollständiger erster Clean-Room-Lauf: PASS
unveränderter zweiter Lauf: PASS
alle Library-Tasks im zweiten Lauf: CURRENT
failed=0
blocked=0
incomplete=0
```

Zusätzlich müssen die verwendeten Repository-Commits, Compiler-/Toolversionen, Source-Pins und angewendeten Patches nachvollziehbar protokolliert sein.
