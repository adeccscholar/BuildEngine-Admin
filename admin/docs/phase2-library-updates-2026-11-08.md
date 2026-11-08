# Bibliotheks-Checkliste: Phase 2 (2026-11-08)

[TOC|Content]

## Ziel und Abgrenzung

Phase 2 aktualisiert Test- und Parallelbibliotheken für native Windows-x64-Builds
mit RAD Studio 13 / BCC64X. Der authoritative Katalog ist
`admin/build-libraries.xml`. Das Testziel umfasst auch den Neustart
und die Repo-Synchronisation der BuildEngine mit der aktualisierten
BuildEngine-Common-DLL. Ein erfolgreicher XML-/Patchabgleich ist **kein**
Nachweis für ein erfolgreiches BCC64X-Build- oder CTest-Ergebnis.

## Neu aktive Koordinaten

| Bibliothek | Vorher | Aktiv | Upstream-Bindung | Consumer |
| --- | --- | --- | --- | --- |
| Catch2 | 3.16.0 | **3.16.1** | v3.16.1, 08092139210881f01a34e60c5f2fd6afa0ea4024 | Catch2 Release/Debug SelfTests + Consumer Smoke |
| GoogleTest/GoogleMock | 1.17.0 | **1.18.0** | v1.18.0, 063de7e9578f82b369302001269680b4b1553359 | GTest-GMock Consumer Smoke |
| oneTBB | 2023.1.0 | **2023.2.0** | v2023.2.0, 41158a44ab3fb7ac5df7f99ef82626c87607447f | oneTBB Smoke und Terminate Smoke |

Frühere Koordinaten sind als `supersededBy` markiert und bleiben
nachvollziehbar. Jeder neue Katalogeintrag erhält einen eigenen Timestamp.

Der aktive Abhängigkeitsgraph referenziert `onetbb@2023.2.0` nun ebenfalls
bei **VTK 9.6.2** und **OpenCV 5.0.0**. Beide haben deshalb einen geänderten
Timestamp und müssen erneut gebaut und getestet werden; ältere
Consumer-PASS-Ergebnisse dürfen nicht als Nachweis für den aktualisierten
TBB-Stand gelten.

Die drei versionsgebundenen BCC64X-oneTBB-Patches sind auf dem Upstream-Tag
v2023.2.0 textuell anhand ihrer Hunk-Kontexte geprüft und unter
`admin/patches/onetbb/2023.2.0/` abgelegt:

- `bcc64x-profiling-widechar.patch`
- `bcc64x-clang-exports.patch`
- `bcc64x-terminate-conformance.patch`

Die bestehenden Catch2-BCC64X-Schutzmaßnahmen für `isnan` und die
UTF-8-Execution-Codepage bleiben bestehen. GoogleTest verwendet weiter
den vorhandenen BCC64X-CMake-Target-Fix.

## Offene Altversionspins außerhalb von Phase 2

**Neun** aktive Abhängigkeitsverweise nutzen bewusst noch eine
als abgelöst markierte Koordinate. Diese Migrationen sind **nicht** Teil
dieses Commits, um die kürzlich getestete PDFium/Skia-Umgebung nicht
gleichzeitig erneut umzubauen.

| Alte Koordinate | Aktive Version | Konsumenten |
| --- | --- | --- |
| libjpeg-turbo 3.2.0 | 3.2.1 | Leptonica, libtiff, Skia, QPDF, PoDoFo, Poppler, PDFium |
| Expat 2.9.0 | 2.10.0 | TECkit |
| libzip 1.11.4 | 1.12.1 | OpenXLSX |

## Nachweise für den nächsten BuildEngine-Lauf

- [x] Upstream-Git-Tags und genaue Commit-IDs verifiziert
- [x] oneTBB-Patch-Hunks mit Upstream v2023.2.0 verglichen
- [x] Drei neue Versionseinträge und `supersededBy`-Einträge vorbereitet
- [x] VTK- und OpenCV-Abhängigkeitspins samt Timestamps aktualisiert
- [x] Aktiven Kataloggraphen erneut auf alte Versionsverweise geprüft
- [ ] BuildEngine-Common mit Repository-Mutex und geflushtem Git-Prozesslog neu gebaut
- [ ] Automatischer BuildEngine-Neustart und Admin-Fetch unter realen Windows-Prozessbedingungen erfolgreich
- [ ] Catch2 3.16.1 Debug/Release: CMake, Build, CTest, Install, Consumer
- [ ] GoogleTest 1.18.0 Debug/Release: Build, Install, Consumer
- [ ] oneTBB 2023.2.0 Debug/Release: Patches, Build, CTest, Install, Consumer und Terminate-Smoke
- [ ] VTK 9.6.2 / OpenCV 5.0.0: Regression mit oneTBB 2023.2.0
- [ ] Veröffentlichung, Metadaten, Lizenznachweise und SBOM-Prüfungen

**Status:** Native Build- und Laufzeitprüfungen sind [nicht verifiziert].
