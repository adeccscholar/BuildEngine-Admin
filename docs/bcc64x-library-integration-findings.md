# BCC64X-Integrationsbefunde aktueller Third-Party-Bibliotheken

Stand: 8. September 2026  
Zielplattform: RAD Studio 13 Florence / C++Builder 13 / BCC64X 20.1.7 / Embarcadero C++ 7.80 / Win64 Modern

## Zweck

Dieses Dokument bündelt technische Erkenntnisse aus den zuletzt integrierten Bibliotheken und aus der Skia-Integration. Es ergänzt den autoritativen Vertrag in `admin/build-libraries.xml`, ersetzt ihn aber nicht.

Der Schwerpunkt liegt auf reproduzierbaren Befunden:

- Was ließ sich mit BCC64X unverändert bauen?
- Wo waren paket-, Buildsystem- oder Compiler-spezifische Anpassungen notwendig?
- Welche Anpassungen gehören in den Bibliotheksvertrag, welche in einen versionsgebundenen Patch und welche in eine generische BuildEngine-/Toolchain-Lösung?
- Welche Probleme sind echte Upstream-/Packaging-Probleme und welche sind BCC64X-spezifisch?

Grundregel des Projekts bleibt: kein stiller Wechsel auf MSVC, clang-cl, MinGW oder eine andere Toolchain. BCC64X ist der zu untersuchende Compilerpfad.

## Zusammenfassung

Die aktuellen Bibliotheken zeigen drei verschiedene Klassen von Integrationsproblemen:

1. **normale Paketierung:** Bibliothek baut mit BCC64X, benötigt aber einen sauberen Install-/Publish-/Smoke-Vertrag;
2. **Upstream-Buildsystem- oder Paketierungsdefizit:** Quellcode ist kompatibel, aber Export-, Install- oder Dependency-Metadaten sind unvollständig;
3. **BCC64X-spezifische Semantik:** Buildsystem oder Quellcode trifft Annahmen über Compilerfamilie, Linker oder Zeichensatz, die für den modernen BCC64X-Pfad nicht gelten.

Die wichtigste architektonische Konsequenz ist, diese Ebenen nicht zu vermischen. Bibliotheksspezifische Workarounds bleiben lokal; generische Compiler-/Linkerprobleme werden einmal zentral gelöst.

---

## libjpeg-turbo 3.2.0

Der aktuelle Vertrag baut libjpeg-turbo als Shared Library mit dem originalen CMake-Buildsystem.

Wesentliche Einstellungen:

- Version `3.2.0`;
- Shared Build aktiv;
- statische Bibliotheken deaktiviert;
- TurboJPEG aktiv;
- SIMD im derzeitigen Vertrag deaktiviert;
- Upstream-Tools und Upstream-Tests im derzeitigen Vertrag deaktiviert;
- Release und Debug werden getrennt installiert;
- ein paketbezogener Consumer-Smoke existiert.

Für Skia wird nicht die mit Skia gebündelte libjpeg-turbo-Kopie verwendet. Skia erhält die von BuildEngine bereitgestellte Version `3.2.0` als explizite Abhängigkeit.

### GN/BCC64X-Erkenntnis

Skias GN-Integration kann absolute BCC64X-Importbibliotheken nicht in jedem Fall sinnvoll über GN `libs` transportieren. Für den BCC64X-Systempfad wird die konkrete Importbibliothek deshalb als roher Linkerparameter über `ldflags` weitergegeben.

Das ist kein libjpeg-Quellcodepatch, sondern eine Anpassung von Skias Third-Party-GN-Vertrag an den BCC64X-Linkerpfad.

---

## libpng 1.6.58

Der aktuelle Vertrag verwendet:

- libpng `1.6.58`;
- zlib `1.3.2` als explizite BuildEngine-Abhängigkeit;
- Shared Build;
- statische Bibliothek deaktiviert;
- Release/Debug getrennt;
- paketbezogenen Consumer-Smoke.

Der Source-Download ist mit SHA-256 gepinnt.

### Aktueller Teststatus im Vertrag

