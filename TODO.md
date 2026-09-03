# BuildEngine-Admin TODO

Stand: 2026-09-03

## Aktuelle belastbare Basis

Der aktuelle Zielmaschinenlauf nach Umstellung der ACE/TAO-Paketierung auf die generische C++-/XML-Copy-Logik endete mit:

```text
jobs=295
success=295
failed=0
blocked=0
incomplete=0
```

ACE/TAO wurde dabei erfolgreich gebaut, installiert, publiziert und als Consumer-Vertrag bereitgestellt. Publish übertrug 4086 Dateien in den gemeinsamen `install\Win64x`-Baum.

Der aktuelle Bibliotheksvertrag enthält:

- pugixml 1.16
- zlib 1.3.2
- Brotli
- Zstd
- XZ
- libzip
- libarchive 3.8.9
- OpenSSL 3.5.8
- curl
- Boost 1.92.0
- nlohmann-json 3.12.0
- ACE 8.0.6 / TAO 4.0.6
- bzip2
- GLEW
- OpenGL/Mesa
- raylib
- SDL2
- SQLite
- Xerces-C 3.3.0

Damit sind die früheren Roadmap-Punkte SQLite, SDL2, OpenGL, GLEW, raylib und bzip2 **kein zukünftiger Neuaufbau mehr**.

## Unmittelbarer Abschluss des aktuellen Blocks

1. [ ] `admin/programs/ace-tao/install.py` entfernen. Der reale 295/295-Lauf hat bestätigt, dass die generische C++-/XML-Copy-Implementierung den bisherigen eigenen Python-Installer ersetzt.
2. [ ] Einen kleinen ACE/TAO-Consumer-Smoke ergänzen.
3. [ ] Die neue `<require kind="directory">`-Funktion in einem realen Bibliotheksvertrag verwenden und damit auf der Zielmaschine verifizieren.
4. [ ] Einen unveränderten Folgelauf durchführen und prüfen, welche Phasen korrekt als aktuell erkannt werden.
5. [ ] Publish-/Consumer-Ownership und unnötige Wiederholungen anhand dieses Folgelaufs bewerten.

## ACE/TAO-Smoke – bewusst begrenzter Umfang

Der Smoke darf die spätere Test-/Demowelt in `BuildEngine-Tests` nicht vorwegnehmen.

Akzeptierter kleiner Umfang:

- kleine `.idl`-Datei,
- paketiertes `tao_idl` ausführen,
- erzeugten Stub-/Skeleton-Code mit BCC64X kompilieren/linken,
- `ORB_init`,
- optional paketierten Naming Service starten,
- einen Namen registrieren und wieder auflösen oder die erwartete Naming-Reaktion prüfen,
- Prozess sauber beenden.

Nicht in diesen Smoke gehören umfangreiche Mehrprozess-, Ausfall-, SSLIOP-, Event-Service- oder Anwendungsszenarien. Diese werden bewusst in `BuildEngine-Tests` aufgebaut.

## Nächster Evidenz-Abschlussblock

Nach dem obigen Cleanup werden vier Bibliotheken/Plattformbausteine aufgenommen, die im früheren Evidenz-Test bereits als mit BCC64X grundsätzlich machbar belegt wurden. Ziel ist die Überführung dieser Evidenz in den heutigen reproduzierbaren BuildEngine-Vertrag.

### 1. SOIL2

- [ ] exakten Upstream/Version/Source-Pin aus der vorhandenen Evidenz rekonstruieren,
- [ ] Lizenz prüfen und deklarieren,
- [ ] vorhandenen BCC64X-Buildpfad auf den heutigen Toolchain-Vertrag übertragen,
- [ ] Release/Debug soweit upstream-seitig sinnvoll,
- [ ] Produktform Shared/Static anhand Upstream und realem Einsatzzweck festlegen,
- [ ] kleinen Image-Load-/Texture-relevanten Consumer-Smoke definieren, ohne daraus bereits eine Grafikdemo zu machen.

### 2. OpenCL

- [ ] klar trennen zwischen OpenCL-Headers/Loader und einem konkreten Vendor-Runtime-Treiber,
- [ ] die im Evidenz-Test verwendete Implementierung bzw. Header-/Loader-Kombination exakt dokumentieren,
- [ ] Source-Pin und Lizenz festlegen,
- [ ] BCC64X Compile-/Link-Vertrag herstellen,
- [ ] Smoke so gestalten, dass fehlende GPU-/Vendor-Runtime nicht fälschlich den Buildvertrag widerlegt,
- [ ] wenn eine verfügbare Plattform existiert: minimale Platform-/Device-Abfrage als Runtime-Gate.

### 3. VTK

- [ ] vorhandene erfolgreiche Evidenz als Ausgangspunkt verwenden, nicht neu portieren,
- [ ] damals verwendete VTK-Version, Modulmenge und Abhängigkeiten rekonstruieren,
- [ ] zunächst exakt den bewiesenen minimalen Modulumfang in den BuildEngine-Vertrag übernehmen,
- [ ] Release/Debug getrennt,
- [ ] Upstream-CMake beibehalten,
- [ ] Consumer-Smoke klein halten: Datenmodell/Algorithmus bzw. minimale Rendering-Initialisierung abhängig vom bewiesenen Profil,
- [ ] größere VTK-Demos getrennt in `BuildEngine-Tests`.

### 4. GoogleTest

- [ ] exakten bewiesenen Upstream-/Versionsstand ermitteln,
- [ ] aktuelle Upstream-Empfehlung für Build/Verwendung prüfen,
- [ ] Shared-vs-Static bewusst neu bewerten,
- [ ] keine automatische Anwendung der allgemeinen „Shared DLL + Import-Lib“-Regel erzwingen, wenn sie für Testinfrastruktur technisch unpassend ist,
- [ ] statische Bereitstellung ausdrücklich als zulässige Ausnahme dokumentieren, wenn sie die sinnvollere Form ist,
- [ ] einfacher Test-Consumer mit mindestens einem erfolgreichen und intern erwarteten Assertion-Pfad.

