# BuildEngine-Admin TODO

Stand: 2026-09-01

## Statusbasis

Der aktuelle BuildEngine-/BCC64X-Stand ist die neue Ausgangsbasis. Bereits erreichte und nicht neu zu erfindende Ergebnisse werden erhalten und nur gezielt erweitert.

### Bereits im aktuellen BuildEngine-Vertrag vorhanden

- pugixml 1.16
- zlib 1.3.2
- Brotli
- Zstd
- XZ
- libzip
- libarchive
- OpenSSL 3.5.8
- curl
- Boost 1.92.0
- nlohmann-json
- Xerces-C 3.3.0
- ACE 8.0.6 / TAO 4.0.6

### Aktuell erreichte BCC64X-Evidenz

- Xerces-C 3.3.0: Release und Debug gebaut, Upstream-CTest ausgeführt, installiert und publiziert; DOM-Consumer-Smoke verifiziert.
- ACE 8.0.6 / TAO 4.0.6: Release und Debug über upstream MPC/BMake mit `BCC64X=1` gebaut.
- ACE/TAO nutzt die verwalteten Abhängigkeiten OpenSSL 3.5.8 und Xerces-C 3.3.0.
- `ACE_XML_Utils` ist mit Xerces-C aktiviert und gebaut.
- Naming Service, COS Event Service und RT Event Service sind in Release und Debug gebaut und installiert.
- Der zuletzt vollständig durchgelaufene Gesamtplan endete mit 206/206 erfolgreichen Jobs, 0 Fehlern und 0 blockierten Jobs.
- Die geringe Parallelität innerhalb des ACE/TAO-BMake-Laufs wird vorerst nicht künstlich verändert. Die Auslastung wird erneut bewertet, sobald mehr unabhängige Bibliotheken im Bulk parallel laufen.

## Aktiver Arbeitsblock: ACE/TAO + zlib

Ziel ist die Aktivierung der bereits von ACE/TAO vorgesehenen zlib-Integration, nicht das Erfinden einer künstlichen Abhängigkeit.

1. zlib 1.3.2 ist bereits ein verwaltetes BuildEngine-Paket.
2. ACE/TAO erhält eine echte Dependency auf zlib 1.3.2.
3. Im MPC-Featurevertrag wird `zlib=1` aktiviert.
4. `ZLIB_ROOT`, `ZLIB_INCDIR` und `ZLIB_LIBDIR` werden auf das verwaltete zlib-Paket gesetzt.
5. Upstream MPC erwartet unter Windows traditionell `zlib.lib` bzw. `zlibd.lib`. Unser BCC64X-zlib-Vertrag liefert dagegen `libz.lib` bzw. `libzd.lib`.
6. Der Patch `admin/patches/ace-tao/8.0.6/bcc64x-zlib-library-names.patch` macht den MPC-zlib-Librarynamen über die Templatevariable `zliblib` überschreibbar.
7. Die ACE/TAO-MPC-Aufrufe setzen je Variante `zliblib=libz` bzw. `zliblib=libzd`.
8. Erwarteter neuer TAO-Baustein ist insbesondere `TAO_ZlibCompressor` aus `TAO/tao/Compression/zlib`.
9. Nächster Gate: realer Zielmaschinenlauf für Release und Debug. Erst danach die tatsächlich erzeugten BCC64X-Artefaktnamen als `<require>`-Gates festschreiben; keine Namen vorab raten.
10. Nach PASS: Install-/Publish-Vertrag und Consumer-/Runtime-Smoke für die neue TAO-zlib-Funktion ergänzen, soweit sinnvoll.

## Große Bibliotheks-Roadmap

Die Reihenfolge ist eine Arbeitspriorität, keine Aussage über zwingende technische Abhängigkeiten. Dependencies werden nur eingetragen, wenn sie technisch real bestehen.

### A. Nächste, voraussichtlich überschaubare Pakete

#### 1. SQLite

- Eigenständigen BCC64X-Vertrag anlegen.
- Aktuelle freigegebene SQLite-Version vor Aufnahme festlegen und SourcePin/SHA256 dokumentieren.
- Bevorzugt offiziellen Amalgamation-/Source-Stand verwenden.
- Shared DLL + Import-Library als Standardprodukt; statische Variante nur getrennt und eindeutig benannt, falls benötigt.
- Release und Debug.
- Upstream-/Selbsttests soweit mit dem gewählten Sourcepaket praktikabel.
- Consumer-Smoke: Datenbank erzeugen, Tabelle anlegen, schreiben, lesen, prepared statement und Transaktion.
- Threading-/Compile-Options explizit dokumentieren, damit das Paket reproduzierbar bleibt.