`PNG_TESTS` ist im derzeitigen Bibliotheksvertrag deaktiviert. Das ist als aktueller Vertragszustand dokumentiert, nicht als generelle Projektregel. Die Projektregel bleibt, Upstream-Tests nicht ohne technische Begründung zu deaktivieren. In der für dieses Dokument ausgewerteten Repository-Evidenz ist keine separate abschließende Begründung für die Deaktivierung festgehalten.

### Skia-Systemintegration

Skia verwendet die BuildEngine-libpng und baut nicht zusätzlich seine eigene libpng-Kopie.

Für BCC64X wurde in Skias `third_party/libpng/BUILD.gn` der Pfad zur konkreten BuildEngine-Importbibliothek von GN `libs` auf `ldflags` umgestellt. Dadurch bleibt der vollständige Importbibliothekspfad erhalten und wird nicht von GN in eine für diesen Fall ungeeignete `-l...`-Form überführt.

Diese Erkenntnis wiederholt sich bei weiteren Systembibliotheken und ist daher ein wichtiger GN/BCC64X-Integrationsbefund.

---

## libtiff 4.7.2

Der aktuelle Vertrag verwendet:

- libtiff `4.7.2`;
- zlib `1.3.2`;
- libjpeg-turbo `3.2.0`;
- Shared Build;
- statische Bibliothek deaktiviert;
- Tools, Contrib, Dokumentation und Upstream-Tests im aktuellen Vertrag deaktiviert;
- Release/Debug getrennt;
- paketbezogenen Consumer-Smoke.

Die installierten Public Header `tiffio.h` und `tiff.h` sind explizite Require-Gates.

### Einordnung

libtiff selbst benötigte in dem aktuellen Vertrag keinen vergleichbaren tiefen Compilerpatch wie Skia. Der wesentliche Integrationswert liegt hier im reproduzierbaren Abhängigkeits- und Paketvertrag und in der Consumer-Verifikation.

Wie bei libpng gilt: Die deaktivierten Upstream-Tests sind der dokumentierte aktuelle Vertragszustand. Eine gesonderte technische Begründung ist in der hier ausgewerteten Repository-Evidenz nicht abschließend festgehalten und sollte bei der nächsten Test-Policy-Bereinigung erneut bewertet werden.

---

## HarfBuzz 14.4.0

HarfBuzz ist für Skia eine echte externe BuildEngine-Abhängigkeit und wird nicht innerhalb des Skia-Builds neu heruntergeladen oder gebaut.

Der Vertrag ist bewusst so geschnitten, dass keine zyklische Abhängigkeit zu FreeType entsteht:

- HarfBuzz baut ohne FreeType;
- optionale Cairo-, Graphite2-, GLib-, ICU-, GObject-, GDI-, Uniscribe- und DirectWrite-Pfade sind für dieses Paket deaktiviert;
- Core Shaping und Subset sind aktiv;
- Raster/Vector/GPU/Utilities sind für diesen Vertrag deaktiviert.

### Upstream-CMake-Paketproblem: Threads

Der von HarfBuzz erzeugte CMake-Export kann `Threads::Threads` in seiner öffentlichen Link-Schnittstelle enthalten, ohne dass der generierte Package-Config-Vertrag selbst `find_dependency(Threads)` ausführt.

Die Lösung im Admin-Repository verändert nicht den HarfBuzz-Quellcode:

1. der generierte Upstream-Export bleibt erhalten;
2. er wird als `harfbuzzTargets.cmake` installiert;
3. ein kleiner BuildEngine-Admin-Package-Wrapper wird als `harfbuzzConfig.cmake` davor gesetzt und stellt die notwendige Dependency-Auflösung her.

Das ist ein Packaging-Fix, kein BCC64X-Sprachkompatibilitätspatch.

### Fehlender öffentlicher Subset-Header

HarfBuzz 14.4.0 installiert über den community-maintained CMake-Pfad `hb-subset.h` und `hb-subset-serialize.h`, lässt aber `hb-subset-depend.h` aus, obwohl `hb-subset.h` diesen Header benötigt.

