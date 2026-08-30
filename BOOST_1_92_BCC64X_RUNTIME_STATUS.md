# Boost 1.92.0 / BCC64X – Runtime-Status und Handoff

Stand: 2026-08-31

Dieses Dokument ist der aktuelle technische Wiedereinstiegspunkt für die noch offene Boost-1.92.0-Runtimeanalyse mit C++Builder 13 / BCC64X. Es hält ausdrücklich auch ausgeschlossene Ursachen fest, damit spätere Arbeiten nicht bereits erledigte Diagnosezweige wiederholen.

## 1. Ziel und Toolchain

Ziel ist der reproduzierbare Boost-1.92.0-Produkt- und Consumer-Nachweis mit der nativen C++Builder-13-Toolchain.

Verifizierter Compiler-/Target-Vertrag:

- Compiler: BCC64X / Clang 20.1.7
- Sprache: C++23
- Target: `x86_64-w64-windows-gnu`
- C++-Standardbibliothek: LLVM libc++
- Windows-GNU-Unterbau: MinGW-w64 / UCRT
- Thread-Modell: posix
- Boost.Config muss über den nativen Clang-Zweig laufen.
- `BOOST_EMBTC` darf im verwalteten Consumer-Pfad nicht aktiv sein.
- `BOOST_NO_CXX11_NOEXCEPT` darf nicht aktiv sein.

MSVC, clang-cl oder eine andere Ersatztoolchain sind nicht Bestandteil dieses Nachweises.

## 2. Historische Referenz

Der historische R193-Nachweis für Boost 1.92.0 war grün. Dort lief Serialization zusammen mit weiteren Bibliotheken in einem größeren Prozess und führte einen normalen `text_oarchive`/`text_iarchive`-Roundtrip aus.

Die damalige Prozesskomposition enthielt mindestens:

- Boost.Iostreams
- Boost.Locale
- Boost.Nowide
- Boost.Serialization
- Boost.URL

Die heutige Reproduktion dieser größeren Link-Komposition änderte den Fehler nicht. Die historische Prozessgröße bzw. die zusätzliche Link-Komposition ist daher als alleinige Ursache ausgeschlossen.

## 3. Aktueller Produktionszustand

Boost 1.92.0 ist in BuildEngine für Source, Build, Install und Publish grundsätzlich vorhanden. Die übrigen Boost-Component-Gates sind grün. Der offene Punkt liegt im Runtime-Gate `system-io-runtime`, konkret im echten Boost.Serialization-Textarchive-Pfad.

Ein zwischenzeitlich vollständig grüner BuildEngine-Lauf war **kein** Nachweis einer reparierten Serialization. Für einen isolierten Diagnosezustand war `boost-evidence-runtime-serialization.exe` temporär durch einen `uncaught_exceptions()`-Boundary-Test ersetzt worden. Dieser Zustand wurde wieder aufgehoben.

Der aktive CMake-Smoke baut jetzt wieder:

- `boost-evidence-runtime-charconv.exe`
- `boost-evidence-runtime-serialization.exe` aus `runtime-serialization.cpp`
- `boost-evidence-runtime-url.exe`

Der echte Serialization-Test linkt wieder gegen:

- `Boost::iostreams`
- `Boost::serialization`
- die lokale Diagnose-DLL `boost-evidence-stream-boundary`

## 4. Gesicherte Runtime-Befunde

### 4.1 Charconv und URL

Die getrennten Runtime-Probes für Charconv und URL laufen erfolgreich. Sie gehören nicht zum offenen Fehlerbild.

### 4.2 Boost.Iostreams ist als eigenständige Ursache ausgeschlossen

Dieser Punkt ist abgeschlossen und soll **nicht** als offener Verdacht oder TODO weitergeführt werden.

Mit einem echten `boost::iostreams::stream<back_insert_device<std::string>>` wurden erfolgreich nachgewiesen:

1. normales Schreiben und Flush;
2. lokale Nachbildung der für `basic_text_oprimitive` relevanten Stream-/Locale-Lebensdauer;
3. vollständiger `binary_oarchive`/`binary_iarchive`-Roundtrip über Boost.Iostreams.

Erst die Verwendung eines echten `boost::archive::text_oarchive` auf demselben funktionierenden Boost.Iostreams-Stream führt in den bekannten Fehlerpfad.

Damit gilt:

> Boost.Iostreams selbst funktioniert im untersuchten BCC64X-Consumer. Der offene Fehler ist nicht als Boost.Iostreams-Defekt zu beschreiben, sondern als Boost.Serialization-Textarchive-/Destruktionsproblem.

### 4.3 Standard-Locale und Codecvt-Grundfunktionalität

Folgende isolierte Tests liefen erfolgreich:

- `std::codecvt<char, char, std::mbstate_t>`
- `std::codecvt<wchar_t, char, std::mbstate_t>`
- `std::stringstream::imbue(std::locale::classic())`
- `std::wstringstream::imbue(std::locale::classic())`
- `boost::archive::codecvt_null<char>`
- `boost::archive::codecvt_null<wchar_t>`
- `boost::archive::basic_ostream_locale_saver` als isolierte lokale Verwendung