#### 2. SDL2

- Historische BCC64X-Erfahrungen in den aktuellen BuildEngine-Vertrag übernehmen, nicht neu portieren, solange vorhandene Evidenz reicht.
- Offizielle Version und SourcePin/SHA256 festlegen.
- Shared DLL + Import-Library, Release und Debug.
- Upstream-CMake möglichst unverändert verwenden.
- Consumer-Smoke mit Fenster/Eventloop; Headless-Teststrategie nur ergänzend, nicht als Ersatz für den realen Windows-Pfad.
- Frühere Vertragsauffälligkeiten wie fehlender expliziter DOWNLOAD-Schritt nicht übernehmen.

#### 3. OpenGL / Windows-Plattformvertrag

- OpenGL unter Windows nicht als künstliches Third-Party-Sourcepaket behandeln, wenn die eigentliche Plattformbibliothek aus dem Windows SDK kommt.
- BuildEngine-Vertrag für die reproduzierbare Erkennung und Nutzung von `opengl32`/Windows-SDK definieren.
- BCC64X Compile-/Link-Smoke für einen minimalen OpenGL-Kontext.
- Header-, Library- und Runtime-Herkunft dokumentieren.
- Dieser Vertrag bildet die Basis für weitere Grafikbibliotheken und Beispiele.

#### 4. GLEW

- Gemeint ist die OpenGL Extension Wrangler Library (GLEW); endgültige Version vor Vertragsanlage festlegen.
- Abhängigkeit auf den OpenGL-Plattformvertrag nur dort modellieren, wo sie für Build/Smoke real benötigt wird.
- Offiziellen SourcePin/SHA256 dokumentieren.
- Shared DLL + Import-Library, Release und Debug, sofern Upstream/BCC64X das Produkt sauber unterstützt.
- Minimaler Consumer-Smoke: OpenGL-Kontext erzeugen, GLEW initialisieren und eine moderne OpenGL-Funktion auflösen.
- Keine vorgefertigten MSVC-Binaries als Ersatz für einen BCC64X-Build verwenden.

#### 5. raylib

- Offizielle Version und SourcePin/SHA256 festlegen.
- BCC64X-Build aus Upstream-Quellen untersuchen und möglichst über Upstream-CMake integrieren.
- Vorher klären, welche Plattform-/Fenster-Backends raylib im gewählten Windows-Build tatsächlich selbst mitbringt und welche externen Abhängigkeiten real sind; keine künstliche GLEW-/SDL-Abhängigkeit anlegen.
- Shared DLL + Import-Library als bevorzugtes Produkt, Release und Debug, soweit Upstream das unterstützt.
- Consumer-Smoke: Fenster öffnen, einfache 2D-/3D-Szene rendern, sauber schließen.
- Danach optional ein kleines Lern-/Demo-Projekt für die C++Builder-Tutorialschiene bereitstellen, getrennt vom Acceptance-Gate.

#### 6. bzip2

- Eigenständigen verwalteten BCC64X-Vertrag erstellen.
- Danach prüfen, ob das ACE/TAO-Feature `bzip2` sinnvoll zusätzlich aktiviert werden soll.
- Release/Debug, Shared-/Import-Library-Vertrag und Roundtrip-Smoke.

#### 7. zziplib

- Nach stabilem zlib-Vertrag aufnehmen.
- SourcePin/SHA256, Release/Debug, Shared-/Import-Library.
- ZIP-Lese-Smoke und anschließend optional ACE/TAO-Feature `zzip` untersuchen.

#### 8. LZO

- Aktuelle geeignete LZO-Version festlegen und Lizenz-/SourcePin-Dokumentation aufnehmen.
- C-Build sollte grundsätzlich ein überschaubarer BCC64X-Kandidat sein, muss aber real verifiziert werden.
- Prüfen, welche der ACE-Features `lzo1` und `lzo2` im aktuellen ACE/TAO-Stand tatsächlich sinnvoll sind.
- Roundtrip-Kompressions-Smoke.