Der Meson-Vertrag enthält den Header in der öffentlichen Subset-Headerliste. BuildEngine ergänzt deshalb beim Installieren genau diesen fehlenden Header. Der Consumer-Smoke wird nicht abgeschwächt.

### Skia-Systemintegration

Analog zu libpng werden die HarfBuzz-Importbibliotheken im BCC64X-Skia-Pfad als rohe `ldflags` übergeben. Das gilt auch für die Subset-Bibliothek, wenn PDF-Subsetting aktiv ist.

---

## FreeType 2.14.3

FreeType wird als Shared Library mit einer bewusst vollständigen externen Dependency-Kette gebaut:

- zlib `1.3.2`;
- bzip2 `1.0.8`;
- Brotli `1.2.0`;
- libpng `1.6.58`;
- HarfBuzz `14.4.0`.

Die Abhängigkeiten werden nicht nur über CMake-Suche opportunistisch gefunden, sondern im Buildvertrag explizit aktiviert und gefordert.

Wichtig ist insbesondere die Richtung der HarfBuzz-/FreeType-Abhängigkeit:

- HarfBuzz wird zunächst ohne FreeType erzeugt;
- FreeType wird anschließend mit HarfBuzz erzeugt.

Damit bleibt der Buildgraph azyklisch.

Der FreeType-Consumer-Smoke wurde außerdem so erweitert, dass die HarfBuzz-Integration tatsächlich nachgewiesen wird und nicht nur ein isoliertes FreeType-Binary geladen wird.

### Runtime-Kette

FreeType zieht insbesondere bzip2 als Runtime-Abhängigkeit nach. Für Package-Smokes darf deshalb nicht nur der direkte Dependency-Pfad berücksichtigt werden. BuildEngine löst inzwischen die transitive Runtime-Dependency-Kette rekursiv auf und stellt die benötigten DLL-Verzeichnisse im Smoke-PATH bereit.

---

## Skia 153 / Source-Pin `2eed75b956045eb8603d3690a1e84bc582a2135d`

Skia ist die bisher komplexeste Integrationsprobe dieses Abschnitts.

Der sichtbare Paketstand verwendet die logische Version `153`; der reproduzierbare Source-Pin bleibt der konkrete Commit

`2eed75b956045eb8603d3690a1e84bc582a2135d`.

Der Source-Download und die Patchpfade sind weiterhin an diesen Commit gebunden.

### Externe BuildEngine-Abhängigkeiten

Skia verwendet explizit:

- OpenGL `26.2.1`;
- zlib `1.3.2`;
- Brotli `1.2.0`;
- libjpeg-turbo `3.2.0`;
- libpng `1.6.58`;
- HarfBuzz `14.4.0`;
- FreeType `2.14.3`.

Diese Bibliotheken werden nicht noch einmal innerhalb von Skia gebaut. Dadurch bleiben Version, Lizenz- und SBOM-Beziehung im BuildEngine-Graph sichtbar.

Transitive Abhängigkeiten, z. B. bzip2 über FreeType, werden ebenfalls über den BuildEngine-Dependency-Graph behandelt und nicht in das Skia-Paket kopiert.

### Gebündelte Skia-Komponenten

Andere Skia-Third-Party-Komponenten bleiben bewusst an die von diesem Skia-Commit gewählten Revisionen gebunden. Dazu gehören unter anderem ICU, WebP, libavif, libgav1, JPEG XL, Wuffs, DNG SDK, Expat, SPIRV-Cross, SPIRV-Tools, glslang, Vulkan-Headers sowie die Vulkan-/D3D-Memory-Allocatoren.

Diese gebündelten Komponenten werden getrennt von den BuildEngine-Abhängigkeiten erfasst. Die Metadatenpipeline übernimmt die ermittelten gebündelten Komponenten in die erzeugte CycloneDX-SBOM. Ein verifizierter Skia-Metadatenlauf ergab insgesamt 27 Komponenten einschließlich Wurzel, BuildEngine-Abhängigkeitsgraph und gebündelten Komponenten.