Daraus folgt nicht, dass jede Locale-/ABI-Interaktion im Serialization-DLL-Pfad korrekt ist. Es ist aber kein allgemeiner Defekt dieser Typen nachgewiesen.

### 4.4 Binary Serialization funktioniert

Ein echter Boost.Serialization-Binary-Archive-Roundtrip läuft erfolgreich, sowohl mit Standardstream als auch mit Boost.Iostreams als Streamträger.

Damit ist Boost.Serialization nicht insgesamt defekt. Der Fehler ist auf den Textarchive-Pfad eingegrenzt.

### 4.5 `no_codecvt` behebt den Fehler nicht

Der Textarchive-Fehler tritt auch mit `boost::archive::no_codecvt` auf.

Zusätzlich wurde der Headerpfad ausgeschaltet (`no_header | no_codecvt`). Der Konstruktor von `boost::archive::text_oarchive` wird dabei erfolgreich verlassen. Der Prozess scheitert erst beim Verlassen des Archive-Scopes.

Damit sind als alleinige Ursache ausgeschlossen:

- Headererzeugung;
- Codecvt-Imbue im Textarchive-Konstruktor;
- Serialisierung eines Nutzwerts, denn der minimale Construct/Destroy-Test speichert keinen Wert.

### 4.6 Aktuell engste Fehlerstelle

Reproduzierbarer Ablauf:

```text
before-ctor
after-ctor
<Fehler beim Verlassen des Scopes>
```

`after-dtor` wird nicht erreicht.

Der relevante Pfad ist damit die Destruktion eines echten `boost::archive::text_oarchive`, insbesondere dessen `basic_text_oprimitive<std::ostream>`-Basisklasse und nachfolgender Memberabbau.

Boost 1.92.0 instanziiert `basic_text_oprimitive<std::ostream>` explizit in der Serialization-Bibliothek. Der Destruktor-Body enthält im Wesentlichen:

```cpp
if(boost::core::uncaught_exceptions() > 0)
   return;
os << std::endl;
```

Danach werden die Member der Basisklasse zerstört, unter anderem Locale-Saver, Archive-Locale und Codecvt-Facet.

## 5. `boost::core::uncaught_exceptions()` ist ausgeschlossen

Ein eigener Minimaltest wurde sowohl direkt im EXE als auch über eine kleine BCC64X-DLL ausgeführt.

Verifiziert wurde:

- `__cpp_lib_uncaught_exceptions == 201411`
- Boost.Core verwendet damit den `std::uncaught_exceptions()`-Zweig.
- `std::uncaught_exceptions()` im EXE: PASS, Ergebnis 0.
- `boost::core::uncaught_exceptions()` im EXE: PASS, Ergebnis 0.
- `std::uncaught_exceptions()` aus BCC64X-DLL: PASS, Ergebnis 0.
- `boost::core::uncaught_exceptions()` aus BCC64X-DLL: PASS, Ergebnis 0.

Damit ist `boost::core::uncaught_exceptions()` als Ursache des Serialization-Absturzes praktisch ausgeschlossen.

Die zugehörigen Diagnosequellen bleiben im Smoke-Verzeichnis als reproduzierbare Evidenz erhalten:

- `runtime-uncaught.cpp`
- `runtime-uncaught-boundary.cpp`
- `runtime-uncaught-boundary.h`

Sie sind derzeit nicht der aktive Serialization-Run.

## 6. Aktueller nächster Test: `std::ostream&` über DLL-Grenze

Obwohl ein allgemeines `std::ostream`-Problem nicht erwartet wird, wird der verbleibende Destruktor-Body direkt nachgebildet.

Die neue kleine BCC64X-DLL `boost-evidence-stream-boundary` erhält eine vom EXE erzeugte `std::ostream&` und führt getrennt aus:

1. `os.put('\n')`
2. `os.flush()`
3. `os << '\n'`
4. `os << std::endl`

Diese vier Probes laufen am Anfang des echten `boost-evidence-runtime-serialization.exe` und müssen abgeschlossen sein, bevor die bereits bekannten Boost.Iostreams- und Serialization-Stufen beginnen.

Dateien:

- `admin/smokes/boost/component-gate/runtime-stream-boundary.h`
- `admin/smokes/boost/component-gate/runtime-stream-boundary.cpp`
- Integration in `admin/smokes/boost/component-gate/runtime-serialization.cpp`

Interpretation:

- Falls alle Stream-DLL-Probes PASS sind, wird auch `os << std::endl` als isolierter DLL-Grenzpfad ausgeschlossen. Dann ist der nächste Fokus der implizite Memberabbau von `basic_text_oprimitive` bzw. die konkrete Boost.Serialization-DLL-ABI/Instantiation.
- Falls eine Stream-DLL-Probe scheitert, existiert ein wesentlich kleinerer BCC64X/libc++-DLL-Reproducer unabhängig von Boost.Serialization.