### B. Größere Grafik-/Visualisierungspakete

#### 9. Skia

- Vorhandene erfolgreiche C++Builder/BCC64X-Erfahrung und bereits erzeugte Bibliotheken als Evidenzbasis sammeln.
- Ziel bleibt ein echter BCC64X-Build bzw. eine belastbar dokumentierte vorhandene BCC64X-Bibliothek; kein stiller Wechsel auf MSVC als Projektentscheidung.
- Buildsystem, Third-Party-Abhängigkeiten und Debug/Release-Vertrag getrennt erfassen.
- Consumer-Smokes für Raster, Text, Pfade und optional GPU/OpenGL.
- VCL-Einbindung separat von der Kernbibliotheks-QA testen.

#### 10. VTK

- Erst nach Stabilisierung der kleineren Grafik-/Math-/Kompressionsbausteine wieder aufnehmen.
- Offizielle Version, SourcePin/SHA256 und vollständigen Abhängigkeits-DAG ermitteln.
- CMake/BCC64X-Kompatibilität schrittweise prüfen; Module zunächst minimal halten, danach gezielt erweitern.
- Keine alternative Toolchain als Ersatz für das BCC64X-Ziel verwenden.
- Release/Debug und kleiner Rendering-/Datenmodell-Consumer als Gate.

## Weitere ACE/TAO-Optionen

ACE/TAO kennt neben OpenSSL, Xerces-C und zlib weitere optionale Fremdbibliotheken/Features. Diese werden nicht pauschal aktiviert, sondern erst wenn das jeweilige Paket im BuildEngine sauber verwaltet wird und ein konkreter funktionaler Nutzen besteht.

Kandidaten:

- bzip2
- zziplib
- LZO1/LZO2
- Boost nur bei konkretem ACE/TAO-Nutzen; nicht allein deshalb, weil Boost bereits vorhanden ist
- GUI-/Toolkit-Features nur bedarfsgerecht, nicht als Vollständigkeitsübung

Für jede Erweiterung gilt: erst eigenständiges Paket verifizieren, danach echte ACE/TAO-Integration aktivieren, anschließend neuer Zielmaschinenlauf.

## Boost 1.92.0 – bekannte Einschränkung, nicht blockierend

Der Boost-Arbeitsblock ist für den aktuellen Third-Party-Fortschritt abgeschlossen.

Gesichert:

- Boost.Iostreams Plain Output/Flush funktioniert.
- Boost.Serialization Binary Archive funktioniert, auch mit Boost.Iostreams als Streamträger.
- Locale/codecvt- und `uncaught_exceptions()`-Hypothesen wurden isoliert geprüft und ausgeschlossen.
- Ein minimaler BCC64X-DLL-Reproducer zeigt die eigentliche Grenze:
  - DLL `std::ostream::put()` auf einem EXE-eigenen Stream: PASS
  - DLL `std::ostream::flush()`: PASS
  - DLL `operator<<('\n')`: reproduzierbare Access Violation `0xC0000005`
- Boost.Serialization Text Archives benutzen im Destruktionspfad denselben problematischen C++-Stream-Insertion-Pfad.

Entscheidung:

- Reproducer und Dokumentation bleiben erhalten.
- Der bekannte Crashpfad wird nicht mehr im normalen Acceptance-Gate ausgeführt.
- Das akzeptierte Boost-BCC64X-Profil umfasst die verifizierten Pfade und schließt Boost.Serialization Text Archives über diese DLL-/`std::ostream`-Grenze ausdrücklich aus.

Details: `BOOST_1_92_BCC64X_RUNTIME_STATUS.md`.

## ACE/TAO – eingefrorene Evidenz und Regeln

Bereits belegt und nicht neu zu erfinden:

- ACE 8.0.6 / TAO 4.0.6
- Release-Tag `ACE+TAO-8_0_6`
- Release-Archiv SHA256 `e741c8b0ec0c7d6747b184d674567b1e73a13bdf2c902485d1299bbf267b3fba`
- upstream MPC/BMake-Pfad
- BCC64X-Build mit `BCC64X=1`
- Buildreihenfolge ACE -> ace_gperf -> TAO_IDL -> TAO core
- build-lokales `tao_idl.exe` vor TAO-Core
- installierte `tao_idl.exe` und `ace_gperf.exe` unter `tools/bin`
- Win64-SSL/select-Korrekturen
- upstream SSLIOP-`params_dup.cpp`-Korrektur
- Xerces-C-/`ACE_XML_Utils`-Integration
- Naming Service, COS Event und RT Event als erreichte Infrastrukturziele
- OpenSSL 3.5.8 und Xerces-C 3.3.0 als verwaltete Dependencies

