# Boost 1.92.0 / BCC64X – Runtime-Status und Handoff

Stand: 2026-08-31

Dieses Dokument ist der technische Wiedereinstiegspunkt für den Boost-1.92.0-Runtimebefund mit C++Builder 13 / BCC64X. Die Diagnose ist für den aktuellen Third-Party-Aufbau abgeschlossen: der verbleibende Textarchive-Fehler ist als bekannte BCC64X/libc++-DLL-Grenze dokumentiert und blockiert den weiteren Bibliotheksgraphen nicht mehr.

## 1. Ziel und Toolchain

Verifizierter Compiler-/Target-Vertrag:

- Compiler: BCC64X / Clang 20.1.7
- Sprache: C++23
- Target: `x86_64-w64-windows-gnu`
- C++-Standardbibliothek: LLVM libc++
- Windows-GNU-Unterbau: MinGW-w64 / UCRT
- Thread-Modell: posix
- Boost.Config läuft über den nativen Clang-Zweig.
- `BOOST_EMBTC` ist im verwalteten Consumer-Pfad nicht aktiv.
- `BOOST_NO_CXX11_NOEXCEPT` ist nicht aktiv.

MSVC, clang-cl oder eine andere Ersatztoolchain sind nicht Bestandteil dieses Nachweises.

## 2. Historische Referenz

Der historische R193-Nachweis für Boost 1.92.0 war grün. Dort lief Serialization zusammen mit Boost.Iostreams, Boost.Locale, Boost.Nowide und Boost.URL und führte einen normalen `text_oarchive`/`text_iarchive`-Roundtrip aus.

Die heutige Reproduktion dieser größeren Link-Komposition änderte das aktuelle Fehlerbild nicht. Die zusätzliche Link-Komposition ist daher als alleinige Ursache ausgeschlossen.

## 3. Gesicherte positive Befunde

Folgende Pfade sind mit dem aktuellen BCC64X-/Boost-1.92.0-Paket verifiziert:

- Boost.Charconv Runtime: PASS
- Boost.URL Runtime: PASS
- Standard-Locale/Codecvt für `char` und `wchar_t`: PASS
- `std::stringstream::imbue` und `std::wstringstream::imbue`: PASS
- `boost::archive::codecvt_null<char>` und `<wchar_t>`: PASS
- `boost::archive::basic_ostream_locale_saver` lokal: PASS
- Boost.Iostreams Plain Output + Flush: PASS
- lokale Nachbildung der Text-Primitive-Lebensdauer auf Standardstream: PASS
- lokale Nachbildung derselben Lebensdauer auf Boost.Iostreams: PASS
- Boost.Serialization Binary Archive auf Standardstream: PASS
- Boost.Serialization Binary Archive auf Boost.Iostreams: PASS
- `std::uncaught_exceptions()` im EXE: PASS
- `boost::core::uncaught_exceptions()` im EXE: PASS
- beide Funktionen aus einer separaten BCC64X-DLL: PASS

Damit sind Boost.Iostreams und Boost.Serialization insgesamt ausdrücklich **nicht** als defekt zu klassifizieren. Der verbleibende Befund betrifft eine engere C++-Stream-ABI-Grenze.

## 4. Entscheidender Boundary-Befund

Ein minimales eigenes BCC64X-DLL-Reproducer erhält eine im EXE erzeugte `std::ostream&`.

Reproduzierter Ablauf in Release und Debug:

```text
local-ostream-endl      PASS
dll-ostream-put         PASS
dll-ostream-flush       PASS
dll-ostream-insert-char BEGIN
<0xC0000005>
```

Damit ist der verbleibende Fehler nicht mehr nur ein Boost.Serialization-Indiz.

> Auf der untersuchten BCC64X/LLVM-libc++-Konfiguration kann eine BCC64X-DLL auf einer im EXE erzeugten `std::ostream&` die elementare Memberfunktion `put()` und `flush()` erfolgreich benutzen, während der freie/überladene C++-Stream-Insertion-Pfad `operator<<` reproduzierbar mit `0xC0000005` scheitert.

`std::endl` enthält denselben Insertion-Pfad und ist damit ebenfalls betroffen.

Dieser Befund erklärt den zuvor eingegrenzten Boost.Serialization-Textarchive-Absturz sehr plausibel: `basic_text_oprimitive<std::ostream>` ist in `boost_serialization.dll` explizit instanziiert und führt in seinem Destruktor `os << std::endl` auf einem vom Consumer bereitgestellten Stream aus.

Die Kausalität zwischen genau diesem Aufruf und jedem möglichen Textarchive-Fall ist technisch sehr stark belegt, wird aber nicht über den reproduzierten Boundary-Befund hinaus verallgemeinert.

## 5. Boost.Iostreams ist abgeschlossen

Boost.Iostreams bleibt aus Beschreibung und TODO als offene Ursache entfernt.

Verifiziert sind:

1. normales Schreiben und Flush mit `boost::iostreams::stream`;
2. lokale Stream-/Locale-Lebensdauer;
3. vollständiger Binary-Archive-Roundtrip.