### Windows-Desktop-Profil

Der aktuelle Vertrag aktiviert ein breites Windows-Desktop-Profil, darunter:

- CPU Raster / skcms;
- DirectWrite und Windows GDI;
- FreeType;
- ICU, HarfBuzz, SkUnicode und SkShaper;
- PNG, JPEG, WebP, Wuffs;
- AVIF, JPEG XL und DNG;
- SVG/Expat;
- PDF und XPS;
- Skottie/Lottie;
- Ganesh;
- Graphite;
- OpenGL;
- Direct3D 12;
- Vulkan und Vulkan Memory Allocator;
- SPIR-V-Validierung und SPIRV-Cross.

Nicht zum Zielprofil gehören insbesondere Android/iOS/macOS-Integrationen, WebGL/WebGPU/CanvasKit/Emscripten, Dawn, ANGLE, Rust-Codecs/Fontations/ICU4X, FFmpeg, Lua, Perfetto/pprof sowie Entwickler-Viewer/Fuzzer.

### BCC64X-GN-Toolchain

Skias GN-Buildsystem kennt BCC64X nicht nativ. Der versionsgebundene `bcc64x-gn.patch` ergänzt den Win64-Modern-Toolchainpfad.

Wichtige Punkte:

- BCC64X wird als echter Compiler/Linkerpfad verwendet;
- RAD- und Platform-SDK-Library-Verzeichnisse werden als GN-Argumente eingespeist;
- die BCC64X-Solink-Regel erzeugt DLL und Importbibliothek über den BCC64X-Treiber;
- absolute BuildEngine-Importbibliotheken werden, wo notwendig, als rohe Linkerflags propagiert.

### Systembibliotheken statt Skia-Duplikate

Für zlib, Brotli, libjpeg-turbo, libpng, HarfBuzz und FreeType wurden Skias GN-Third-Party-Verträge so erweitert, dass die BuildEngine-Pakete genutzt werden können.

Bei libpng, libjpeg-turbo und HarfBuzz zeigte sich dabei ein gemeinsames Muster: konkrete absolute BCC64X-Importbibliothekspfade müssen als `ldflags` transportiert werden, nicht als normale GN-`libs`-Einträge.

### Component Build und Exportgrenzen

Ein pauschales `--export-all-symbols` war für Skia keine tragfähige Lösung. Der Exportumfang wurde zu groß und überschritt bei breiten Komponenten den sinnvollen PE/COFF-Rahmen.

Die Lösung ist ein hybrides Component-Modell:

Shared BCC64X-Komponenten sind gezielt auf die öffentliche Nutzungsoberfläche begrenzt, insbesondere:

- `skia`;
- `skunicode_core`;
- `skunicode_icu`;
- `skunicode_client_icu`;
- `skunicode_bidi`;
- `skunicode_libgrapheme`;
- `skunicode_icu4x`;
- `skshaper`;
- `svg`;
- `skottie`.

Andere interne `skia_component`-Targets bleiben im BCC64X-Component-Build statisch. `jsonreader` und `sksg` werden als statische Implementierungskomponenten in ihre Consumer eingebunden; eigene DLLs dafür werden nicht erzeugt.

Zusätzlich wird für Skia-eigenen Bibliothekscode gezielt Default Visibility verwendet, während fremder/interner Code nicht pauschal exportiert wird.

### Einzelne Kompatibilitätsstellen

Im Verlauf wurden mehrere konkrete BCC64X-/Windows-Stellen identifiziert und versionsgebunden gelöst, darunter:

- Direct3D-`GetDesc`-Kompatibilität;
- Debug-SPIR-V-Validator-Pfad;
- Windows-Threading in libgav1;
- SkArenaAlloc-Export;
- skcms-DLL-Grenzen;
- zentrale Sichtbarkeits-/Komponentenlogik;
- Systemdependency- und Link-Propagation;
- HarfBuzz-, libpng-, libjpeg-, Brotli- und weitere Systembibliothekspfade.