Historische Revisionsnamen dienen nur als Provenienz und werden nicht in aktive Pfadnamen übernommen.

## Einheitlicher Paket-Acceptance-Vertrag

Für neue Bibliotheken gelten grundsätzlich dieselben Qualitätsziele:

1. offizieller Upstream und eindeutig festgelegte Version,
2. SourcePin mit URL/Tag und SHA256 bzw. gleichwertiger reproduzierbarer Identität,
3. BCC64X als tatsächliche Zieltoolchain,
4. Release und Debug in getrennten Buildverzeichnissen,
5. Shared DLL + Import-Library als Standard, sofern die Bibliothek Shared unterstützt,
6. statische Produkte nur zusätzlich und eindeutig benannt,
7. Upstream-Tests nicht ohne Analyse abschalten,
8. Install-Vertrag mit expliziten `<require>`-Gates,
9. Publish in den gemeinsamen Consumer-Baum,
10. Compile-/Link-Smoke und bei sinnvoller Funktion auch Runtime-Smoke,
11. Patches versionsgebunden und vor Apply mit `git apply --check` geprüft,
12. keine bibliotheksspezifische Logik in den generischen BuildEngine-Kern verschieben, wenn sie im XML-/Admin-Vertrag beschrieben werden kann,
13. echte Dependencies im DAG modellieren; keine künstlichen Dependencies nur zur Scheduler-Auslastung,
14. nach mehreren neuen unabhängigen Paketen einen vollständigen Bulk-Lauf durchführen und Scheduler-/CPU-Auslastung neu bewerten.

## BuildEngine-Vertragsorganisation

`admin/build-libraries.xml` bleibt der Hauptvertrag. Zusätzliche Bibliotheken können als vollständige Schema-Dokumente unter `admin/build-libraries.d/*.xml` liegen. BuildEngine führt diese Fragmente deterministisch vor der bestehenden Validierung zusammen. Duplicate-ID-, Dependency-, Tool-, Variant-, Publish- und Smoke-Prüfungen bleiben zentral.

Aktuell nutzen Xerces-C und ACE/TAO diesen Fragmentmechanismus. Weitere größere Pakete sollen bevorzugt ebenfalls als eigenständige Fragmente organisiert werden, statt den Hauptvertrag unnötig weiter aufzublähen.

## BuildEngine-interne Folgearbeiten

### Publish-/Consumer-Ownership

Offen bleibt die saubere Eigentümertrennung im gemeinsamen `Win64x`-Consumer-Baum. Der Consumer-Integrationsschritt darf keine von einzelnen Paketen publizierten CMake-Dateien als vermeintlich eigene Altdateien entfernen.

Zielbild:

- eigenes Manifest für `integration:consumer`, z. B. `.buildengine/manifests/integration-consumer.manifest`,
- Löschen ausschließlich der vom Consumer selbst besessenen Dateien,
- Kollisionsprüfung gegen Paketmanifeste,
- danach prüfen, ob unnötige Wiederholungen von Publish-Schritten vollständig verschwinden.

Diese Arbeit ist unabhängig von der ACE/TAO-zlib-Integration und darf deren aktuellen Erfolgsstand nicht vermischen.

### Bulk-/Scheduler-Verifikation

Nach Aufnahme mehrerer unabhängiger Pakete, insbesondere SQLite, SDL2, GLEW und raylib:

- vollständigen Build-All/Bulk-Lauf ausführen,
- tatsächliche Worker-/CPU-Auslastung beobachten,
- Critical Path und Leerlaufphasen dokumentieren,
- erst dann entscheiden, ob innerhalb großer Einzelpakete wie ACE/TAO zusätzliche Parallelisierung notwendig ist.

Ziel ist Gesamtdurchsatz des DAG, nicht maximale Parallelisierung jedes einzelnen Buildwerkzeugs um jeden Preis.