Der Fehler tritt erst auf, wenn Code aus einer DLL den problematischen `std::ostream`-Insertion-Pfad auf einem Consumer-Stream verwendet.

## 6. Acceptance-Entscheidung

Der bekannte Crashpfad bleibt als reproduzierbare Diagnosequelle im Repository erhalten, wird aber **nicht mehr als normaler Acceptance-Gate ausgeführt**.

Der aktive `boost-evidence-runtime-serialization.exe` prüft weiterhin:

- lokalen `std::endl`-Pfad;
- DLL `put()`;
- DLL `flush()`;
- lokale Text-Primitive-Lebensdauer;
- Boost.Iostreams Plain Output;
- lokale Text-Primitive-Lebensdauer auf Boost.Iostreams;
- Binary Serialization über Boost.Iostreams.

Danach meldet er zwei stabile `KNOWN-LIMITATION`-Zeilen und beendet sich erfolgreich.

Die folgenden Funktionen bleiben nur als Reproducer im Quelltext und werden im Acceptance-Lauf nicht aufgerufen:

- DLL `operator<<('\n')`;
- DLL `std::endl`;
- echte Boost.Serialization-Textarchive-Construct/Destroy-Probes.

Damit bedeutet ein grünes Boost-Gate künftig:

> Das definierte BCC64X-Benutzungsprofil ist verifiziert; die bekannte libc++-C++-Stream-DLL-Grenze und davon betroffene Boost.Serialization-Textarchives sind dokumentiert und nicht Teil des akzeptierten Profils.

Es bedeutet ausdrücklich nicht, dass der bekannte Textarchive-Reproducer repariert wurde.

## 7. Reproducer-Dateien

Aktive bzw. erhaltene Diagnosequellen:

- `admin/smokes/boost/component-gate/runtime-serialization.cpp`
- `admin/smokes/boost/component-gate/runtime-stream-boundary.cpp`
- `admin/smokes/boost/component-gate/runtime-stream-boundary.h`
- `admin/smokes/boost/component-gate/runtime-uncaught.cpp`
- `admin/smokes/boost/component-gate/runtime-uncaught-boundary.cpp`
- `admin/smokes/boost/component-gate/runtime-uncaught-boundary.h`

## 8. Nicht erneut verfolgen ohne neue Evidenz

- Boost.Iostreams als eigenständiger Defekt
- größere R193-Link-Komposition
- Boost.Locale-WinAPI-Backend
- allgemeiner Locale-/Codecvt-Defekt
- `boost::core::uncaught_exceptions()`
- Binary-Archive allgemein
- Nutzdaten-/`std::string`-Roundtrip als primäre Ursache

## 9. Relevante Admin-Commits

- `c218b8ba3c838b5cd6a4d757b5197cf70a6a3d53` – R193 Serialization-Linkgraph reproduziert
- `f00eafc55b8368529cb67ab0ed64fd9c43a9d4be` – Boost.Locale BCC64X WinAPI-Backend deaktiviert
- `5fc3562e7d5658b27d4b3708e79f26ff99fd51c9` – Locale/Codecvt-/Archive-Diagnose erweitert
- `2f9b810ff0dc69e874516151e96cc71d44d6df3d` – Konstruktor/Destruktor-Checkpoints
- `fe4a05017829dbb2c358c066f85dc634da2a87b4` – Serialization-Boundary-Test
- `2260fdc79885969a8e40bf92cc311119a6781d0d` – `uncaught_exceptions()` DLL-Reproducer
- `8fc158cb48d25a1108f5ec50af62f1ce014acb9f` – `std::ostream&` DLL-Boundary integriert
- `98328f3a60848910e8e0951954b2061ba55d837a` – bekannten Crashpfad aus Acceptance entfernt, Evidenz erhalten

## 10. BuildEngine-Currentness

Während der Diagnose wurde zusätzlich festgestellt, dass Smoke-Currentness bislang nur den Source-Pfad, nicht den Source-Inhalt berücksichtigte. Deshalb konnten geänderte Smoke-Quellen fälschlich als `current` gelten.

BuildEngine-Commit:

- `9ae53c65f530bf57c889123380a263723523175b` – Smoke-Sourcebaum wird über relative Dateipfade + SHA-256 fingerprinted; Änderungen invalidieren den betroffenen Smoke.

## 11. Nächster Third-Party-Arbeitsblock

Boost ist für den aktuellen Projektfortschritt abgeschlossen.

Nächste Reihenfolge:

1. Xerces-C als eigenständiges BCC64X-Paket integrieren und verifizieren.
2. Danach ACE 8.0.6 / TAO 4.0.6 auf Basis des bereits erfolgreich verifizierten Evidenzpfades integrieren.
3. Xerces-C als Abhängigkeit für die ACE/TAO-Komponenten aktivieren, die im früheren MPC-Lauf wegen `requires xerces` ausgelassen wurden, insbesondere `ACE_XML_Utils`.
4. Bestehende ACE/TAO-Evidenz, Patches, MPC/BMake-Vertrag und Service-Gates übernehmen; keine Neuerfindung des erfolgreichen Ports.