## 7. Nicht erneut als Hauptursache verfolgen

Folgende bereits geprüfte Ansätze sollen ohne neue Evidenz nicht erneut als Hauptdiagnose gestartet werden:

- Boost.Iostreams als eigenständiger Defekt;
- größere R193-Link-Komposition;
- Boost.Locale-WinAPI-Backend;
- allgemeiner Codecvt-Defekt;
- `boost::core::uncaught_exceptions()`;
- Binary-Archive allgemein;
- Nutzdaten-/`std::string`-Roundtrip als primäre Ursache.

Die aktuelle Boost.Locale-Produzentenpolicy mit deaktiviertem WinAPI-Backend wurde in Release und Debug wirksam nachgewiesen und änderte den Serialization-Fehler nicht.

## 8. Offene Diagnose nach dem Stream-Boundary-Test

Wenn die vier `std::ostream&`-DLL-Probes PASS sind, ist in dieser Reihenfolge fortzusetzen:

1. Impliziten Memberabbau von `basic_text_oprimitive<std::ostream>` isolieren.
2. Insbesondere Lifetime/ABI von `locale_saver`, `archive_locale` und `codecvt_null_facet` über die echte Serialization-DLL-Grenze untersuchen.
3. Aktuelle gegenüber historischer R193-Produzenten-Compile-/Visibility-Konfiguration vergleichen.
4. Tatsächlich geladene DLL-Identität im Prozess prüfen, nicht nur `PATH` oder `where`.
5. Erst danach einen Produzentenpatch erwägen.

Unbelegte Annahmen sind weiterhin als `[nicht verifiziert]` zu kennzeichnen.

## 9. Relevante Admin-Commits der Diagnose

Wichtige jüngere Schritte:

- `c218b8ba3c838b5cd6a4d757b5197cf70a6a3d53` – R193 Serialization-Linkgraph reproduziert.
- `f00eafc55b8368529cb67ab0ed64fd9c43a9d4be` – Boost.Locale BCC64X WinAPI-Backend deaktiviert.
- `5fc3562e7d5658b27d4b3708e79f26ff99fd51c9` – erste Serialization-Diagnose erweitert.
- `2f9b810ff0dc69e874516151e96cc71d44d6df3d` – Konstruktor/Destruktor-Checkpoints ergänzt.
- `fe4a05017829dbb2c358c066f85dc634da2a87b4` – Serialization-Boundary-Test als gezielter Smoke.
- `2260fdc79885969a8e40bf92cc311119a6781d0d` – isolierter `uncaught_exceptions()`-DLL-Boundary-Test.
- `8fc158cb48d25a1108f5ec50af62f1ce014acb9f` – echten Serialization-Smoke wiederhergestellt und `std::ostream&`-DLL-Boundary integriert.

## 10. BuildEngine-Nebenbefund

Unabhängig vom Boost-Runtimeproblem wurde im BuildEngine die inkrementelle Current-Prüfung aus dem synchronen Vorlauf in Scheduler-Worker verschoben. Dadurch können Current-Prüfungen als aktive Scheduler-Arbeit über Heartbeat sichtbar werden.

Relevante BuildEngine-Commits:

- `f9d8bdc7f24c3faafaa4080bb0b64010f5ecbf8c` – inkrementelle Checks schedulerfähig gemacht.
- `8734f5fbd50d5d054c80a6d819db81246ef0b19d` – Current-Check als Worker-Phase `check` ausgeführt.
- `5a88a1e2b3d7ac760b71f3c6dd822c0b977eb76d` – alle Jobs vor der Current-Auswertung in den Scheduler gegeben.
- `1a8b1eed8a9c53e5cc418dfb54b9266abaa0ea4c` – irreführenden Heartbeat ohne aktive Scheduler-Arbeit unterdrückt.

Die vorübergehend vereinfachte Publish-Current-Prüfung ist nicht als endgültige Semantik gedacht. Ziel bleibt eine strikte Publish-Prüfung im Worker mit sichtbarem Fortschritt, nicht eine dauerhaft abgeschwächte Integritätsprüfung.

## 11. Nächster praktischer Lauf

Normalen BuildEngine-Lauf ohne `--rebuild` starten.

Erwartung für `system-io-runtime`:

1. Charconv PASS.
2. Im Serialization-Log zuerst fünf erfolgreiche lokale/DLL-Stream-Stufen, sofern die `std::ostream&`-Grenze unproblematisch ist.
3. Danach bereits bekannte Boost.Iostreams-Evidenz PASS.
4. Anschließend erneuter Fehler beim echten `text_oarchive`-Scope, sofern der eigentliche Serialization-Fehler unverändert besteht.
5. URL PASS.

Der Gesamt-Smoke muss solange rot bleiben, wie der echte Serialization-Textarchive-Test scheitert.