### GoogleTest – Entscheidungsregel zur statischen Bibliothek

Die allgemeine BuildEngine-Produktpolitik bevorzugt Shared Libraries, wenn eine Bibliothek als normale Runtime-Komponente einer Anwendung gedacht ist und Upstream dies sinnvoll unterstützt.

GoogleTest hat eine andere Rolle:

```text
Anwendung / Produktionsbibliothek -> normale Runtime-Abhängigkeit
GoogleTest                         -> Test-/Build-Infrastruktur
```

Deshalb ist eine statische GoogleTest-Bibliothek kein unerwünschter Sonderweg, wenn:

- Upstream diese Form bevorzugt oder gleichwertig unterstützt,
- dadurch keine relevante Funktion verloren geht,
- Testprogramme dadurch einfacher und reproduzierbarer werden,
- keine künstliche Runtime-DLL-Abhängigkeit nur zur Einhaltung einer allgemeinen Produktregel erzeugt wird.

Die Entscheidung wird nach Prüfung explizit im Bibliotheksvertrag und in der Dokumentation festgehalten.

## Danach: Feld bewerten und schließen

Nach SOIL2, OpenCL, VTK und GoogleTest soll zunächst **kein automatischer Endlos-Ausbau** weiterer Third-Party-Bibliotheken erfolgen. Stattdessen wird der erreichte Evidenzraum bewertet:

- Welche relevanten Buildsysteme wurden abgedeckt?
- Welche C- und C++-Bibliothekstypen wurden abgedeckt?
- Welche Grenzen sind echte BCC64X-/RTL-/ABI-Grenzen?
- Welche Probleme waren nur Buildsystemintegration?
- Welche Patches sind Upstream-Kandidaten?
- Welche generischen BuildEngine-Fähigkeiten haben sich als dauerhaft nützlich erwiesen?

Danach wird entschieden, ob weitere Pakete einen zusätzlichen Erkenntniswert liefern.

## Komplexe Test- und Demonstrationswelt

Parallel zur kleinen Package-Acceptance-Schicht wird `BuildEngine-Tests` bewusst als eigenständige Welt weiterentwickelt.

Dort können später unter anderem entstehen:

- komplexere ACE/TAO-Client/Server-Szenarien,
- Naming-/Event-Service-Demos,
- SSL/TLS- und Zertifikatsszenarien,
- OpenGL/raylib/VTK-Grafikbeispiele,
- OpenCL-Rechenbeispiele,
- kombinierte Bibliotheks-Integrationen,
- Tutorial-/Lernprogramme.

Diese Tests dürfen umfangreicher sein und mehrere Pakete kombinieren. Sie sind aber **nicht** Voraussetzung dafür, dass ein einzelner Bibliotheksvertrag als erfolgreich gilt.

## Weiterhin offene BuildEngine-/Admin-Themen

### Publish-/Consumer-Ownership

- [ ] Eigentümertrennung im gemeinsamen `Win64x`-Consumer-Baum weiter untersuchen,
- [ ] eigene Manifeste für publizierte Dateien/Integrationsdateien prüfen,
- [ ] fremde Paketdateien niemals als vermeintliche Altdateien löschen,
- [ ] bei unverändertem Zustand unnötige erneute Publish-Arbeit vermeiden.

### Incremental-Verifikation

- [ ] unmittelbar nach dem 295/295-Lauf einen unveränderten zweiten Lauf analysieren,
- [ ] sicherstellen, dass Source/Build/Test/Install/Publish/Smoke nur bei tatsächlich ungültigem Zustand erneut laufen,
- [ ] Rebuild-Verhalten weiterhin klar von normalem Incremental-Lauf trennen.

### Generische Copy-Funktion

Der neue gefilterte C++-Copy-Pfad ist real durch ACE/TAO belegt. Später mögliche, aber derzeit nicht automatisch einzubauende Erweiterungen:

- [ ] optionales `required` pro Include-Pattern,
- [ ] geordnete Fallback-Auswahl statt nur `singleFile`, falls ein realer Bibliotheksfall dies benötigt,
- [ ] Performance/Logging anhand größerer Paketbäume auswerten.

### `<require>`

- [x] reguläre Datei als kompatibler Default,
- [x] `kind="directory"`,
- [x] `kind="any"`,
- [ ] reale `directory`-Verwendung im Bibliotheksvertrag als Zielmaschinen-Evidence,
- [ ] verschachtelte Extract-Requirements vorerst bewusst dateibezogen lassen.

## Eingefrorene Projektregeln

1. BCC64X bleibt tatsächliche Zieltoolchain.
2. Kein stiller Ersatz durch MSVC, clang-cl oder MinGW.
3. Upstream-Buildsysteme werden bevorzugt erhalten.
4. `admin/build-libraries.xml` bleibt einziger normativer Bibliotheks-/Dependency-Vertrag.
5. Generische Mechanik gehört in BuildEngine-C++, Bibliothekswissen ins XML/Admin.
6. Patches sind versionsgebunden und reproduzierbar.
7. Tests werden nicht ohne Analyse deaktiviert.
8. Release und Debug bleiben getrennte Varianten, soweit für die Bibliothek sinnvoll.
9. Shared ist Standard für normale Runtime-Bibliotheken, aber keine dogmatische Regel für Testinfrastruktur wie GoogleTest.
10. Kleine Package-Smokes und komplexe `BuildEngine-Tests` bleiben bewusst getrennt.
11. Primärdokumentation ist Deutsch.