Die Patchdateien liegen ausschließlich unter dem konkreten Skia-Source-Pin. Sie sind damit nicht als allgemeingültige Patches für beliebige Skia-Versionen zu verstehen.

### UCRT-Problem: `fabsf` und verwandte Symbole

Skia Debug machte einen generischen BCC64X-20.1.7-Runtime-Linkfehler sichtbar. Bestimmte adressierbare C-Math-Symbole wurden aus `api-ms-win-crt-math-l1-1-0.dll` importiert; das Binary linkte, konnte aber mit Windows-Fehler 127 nicht geladen werden.

Der Befund wurde außerhalb von Skia mit Minimalproben reproduziert. Betroffen sind:

- `fabsf`;
- `nextafterl`;
- `nexttoward`;
- `nexttowardf`;
- `nexttowardl`.

Die mit RAD Studio ausgelieferte `libucrt.a` enthält gleichzeitig lokale `libucrt_extra`-Implementierungen. Ein aus genau vier verifizierten Members erzeugtes `libbcc64x-ucrt-compat.a` behebt den Laufzeitfehler, wenn es vor der normalen Runtime aufgelöst wird.

Die vollständige Evidenz steht in `docs/bcc64x-ucrt-runtime-link-bug.md`.

Wichtig: Die Lösung ist **kein Skia-Source-Patch**. BuildEngine erzeugt das Compatibility-Archiv aus der lokal installierten Embarcadero-Runtime über den generierten Toolvertrag `bcc64x-ucrt-compat`. Es werden keine Embarcadero-Objektdateien im Repository verteilt.

Skia nutzt dafür seinen vorhandenen Upstream-Mechanismus `extra_ldflags`, so dass das Compatibility-Archiv vor der normalen Runtime erscheint.

### Smoke- und Runtime-Erkenntnisse

Ein Release-Package-Smoke für Skia wurde mit 17/17 Checks erfolgreich nachgewiesen. Dabei war zusätzlich die rekursive Runtime-Dependency-Auflösung notwendig, damit nicht nur direkte Skia-DLLs, sondern auch transitive Dependency-DLLs gefunden werden.

Der endgültige Debug-Package-Smoke nach allen UCRT-Arbeiten ist in der für dieses Dokument ausgewerteten Evidenz **[nicht verifiziert]**. Diese Aussage soll bei einem späteren vollständigen Lauf durch das reale Log ersetzt werden.

---

## Catch2 3.16.0

Catch2 wurde bewusst als aktuelles Gegenbeispiel zu veralteten Paketständen aufgenommen. Version `3.16.0` ist eine kompilierte Testbibliothek und wird im BuildEngine-Vertrag statisch gebaut. Für ein Testframework ist diese statische Bereitstellung eine bewusst akzeptierte Ausnahme vom üblichen Shared-Library-Default.

Der Upstream-Source-Pin ist Commit

`317ac1ed4c0bb6e6b91eafc817e05c488feffcb3`.

Der Build verwendet C++23 und behält die Upstream-SelfTests aktiv.

### BCC64X-Sonderfall 1: veraltete `__BORLANDC__`-Erkennung

Catch2 3.16.0 behandelt jedes definierte `__BORLANDC__` als historischen Embarcadero-Compilerpfad und aktiviert dadurch `CATCH_CONFIG_POLYFILL_ISNAN`.

Dieser Pfad ruft `std::_isnan()` auf. BCC64X 20.1.7 stellt `std::_isnan` nicht bereit; der Build scheitert in `catch_polyfills.cpp`.

