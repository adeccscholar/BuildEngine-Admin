# Catch2 3.16.0 mit BCC64X 20.1.7

Stand: 8. September 2026  
Toolchain: RAD Studio 13 Florence / C++Builder 13 / BCC64X 20.1.7 / Embarcadero C++ 7.80 / Win64 Modern

## Status

Catch2 3.16.0 baut mit BCC64X in Release und Debug erfolgreich. Die vollständigen Upstream-CTest-Suites laufen in beiden Varianten erfolgreich durch.

Verifizierter BuildEngine-Lauf:

```text
library:catch2:build:Debug    PASS, 170 Build-Schritte, warnings=0, errors=0
library:catch2:build:Release  PASS, 170 Build-Schritte, warnings=0, errors=0
library:catch2:test:Debug     PASS, 82/82 Upstream-CTest-Einträge
library:catch2:test:Release   PASS, 82/82 Upstream-CTest-Einträge
library:catch2:install:Debug  PASS
library:catch2:install:Release PASS
library:catch2:install:common PASS
library:catch2:install        PASS
library:catch2:metadata       PASS, CycloneDX 1.7, components=1
library:catch2:publish        PASS, 188 Dateien
```

Der verbleibende Fehler des betreffenden Laufs lag ausschließlich in den beiden Package-Consumer-Smokes während `find_package(Catch2 CONFIG)`.

## BCC64X-Anpassung 1: Catch2-`__BORLANDC__`-Erkennung

Catch2 3.16.0 aktiviert bei definiertem `__BORLANDC__` den historischen Polyfill `CATCH_CONFIG_POLYFILL_ISNAN`. Dieser verwendet `std::_isnan()`, das in der modernen BCC64X-20.1.7-Standardbibliotheksumgebung nicht vorhanden ist.

Catch2 stellt selbst den negativen Override bereit:

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
```

Damit wird der nicht passende historische Polyfill deaktiviert, ohne Catch2-Quellcode zu verändern.

## BCC64X-Anpassung 2: Execution Code Page

Catch2s SelfTests enthalten echte UTF-8-Zeichen in normalen Narrow-String-Literalen, unter anderem `š` und `👾`, und testen deren UTF-8-Bytefolge.

Maschinelle Minimalproben mit dem real installierten BCC64X 20.1.7 ergaben für `š`:

```text
Default                                             -> 9A
-finput-charset=UTF-8                              -> 9A
-finput-charset=UTF-8 -fexec-charset=UTF-8         -> 9A
UTF-8-Quelldatei mit BOM                           -> 9A
-fborland-input-code-page=65001                    -> 9A
-fborland-exec-code-page=65001                     -> C5 A1
-fborland-input-code-page=65001
  -fborland-exec-code-page=65001                   -> C5 A1
```

BCC64X weist über `--help` selbst folgende Embarcadero-spezifische Optionen aus:

```text
-fborland-input-code-page=<code page>
-fborland-exec-code-page=<code page>
```

Für Catch2 ist ausschließlich die Execution Code Page relevant. Der Vertrag verwendet deshalb:

```text
-fborland-exec-code-page=65001
```

Mit

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
-fborland-exec-code-page=65001
```

lief der zuvor fehlschlagende Catch2-SelfTest `RunTests` gezielt mit 1/1 PASS. Im anschließenden normalen BuildEngine-Lauf liefen dann in Release und Debug jeweils alle 82 CTest-Einträge erfolgreich durch.

## Package-Smoke: CMake-Config ist variantenspezifisch installiert

Catch2 definiert upstream:

```cmake
set(CATCH_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Catch2")
```

Der BuildEngine-Vertrag verwendet bewusst variantenspezifische Bibliotheksverzeichnisse:

```text
Release: lib/win64/Release
Debug:   lib/win64/Debug
```

Damit werden die unveränderten Catch2-CMake-Pakete hier installiert:

```text
lib/win64/Release/cmake/Catch2
lib/win64/Debug/cmake/Catch2
```

BuildEngines `scope="package"`-Smoke setzt `CMAKE_PREFIX_PATH` auf den Package-Root und `CMAKE_LIBRARY_PATH` auf `lib/win64/<Configuration>`. Ein config-spezifisches Unterverzeichnis `lib/win64/<Configuration>/cmake/Catch2` ist jedoch kein normaler `find_package(... CONFIG)`-Suchpfad unter diesem Prefix.

Deshalb schlugen beide Consumer-Smokes beim `find_package(Catch2 3.16 CONFIG REQUIRED)` fehl, obwohl Build, Tests, Installation und Publish erfolgreich waren.

### Gewählte Korrektur

Der Package-Smoke erhält explizit den unveränderten, tatsächlich installierten Catch2-Config-Pfad:

```text
-DCatch2_DIR:PATH={ConsumerRoot}\lib\win64\{Configuration}\cmake\Catch2
```

Das ist bewusst besser als die generierten Catch2-Exportdateien nachträglich in ein anderes Verzeichnis zu kopieren: CMake-Exportdateien enthalten relocationsbezogene Import-Prefix-Logik, die an ihrem ursprünglichen Installationslayout verbleiben soll.

Die Korrektur betrifft ausschließlich den Catch2-Package-Smoke. Catch2-Quellcode und BuildEngine-Kern müssen dafür nicht geändert werden.

## Architekturentscheidung

Catch2 bleibt statisch. Für ein Testframework ist dies eine bewusst akzeptierte Ausnahme vom generellen Shared-Library-Default.

Die beiden BCC64X-Compile-Einstellungen bleiben Catch2-lokal. Insbesondere wird `-fborland-exec-code-page=65001` nicht ohne separate Toolchain-Policy-Entscheidung global für alle Bibliotheken gesetzt.

## Source-Pin

Version: `3.16.0`  
Tag: `v3.16.0`  
Commit: `317ac1ed4c0bb6e6b91eafc817e05c488feffcb3`

Es werden keine Catch2-Source-Patches benötigt.
