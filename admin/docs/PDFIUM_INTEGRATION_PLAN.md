# PDFium in die BuildEngine integrieren
## Nativer C++23-/BCC64X-Build mit vorhandener Skia-153-Bibliothek

Stand: 2026-10-15  
Status: **Entwurfs- und Umsetzungsplan, kein erfolgreich verifizierter PDFium-Build**

### 0. Ziel, Grenzen und verbindliche Entscheidungen

PDFium wird als eigenständiges Third-Party-Paket in die bestehende BuildEngine-FSM integriert (Source/Verify/Patch/Build/Test/Install/License/Package/Bootstrap). Vorrangiges Ziel ist **ein nativer BCC64X-Build des C++-Kerns**, keine fremde C-ABI-DLL als Primärweg. Ein eigener moderner C++23-Adapter kapselt instabile PDFium-interne Typen; PoDoFo bleibt für die bereits vorhandene Formular- und PDF-Bearbeitung erhalten.

**Nicht verhandelbar:**

1. PDFium nutzt **genau das vorhandene BuildEngine-Paket Skia 153** (Git-Commit \`2eed75b956045eb8603d3690a1e84bc582a2135d\`), keine zweite, implizit heruntergeladene oder separat kompilierte Skia-Bibliothek.
2. Andere vorhandene Pakete (zlib, FreeType, ICU, JPEG, PNG usw.) werden bevorzugt wiederverwendet. Unvereinbarkeiten sind als Testbefund zu dokumentieren, nicht durch heimliches Bundling zu kaschieren.
3. Fehlende, eigenständig nützliche und permissiv lizenzierte Bibliotheken werden **vor PDFium** als reguläre BuildEngine-Pakete bereitgestellt.
4. Keine automatische Änderung von Skia-153-Pins. Eine passende PDFium-Revision wird gewählt oder ein minimaler, versionierter Kompatibilitätspatch ausgearbeitet. Ist beides nicht möglich, bleibt die Integration **blockiert**.
5. Keine stillen Netzwerkzugriffe im Build. Alle Downloads im Source-Schritt mit erwarteten SHA-256-Werten, vollständigen Revisionen, Lock-Einträgen, nachweisbaren Lizenzdateien.
6. Keine PowerShell-, .NET-, Cygwin-/cygpath- oder systemweiten Paketmanager-Abhängigkeiten. GN/Ninja, bcc64x, llvm-ar und Python entsprechend den bestehenden BuildEngine-Tools.
7. GPL/AGPL-Bestandteile nicht versehentlich in die neue PDFium-Laufzeitabhängigkeit aufnehmen. Lizenzstatus pro **tatsächlich verwendeter Variante** ermitteln, nicht allein nach dem Namen der Hauptbibliothek.
8. Eingebundene C++-Bibliotheken dürfen keine inkompatiblen CRT-, STL-, Exception-, RTTI- oder Exportkonfigurationen mischen.

### 1. Gesicherter BuildEngine-Bestand

Grundlage: \`BuildEngine-Admin/admin/build-libraries.xml\`, Bibliothek \`skia/153\`.

| Paket | Vorhanden | Rolle / Handlungsbedarf |
|---|---|---|
| Skia 153 | Ja | **Pflicht**, exakt bestehendes Paket; vorhandene BCC64X-GN-Portierung, geteilter/component-Build prüfen |
| zlib 1.3.2 | Ja | GN-External-Target, kein zweites zlib |
| libjpeg-turbo 3.2.0 | Ja | JPEG-Decoding; GN-External-Target/Definitionsprüfung |
| libpng 1.6.58 | Ja | Skia-Codec-/optional PDFium-Pfade, kein zweites libpng |
| FreeType 2.14.3 | Ja | PDF-Fonts; Unterschied zu PDFiums modifizierter FreeType-Konfiguration prüfen |
| ICU | Ja | fxcrt/Unicode; ABI-, Daten- und Versionstest |
| HarfBuzz 14.4.0 | Ja | Skia-Abhängigkeit; keine ungeprüfte neue Kopie |
| Brotli 1.2.0 | Ja | Optional; PDFiums experimentelle Brotli-PDF-Funktion zunächst aus |
| libtiff | Ja | XFA-TIFF vorläufig ausgeschaltet |
| googletest | Ja | Smoke-/Unit-Tests mit vorhandener Bibliothek, nicht doppelt bauen |
| GN, Ninja, llvm-ar, Python | In BuildEngine/Skia-Workflow etabliert | Version und Hosttools im Lock erfassen |

PDFium erwartet zurzeit eine eigene Chromium-GN-Struktur. Das schon portierte GN-Erzeugungssystem von Skia wird **als Implementierungsvorbild** genutzt, nicht automatisch als drop-in-kompatibel angenommen.

### 2. Fehlende Bibliotheken: zuerst regulär aufnehmen

**Priorität A – vor PDFium separat in BuildEngine aufnehmen:**

| Paket-ID (Vorschlag) | Upstream / Lizenzsignal | Funktion | Paketstrategie |
|---|---|---|---|
| \`abseil\` / \`abseil-cpp\` | Google Abseil, Apache-2.0 | direkte Core-Abhängigkeit (\`fxcrt\`) | gepinnter kompatibler Stand, CMake, bcc64x, Tests, CMake-Targets und Lizenz |
| \`fast_float\` | fastfloat/fast_float, MIT / Apache-2.0 / BSL-1.0 optionale Lizenzvarianten | \`fxcrt\`-Zahlenparser | Header-only/Interface-Paket, Compile-Tests und Lizenztext |
| \`lcms2\` | LittleCMS 2, MIT | ICC-Farbmanagement | unabhängiges C-Paket mit Export, Smoke-Tests, Windows-GN-External-Adapter |
| \`openjpeg\` | OpenJPEG, BSD-2-Clause | JPEG 2000 / JPX | eigenständiges Paket, JPEG2000-Smoke und Kompatibilität mit PDFiums forked sources prüfen |

**Priorität B – prüfen, nur bei echter Abhängigkeit vorziehen:**

- \`simdutf\`: MIT **oder** Apache-2.0; in PDFiums \`DEPS\` vorhanden, aber zunächst die produktive, mit den gewählten GN-Flags erreichbare Target-Closure ermitteln. Wenn benötigt, eigenständiges Paket mit UTF-8/UTF-16-Tests.
- \`fp16\`: MIT; in \`DEPS\` vorhanden, noch keine bestätigte zwingende Laufzeitabhängigkeit unseres konkreten Profils. Bei Bedarf Header-only-Paket.
- Sonstige Chromium-Hilfsbausteine: nach \`gn desc\`/Abhängigkeitsauflösung bewerten. Allgemein wiederverwendbare permissive Libraries separat paketieren; GN-Buildskripte und PDFium-spezifische Shader-/Konfigurationshilfen bleiben **reproduzierbare Build-Inputs**, keine künstlich verselbstständigten Runtime-Pakete.

**Nicht pauschal integrieren:** V8, XFA-Unterbau, PartitionAlloc, Rust/Fontations, Chromium-Codecs, Android-Werkzeuge, CIPD-Compiler-Downloads, Fuzz-Corpora. Diese sind im Minimalprofil abgeschaltet oder gehören zu späteren optionalen Profilen. Ihre bloße Nennung in \`DEPS\` macht sie nicht automatisch zur Laufzeitabhängigkeit.

**Aufnahmeregel für jedes neue Paket:** Versionspin + Download-Hash + Lizenz(en) + Patches + Debug/Release oder Interface-Vertrag + Consumer-Smoke mit BCC64X + Install/Package + Lock/Bootstrap + Dependency-Closure + SBOM. Erst danach darf \`pdfium\` darauf verweisen.

### 3. PDFium-Revision und Skia-Kompatibilität: Gate 0

Offizielle PDFium-Dateien: \`DEPS\`, \`pdfium.gni\`, \`BUILD.gn\`, \`skia/BUILD.gn\`, \`core/fxge/BUILD.gn\`.

- Der zum untersuchten PDFium-\`DEPS\`-Stand gehörende Skia-Pin (\`7ef86a5b0eb91db85580bd71ebed3c4ba489d5ba\`) weicht von unserem installierten Skia 153 (\`2eed75b956045eb8603d3690a1e84bc582a2135d\`) ab. Keine API- oder ABI-Kompatibilität aus dem gemeinsamen Google-Ursprung ableiten.
- Drei PDFium-Revisionskandidaten aus dem zeitlichen Umfeld von Skia/153 auswählen: möglichst dieselbe Chromium/Skia-Milestone-Familie, dann neuere stabile PDFium-Revisionen. **Jeden** Kandidaten mit Commit-SHA und kompletter \`DEPS\`-Map dokumentieren; eine ungeprüfte „aktuelle“ Revision wird nicht festgelegt.
- Skia-Abgleich: \`git merge-base\`/Commit-Abstand sowie API-Signaturen der von \`core/fxge/skia/*\` und \`skia/config/SkPdfiumUserConfig.h\` benötigten Typen; Build-Flags, Header, Exportmakros, Inline-Funktionen und Linksymbole.
- PDFium verwendet in seinem GN-Projekt \`//skia\` mit Quellimporten aus \`//third_party/skia/gn/*.gni\`, \`skcms\` usw. **Der externe Paketmodus ist keine vorhandene Standard-GN-Option.** Er wird durch einen BuildEngine-spezifischen, versionsgebundenen GN-Adapter/Patch bereitgestellt.
- Die Skia-153-BuildEngine-Variante hat im GN-Vertrag \`is_component_build = true\`; PDFium soll unabhängig davon (zunächst \`is_component_build = false\`) aufgebaut werden. Die externe Skia-Importkonfiguration muss daher die tatsächlich exportierten Symbole und die passende \`SKIA_DLL\`-/Builddefinition abbilden.
- Eine separate Skia-Kompilation im PDFium-Arbeitsbaum **ist verboten**; auch \`gn desc\` muss zeigen, dass kein \`skia\`-Quellziel aktiv ist.

**Gate-0-Abnahme:** exakt gepinnte PDFium-Revision, geprüfte externe Skia-Header/Exports/ABI, begrenzte Patchliste, vollständiges Lizenz-/Dependency-Manifest. Unaufgelöste Skia-Version = STOP statt zweiter Skia-Kopie.

### 4. Build-Inputs reproduzierbar bereitstellen

BuildEngine-\`source\` stellt ein isoliertes PDFium-Arbeitsverzeichnis aus verifizierten Archiven/Commits her. Die Chromium-\`DEPS\`-Datei ist die **Referenzliste**, nicht die Anweisung zu unkontrolliertem \`gclient sync\`.

Zu beschaffende Inputs nach tatsächlich benötigter GN-Import-Closure:

- PDFium-Quellen und Unterordner, \`pdfium.gni\`, \`build_overrides/*\`, \`skia/*\`-Adapterdateien.
- Chromium \`build\`, \`buildtools\`, benötigte \`testing\`-GN-Definitionen und Python-GN-Hilfen, exakt aus der gepinnten PDFium-\`DEPS\`-Revision.
- Abseil/fast_float/etc. **als BuildEngine-Pakete**, nicht als zusätzliche Kopien unter PDFium.
- GN, Ninja, Python und BCC64X aus BuildEngine-Toolauflösung; \`gn gen\` darf keine automatische Toolchaininstallation starten.
- Prüfen, ob GN-Generierung ohne \`depot_tools\`/\`gclient\`-Hooks möglich ist. Falls minimale Hook-Outputs fehlen: diese deterministisch im Source-/Prepare-Schritt erzeugen, statt komplette Depot-Tool-Updates auszuführen.

Jedes Artefakt besitzt Provenance: URL, Commit/Version, SHA-256, erwartete Extraktionswurzel, Patch-SHA, Lizenz- und NOTICE-Dateien. Erzeugte Source-Bäume sind rebuildbar; keine unversionierte Source-Kopie als Voraussetzung.

### 5. Eigene GN/BCC64X-Portierung

Anknüpfen an \`skia/153\` im bestehenden BuildEngine-XML und dessen \`bcc64x-gn.patch\`-Modell:

1. Eigene, gekapselte \`pdfium_use_bcc64x = true\`-Konfiguration im PDFium-Build (Name vorgeschlagen). Ziel CPU x64, C-/C++-Compiler und Linker \`bcc64x\`, Archiver \`llvm-ar\`, Host GN/Ninja.
2. BCC64X-spezifische MSVC-/Clang-Flagübersetzung und Windows-SDK-/RAD-Library-Pfade. \`clang_use_chrome_plugins = false\`; keine stillschweigende Verwechslung mit MSVC oder GNU-Clang.
3. UTF-8, C++-Standard, exceptions/RTTI, CRT, \`_ITERATOR_DEBUG_LEVEL\`-Äquivalente, Visibility, Exportmakros, Debug/Release pro Variante explizit dokumentieren. Kein undefinierter Mix mit Skia.
4. GN-Target-Umleitung \`//skia\` auf **BuildEngine-Paket-Skia**, einschließlich Includes, öffentlichen Konfigurationen, Importbibliothek, DLL-Suchpfad und transitiven Abhängigkeiten. Kein „source_set“, das Skia-Implementierungsdateien erneut kompiliert.
5. Windows-GN-External-Targets für zlib, JPEG, PNG, FreeType, ICU, LittleCMS, OpenJPEG, Abseil, fast_float statt systemweitem \`pkg-config\`. Externe Pakete nur dann einschalten, wenn PDFium-ABI/API und CMake-/GN-Publikation zusammenpassen.
6. Für FreeType/LCMS2/OpenJPEG die PDFium-spezifischen Anpassungen gegenüber Upstream klären. Externes Paket kann erst nach passenden funktionalen Tests Standard werden; die Sicherheitsregeln und Testfälle des PDFium-Forks dürfen nicht verloren gehen.
7. Patches nach Funktion trennen: \`bcc64x-gn\`, \`buildengine-external-skia\`, \`buildengine-external-deps\`, \`bcc64x-source-compat\`. Jeder Patch erst \`git apply --check\`, danach \`git apply\`; Upstream-Patches nachvollziehbar.
8. Auf leere/duplizierte Archive achten: BCC64X-\`llvm-ar\`-Archiv-Neuaufbau und identische Objektlisten testen, keine inkrementelle Mehrfachaufnahme von Symbolen.

**Schematische GN-Zielkonfiguration (erst nach Target- und Revisionstest als produktive \`args.gn\` verwenden):**

\`\`\`gn
target_cpu = "x64"
is_debug = false
is_component_build = false
clang_use_chrome_plugins = false

pdf_enable_v8 = false
pdf_enable_xfa = false
pdf_use_partition_alloc = false
pdf_enable_fontations = false
pdf_enable_rust_bmp = false
pdf_enable_rust_png = false
pdf_enable_brotli = false

pdf_use_skia = true
pdf_use_agg = true

use_system_zlib = true
use_system_libpng = true
use_system_lcms2 = true
use_system_libopenjpeg2 = true
pdf_bundle_freetype = false

# BuildEngine-spezifischer GN-Patch/Adapter, kein Upstream-Flag:
pdfium_use_bcc64x = true
\`\`\`

Weitere externe Paketflags, insbesondere JPEG, ICU und Skia, sind revisionsabhängig. \`pdf_is_complete_lib\` **anfangs nicht erzwingen**; zunächst modulare C++-Linkfähigkeit und externes Skia nachweisen. Später vollständige Archive nur ohne eingebettete Duplikate externer Pakete testen.

### 6. Profile und optionaler Funktionsumfang

**P0 / Minimal-Viewer und Druck (verbindlich):** BCC64X, Skia-Renderer, AGG als Vergleichs-Backend (mitgebaut, nicht primär), AcroForms, Text, Bilder, ICC, JPEG2000, Druckbitmap, keine V8-Skripte/XFA/Rust/Fontations, keine Netzwerkanbindung.

**P1 / Nutzung im Produkt:** C++23-RAII-Fassade, VCL-Viewer, Zoom/Rotation, Textsuche/Markierungen, Formular-Widgetdarstellung, PoDoFo-Integration, GDI-Druck und Testdrucker.

**P2 / Erweiterungen nur nach gesonderter Entscheidung:** V8/JavaScript, XFA, Fontations/Rust, zusätzliche Bildformate, Linux-Backend, erweiterte GPU-Pfade. Jeder zusätzliche Funktionsblock braucht eigenes Lizenz- und Dependency-Gate.

Für Formulare gilt: ausgefüllter Feldwert und sichtbarer Appearance-Stream sind verschiedene Dinge. Der Viewer muss echte gerenderte AcroForms und ggf. Widget-Overlays prüfen; PoDoFo speichert, PDFium rendert, Druck ist ein eigener Auftragsprozess.

### 7. Native C++23-Nutzung statt C-API-first

- PDFium-intern nutzen, soweit realistisch, \`CPDF_Document\`, \`CPDF_Page\`, Render-/Bitmap-Helfer und ihre native Besitzsemantik.
- Keine unkontrollierte Veröffentlichung dieser instabilen internen Klassen an alle Verbraucher.
- Eigenes API-Paket: \`adecc::pdf::document\`, \`page\`, \`render_surface\`, \`text_search\`, \`form_view\`; move-only und RAII, präzise Fehler-/Resultattypen, kontrollierte Lifetimes.
- Die C-API bleibt **optional** als Referenz-/Differenztest oder Compatibility-Fallback; sie ist **nicht** der primäre Implementierungsweg.
- Für Anzeige: PDFium → Skia-\`SkBitmap\`/\`SkImage\`/Surface nach nachgewiesener API-Kompatibilität → VCL. Für Druck: PDFium/Skia gerendertes Pixel-/DC-Ziel → Windows-Druckspooler; Ausgabe/Spoolerstatus getrennt erfassen.

### 8. BuildEngine-FSM und Paketvertrag

Neue \`<library id="pdfium" ...>\`-Definition in \`BuildEngine-Admin/admin/build-libraries.xml\` mit expliziten \`<dependency>\`-Einträgen einschließlich \`skia/153\`.

**Stages / erwartete Artefakte:**

| Stage | Aktionen | Nachweis |
|---|---|---|
| Source | verifizierte PDFium-/Chromium-GN-Inputs materialisieren | Hashes, Commit-Lock, keine verborgenen Downloads |
| Patch | getrennte GN-/Port-/External-Dependency-Patches prüfen/anwenden | Patch-SHA und \`git apply --check\` |
| Build | \`gn gen\`, Target-Closure validieren, \`ninja\` für Debug und Release | Buildlogs, Linkmap, exakt ein Skia-Paket |
| Test | eigene Smoke-Tests + passende PDFium-Unit-/Embeddertests | CTest/Logzusammenfassung, keine ungeprüften Tests |
| Install | Includes, C++-/Importlibs, notwendige Laufzeit-DLLs, Lizenzinfos, CMake-Paket | install/package-Pfade vollständig |
| Publish | \`install/Win64x\` als Consumer-SDK mit \`find_package(PDFium CONFIG REQUIRED)\` | Targets / transitive Dependencies / Konsumenten-Smoke |
| StackBuilder | versioniertes Paket mit Binärdateien, Patch-/Dependency-Lock, Lizenzmetadaten | Archiv reproduzierbar |
| Bootstrap | Paket- und transitive Abhängigkeiten prüfen, laden, nach ThirdParty integrieren | Auflösung über vorhandenes XML-/Lock-System |

Vorgeschlagener Paketname: \`pdfium/<revision-or-version>\` gemäß bestehender BuildEngine-Namensregeln. Der \`PDFium::PDFium\`-Targetname ist ein **Vorschlag** und wird mit dem bestehenden CMake-Publish-Modell abgeglichen. Ein Debug-/Release-Paket darf niemals eine Skia-DLL der falschen Variante beziehen.

### 9. Tests und Abnahmekriterien

**T0 – isolierte neue Bibliotheken:** jede fehlende Bibliothek mit BCC64X bauen bzw. Header-Smoke; reale Paket-Dependencies, Tests, Lizenzen, Paket und Bootstrap nachweisen.

**T1 – GN-Erzeugung:** PDFium \`gn gen\` reproduzierbar, absolut keine impliziten Source- oder Tooldownloads, keine unaufgelösten Chromium-GN-Imports. \`gn desc\` liefert erwartete Closure und **keine gebaute interne Skia-Bibliothek**.

**T2 – nativer C++-Link:** Consumer inkludiert native PDFium-C++-Header, instanziiert/lädt ein Dokument und linkt ausschließlich gegen veröffentlichte BCC64X-BuildEngine-Pakete; Skia-Version und Buildtyp nachweisbar.

**T3 – Rendering:** synthetische PDF mit Text/Unicode/Umlauten, Rotation, Transparenz, Bildern, Font-Subsets, ICC- und JPEG2000-Beispielen; 72/150/300 dpi und gerenderte Bildabmessungen, visuelle Goldens und Pixel-Diffs Skia gegen AGG bei definierten Toleranzen. PDFium AGG dient **nur als Vergleich**; eine separate AGG-Installation ist nicht nötig.

**T4 – AcroForm:** Textfelder, mehrzeilige Felder, Unicode, Checkboxen, Radio-/Choice-Felder mit PoDoFo füllen; gespeicherte Werte **und** sichtbare Darstellung mit PDFium validieren; gefülltes Formular drucken. XFA ohne V8 muss eindeutig als nicht unterstützt erkannt werden.

**T5 – Druck und GUI:** VCL-Viewer mit Dokument-/Seiten-RAII, Zoom, Scrollen und Seitenrotation; Testdrucker/PDF-Ausgabedrucker und Seitengeometrie inklusive Rand, Duplex/Skalierung; Spooler-Übergabe nicht mit physisch erfolgreichem Ausdruck verwechseln.

**T6 – Robustheit/Sicherheit:** kaputte PDFs, Passwort/geschützte Dokumente, leere Seiten, Extremgrößen, große Dateien, Ressourcenlimits, ungültige Fonts; kontrollierte Fehler, keine Leaks/Use-after-free. Fuzzing optional auf verfügbarer kompatibler Hosttoolchain als zusätzliche Sicherheitsprüfung.

**T7 – Reproduzierbarkeit:** Clean-Build zweimal mit denselben Inputs, Hash-Vergleich der normalisierbaren Artefakte, Byteidentität soweit BuildEngine-Policy verlangt; Debug/Release-CI auf Windows, Linux später gesondert. Lizenz-SBOM deckt tatsächlich gelinkte/runtime-beladene Pakete ab.

**Gate-Auswertung:** Roter Test oder nicht geklärte Lizenz = kein Publish. Testumfang nicht stillschweigend auf „kompiliert“ verkürzen.

### 10. Lizenzmanagement

- PDFium: Upstream-\`LICENSE\` enthält BSD-artige und Apache-2.0-Texte; konkrete Datei-/Komponentenlizenzen im finalen Source-Snapshot prüfen. Alle Notices mitliefern.
- Skia 153: bestehende BuildEngine-\`licenseSignal="yellow"\` respektieren; gelinkte Skia-Komponenten samt Unterlizenzdateien erneut bewerten.
- Abseil: Apache-2.0; fast_float: mehrere permissive Lizenzoptionen, konkreten Paket-Lizenzpfad dokumentieren; LittleCMS: MIT; OpenJPEG: BSD-2-Clause; simdutf: MIT/Apache-2.0; FP16: MIT.
- FreeType-Lizenzoption dokumentieren; ICU-/Unicode-Notices, libjpeg-turbo- und übrige Bildcodec-Notices nicht übersehen.
- Getrennte Betrachtung von **Host-Buildtools** und **ausgelieferten Runtime-Binärkomponenten**. Bei Apache 2.0 NOTICE-/Patenthinsweise prüfen; Downloadberechtigung allein ist keine Lizenzfreigabe.
- Revisionsabhängige Lizenzübersicht und SBOM aus dem tatsächlich verwendeten GN-Graph generieren und mit BuildEngine-Ampel abgleichen. Keine GPL-/AGPL-Verschleppung.

### 11. Umsetzungspakete in Reihenfolge

1. **Inventur & Revisionen (Gate 0):** PDFium-Kandidaten, kompatibler Skia-153-Pin, exakt benötigte GN- und C++-Dependencies, Lizenz-Closure.
2. **Zuerst fehlende Pakete:** \`fast_float\`, \`abseil\`, \`lcms2\`, \`openjpeg\`; \`simdutf\`/\`fp16\` nach Reachability-Prüfung; jeweils vollständige BuildEngine-FSM einschließlich Smoke.
3. **PDFium Source/Prepare:** deterministische, verifizierte PDFium-/Chromium-GN-Inputs; keine gclient-Hooks oder nachgeladenen Compiler.
4. **BCC64X GN-Toolchain:** vorhandene Skia-Erfahrungen übertragen, Warn-/Flag-Probleme separat beheben.
5. **External-Dependencies-GN:** Paket-Mapping für vorhandene und neu integrierte Bibliotheken, Windows ohne \`pkg-config\`.
6. **External-Skia-GN:** Skia 153 SDK, DLL-/Export-/ABI-Konfiguration, nachweislich keine erneute Skia-Kompilation.
7. **Nativer PDFium-Compile/Link:** erst Basis-C++-Targets, dann kompletter PDFium-Kern mit Skia+AGG, Debug/Release.
8. **C++23-Consumer & Visual-Test:** Render, Form, Viewer, Druck; erst synthetische Dokumente; keine vertraulichen Kunden-PDFs im Repository.
9. **Publish/Bootstrap/StackBuilder:** CMake-Targets, Lizenzdashboard, Lock, SBOM, Paketarchive, reproduzierbare CI.
10. **Optionalprofile:** Linux, V8/XFA, Rust/Fontations nur nach separatem Genehmigungs- und Lizenzprozess.

### 12. Risiken und klare Stop-Kriterien

| Risiko | Vorbeugung / Stop |
|---|---|
| Skia-153-API weicht von PDFiums Pin ab | passende PDFium-Revision oder versionierter Kompatibilitätspatch; kein zweiter Skia-Build |
| PDFium bindet Skia-Implementierung intern ein | GN-Closure/GN-Adapter prüfen; Skia-Sources im PDFium-Build = Stop |
| Chromium-GN erwartet MSVC/Chromium-Clang-Flags | BCC64X-GN-Patch wie bei Skia; keine ungeprüfte Toolchain-Substitution |
| Externe lcms2-/OpenJPEG-Version weicht vom PDFium-Fork ab | PDF-ICC-/JPX-Regressionstests; nur nach API/Funktionsgleichheit freigeben |
| Skia-DLL-/CRT-/Exportkonflikt | native C++-Link-/Runtime-Tests, Symbolkontrolle und gleicher Compiler/ABI |
| Build zieht verborgene Pakete nach | isolierte Offline-Buildtests, Source-/Hash-Lock |
| Natives PDFium-C++ ist intern instabil | eigene versionierte adecc-C++23-Fassade, Consumer-Smoke pro Revision |
| PDF-Formularwerte werden nicht sichtbar | Appearance-/Widgettest und Testdruck vor produktiver Freigabe |
| Lizenz-Widerspruch | Lizenzampel rot, Build/Publish blockiert |

### 13. Erfüllungsnachweis / Definition of Done

PDFium gilt erst als **in BuildEngine integriert**, wenn:

- der PDFium-/Skia-/Drittbibliotheks-Pin revisionssicher dokumentiert ist;
- fehlende allgemein nützliche Libraries als vollständige eigenständige Pakete eingebunden sind;
- PDFium Debug/Release aus reproduzierbaren Inputs **mit BCC64X** gebaut wurde;
- exakt **eine** Skia-153-Installation benutzt und nachweislich nicht neu kompiliert wurde;
- nativer C++23-Consumer, Rendering mit Skia, kontrollierte AcroForm-/Drucktests laufen;
- GN-/Build-/Consumer-Tests, Lizenzdashboard, Notices, SBOM, Install/Publish, Bootstrap und StackBuilder bestanden sind;
- Fehlerfälle, Einschränkungen (v.a. XFA ohne V8) und alle Port-Patches dokumentiert sind.

### 14. Quellen / am Umsetzungsstart erneut pinnen

- PDFium upstream: https://pdfium.googlesource.com/pdfium/
- PDFium DEPS: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/DEPS
- PDFium GN-Optionen: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/pdfium.gni
- PDFium Haupt-Targets: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/BUILD.gn
- PDFium Skia-GN: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/skia/BUILD.gn
- PDFium externe Bibliotheken: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/third_party/BUILD.gn
- PDFium Lizenz: https://pdfium.googlesource.com/pdfium/+/refs/heads/main/LICENSE
- Abseil: https://github.com/abseil/abseil-cpp
- fast_float: https://github.com/fastfloat/fast_float
- LittleCMS 2: https://pdfium.googlesource.com/pdfium/+/HEAD/third_party/lcms/LICENSE
- OpenJPEG: https://github.com/uclouvain/openjpeg
- simdutf: https://github.com/simdutf/simdutf
- FP16: https://github.com/Maratyszcza/FP16

**Wichtig:** \`refs/heads/main\`-URLs sind Recherchebelege, aber **keine reproduzierbaren Build-Pins**. Vor der ersten Implementierung Commit-SHA, DEPS-Closure, Versions-/Lizenzstatus und Upstream-Quellen auf einen einheitlichen Snapshot festlegen.


### 15. Verbindliche Präzisierung: externe DLLs und eingefrorener Source-Snapshot (2026-10-16)

Diese Festlegung hat Vorrang vor älteren Passagen dieses Entwurfs, soweit diese
für bereits verfügbare Bibliotheken nur eine Quellcode-Einbindung vorsehen.

**BuildEngine-Pakete zuerst:** Sofern ein vorhandenes Paket für BCC64X als DLL
bereitgestellt ist und die PDFium-GN-Targets dafür kompatibel angepasst werden
können, wird dessen installierte DLL mit Importbibliothek und öffentlichen
Headern verwendet. Eine weitere Kopie desselben Upstream-Quellcodes unter
PDFium ist keine automatische Ausweichlösung. Zunächst sind exportierte Symbole,
Compiler-/CRT-Kompatibilität, Threading, Debug/Release-Konfiguration und
Funktionalität nachzuweisen. Bei einer unvereinbaren PDFium-spezifischen
Quellvariante wird diese Abweichung dokumentiert und separat entschieden.
Nicht jedes vorhandene Paket besitzt eine DLL; ein statisches oder header-only
Paket darf nicht als DLL ausgegeben werden.

**Unveränderliche DEPS-Auflösung:** Ein konkreter PDFium-Commit und sämtliche
benötigten internen DEPS-Revisionspins werden vor dem Source-Schritt festgelegt.
Die Source-Phase lädt ausschließlich diese vorab identifizierten Quellen über
BuildEngine, prüft SHA-256 und erwartete Verzeichnisstruktur und legt das
vollständige GN-Eingabeverzeichnis an. Die Python/GN/Ninja-Orchestrierung folgt
dem bereits erfolgreich eingesetzten Skia-Verfahren, ohne Skia selbst zu ändern.

**Netzwerksperre nach Source:** Ab Abschluss der Source-Bereitstellung dürfen
`gn gen`, Ninja, Python-Hilfsskripte, Tests, Install, Doxygen und Publish
keine Quellen, Tools oder Module nachladen oder synchronisieren. Kein
`gclient sync`, kein `git-sync-deps`, keine automatischen Git-, CIPD-,
FetchContent- oder Update-Hooks in diesen Phasen. Fehlt eine Komponente,
muss der Lauf mit einem konkreten fehlenden Pin/Artefakt fehlschlagen,
statt sie im Hintergrund herunterzuladen. Ein Offline-Wiederholungstest
dieser Phasen ist ein verpflichtender Akzeptanztest.

**Zuordnung und SBOM:** Zu jeder tatsächlich verwendeten Komponente dokumentieren
wir entweder den externen BuildEngine-Paketkoordinaten samt DLL/Importlib
oder den intern gepinnten PDFium-DEPS-Quellstand. Intern eingebundene
Komponenten (z. B. PDFiums AGG 2.3, gegebenenfalls PartitionAlloc) werden
wie bei Skia über `sbom/bundled-components.json` in die CycloneDX-SBOM
und die Lizenznachweise integriert. Eine heruntergeladene, deaktivierte
Komponente darf nicht als ausgelieferte Runtime-Abhängigkeit erscheinen.
Die tatsächlichen GN-Targets müssen diese Klassifikation bestätigen.

**Keine Bestandsinvalidierung:** Die neu anzulegende PDFium-Konfiguration
darf keine Zeitstempel, Definitionen oder FSM-Nachweise der bisher
erfolgreich gebauten 56 Pakete verändern.