Catch2 selbst stellt den negativen Konfigurationsschalter `CATCH_CONFIG_NO_POLYFILL_ISNAN` bereit. Deshalb wird kein Source-Patch verwendet, sondern der Buildvertrag setzt:

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
```

Damit bauen Release und Debug erfolgreich.

### BCC64X-Sonderfall 2: Execution Code Page für Narrow Strings

Nach dem erfolgreichen Build blieb der Upstream-SelfTest `RunTests` rot. Der eigentliche nicht erwartete Fehler lag ausschließlich im Test `XmlEncode: UTF-8`.

Catch2 enthält echte UTF-8-Zeichen direkt in normalen `char`-Stringliteralen, z. B. `š` und `👾`, und erwartet deren UTF-8-Bytefolge.

Maschinelle Minimalproben ergaben für `š`:

```text
BCC64X default                              -> 9A
-finput-charset=UTF-8                       -> 9A
-finput-charset=UTF-8 -fexec-charset=UTF-8  -> 9A
UTF-8-Quelldatei mit BOM                    -> 9A
\u0161 im ASCII-Quelltext                   -> C5 A1
```

Die vom real installierten BCC64X 20.1.7 selbst ausgewiesenen Embarcadero-spezifischen Optionen sind:

```text
-fborland-input-code-page=<code page>
-fborland-exec-code-page=<code page>
```

Die Gegenproben ergaben:

```text
-fborland-input-code-page=65001                           -> 9A
-fborland-exec-code-page=65001                            -> C5 A1
-fborland-input-code-page=65001 -fborland-exec-code-page=65001 -> C5 A1
```

Damit ist die Ursache klar: Für Catch2 ist nicht die Source-Codepage, sondern die **Execution Code Page normaler Narrow-String-Literale** entscheidend.

Der Catch2-Vertrag setzt deshalb zusätzlich:

```text
-fborland-exec-code-page=65001
```

Die zuvor untersuchten generischen Clang-Optionen `-finput-charset=UTF-8` und `-fexec-charset=UTF-8` gehören nicht in den Catch2-Vertrag.

### Verifikation

Mit den beiden BCC64X-spezifisch notwendigen Einstellungen

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
-fborland-exec-code-page=65001
```

lief der gezielt wiederholte Upstream-Test erfolgreich:

```text
1/1 Test #1: RunTests ... Passed
100% tests passed, 0 tests failed out of 1
```

Vor der Codepage-Korrektur hatte derselbe SelfTest 531 Testfälle; 507 waren regulär erfolgreich, 16 schlugen erwartungsgemäß fehl, 7 wurden übersprungen und genau ein echter Testfall mit drei echten Assertions schlug fehl. Das half, die absichtlichen Catch2-`[!shouldfail]`-/`[!mayfail]`-Ausgaben von dem realen UTF-8-Problem zu trennen.

Die Anpassung benötigt keinen Catch2-Source-Patch.

### Architekturfolgerung

`-fborland-exec-code-page=65001` wird zunächst **nur für Catch2** gesetzt. Eine globale Änderung der BCC64X-Toolchain würde die Semantik normaler Narrow-String-Literale sämtlicher Bibliotheken verändern und ist daher eine eigene Toolchain-Policy-Entscheidung.

---

## BitmapPlusPlus 1.1.1

BitmapPlusPlus wurde als moderne, kleine C++-BMP-Bibliothek aufgenommen, statt einen historischen EasyBMP-Stand zu reproduzieren.

Wesentliche Eigenschaften:

- Version `1.1.1`;
- MIT-Lizenz;
- Header-only;
- Upstream definiert nur ein CMake-INTERFACE-Target;
- Upstream besitzt keine `install()`-/Package-Config-Regeln.

BuildEngine erfindet deshalb keinen künstlichen Build. Der Vertrag verwendet einen direkten Install-Pfad:

- Header kopieren;
- Lizenz kopieren;
- kleinen BuildEngine-Admin-CMake-Package-Config bereitstellen;
- Release- und Debug-Consumer-Smokes mit BCC64X/C++23.

Der Consumer-Smoke schreibt ein BMP, lädt es erneut und prüft Geometrie und Pixelwerte. Damit wird echte Header-/Compile-/Runtime-Nutzbarkeit geprüft und nicht nur `#include`.

---

## Generische BuildEngine-Erkenntnisse aus dieser Integrationsrunde

### 1. Transitive Runtime-Abhängigkeiten gehören in Package-Smokes

Ein Package-Smoke darf nicht nur die direkten DLL-Verzeichnisse einer Bibliothek in `PATH` aufnehmen. FreeType -> bzip2 und ähnliche Ketten haben gezeigt, dass die Runtime-Closure rekursiv aus dem BuildEngine-Abhängigkeitsgraphen bestimmt werden muss.

BuildEngine löst diese transitive Runtime-Kette inzwischen für Package-Smokes auf.

### 2. Gebündelte Komponenten müssen in die SBOM

Bei Skia reicht der BuildEngine-Dependency-Graph allein nicht aus, weil ein Teil der Third-Party-Komponenten bewusst von Skia selbst gepinnt und mitgebaut wird.

Die Metadata-Pipeline kann deshalb zusätzlich eine ermittelte Liste gebündelter Source-Komponenten aufnehmen. Dadurch bleiben externe BuildEngine-Pakete und eingebettete Upstream-Komponenten getrennt sichtbar.

### 3. Toolchain-Probleme nicht als Bibliothekspatch verstecken

Der BCC64X-UCRT-Fehler wurde durch Skia entdeckt, aber außerhalb Skias reproduziert. Deshalb gehört die Lösung in die generische BCC64X-Tool-/Linkschicht und nicht in Skia-Quellcode.

Das gleiche Prinzip gilt umgekehrt für Catch2s `__BORLANDC__`-Autodetection: Das ist Catch2-spezifisch und bleibt im Catch2-Vertrag.

### 4. Keine geschätzten Patches

Versionsgebundene Patches werden nur aus exakt materialisiertem Upstream-Zustand erzeugt und mit `git apply --check` gegen den vorgesehenen Zustand validiert. Ein Patch wird nicht aus Logausschnitten oder vermutetem Source-Kontext rekonstruiert.

### 5. Upstream-Paketfehler von Sprachkompatibilität trennen

Beispiele:

- HarfBuzz fehlendes `find_dependency(Threads)` bzw. fehlender `hb-subset-depend.h`: Packaging-/Installationsproblem;
- Catch2 `std::_isnan`: veraltete Compilererkennung im Upstream;
- Catch2 UTF-8-Narrow-Literals: BCC64X-Execution-Codepage-Semantik;
- Skia UCRT-Math-Imports: generischer BCC64X-Runtime-/Linkerbefund;
- GN `libs` vs. absolute BCC64X-Importlibs: Buildsystem-/Toolchain-Adapterproblem.

Diese Trennung ist wichtig, damit spätere Upstream-Updates gezielt neu bewertet werden können.

---

## Offene Punkte

- vollständiger erneuter Catch2-BuildEngine-Lauf für Release und Debug einschließlich aller 82 CTest-Einträge, Install, Publish und beider Package-Smokes nach der finalen Execution-Codepage-Einstellung;
- endgültigen Skia-Debug-Package-Smoke nach der UCRT-Lösung mit realem Log nachweisen;
- bei libpng, libtiff und libjpeg-turbo die derzeit deaktivierten Upstream-Tests im Rahmen der Test-Policy erneut bewerten und die technische Entscheidung explizit dokumentieren;
- prüfen, ob UTF-8 als globale BCC64X-Execution-Codepage zukünftig eine bewusste Toolchain-Policy werden soll; bis dahin bleibt `-fborland-exec-code-page=65001` Catch2-lokal;
- Tool-Identitäten und externe lokale Buildinputs zukünftig gezielt in den inkrementellen Library-State aufnehmen, statt nur Library-Timestamps und Dependencies zu berücksichtigen.

## Zugehörige Dokumentation

- `docs/bcc64x-ucrt-runtime-link-bug.md` – reproduzierter BCC64X-UCRT-Link-/Laufzeitfehler;
- `docs/library-license-sbom.md` – kumulierte Lizenzinformationen und CycloneDX-SBOM;
- `admin/build-libraries.xml` – autoritativer Bibliotheks- und Dependency-Vertrag;
- `admin/build-tools.xml` – Tool- und Generated-Tool-Vertrag, einschließlich `bcc64x-ucrt-compat`.
