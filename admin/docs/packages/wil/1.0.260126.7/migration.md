# Windows Implementation Library (WIL) 1.0.260126.7

[TOC|Content]

## Aktueller BuildEngine-Vertrag (10. November 2026)

**Vertragsdaten, keine bestätigte Build-Ausführung.** Ergebnisse aus früheren Tests sind versions- und buildgebunden; neue Ausführungen bleiben **[nicht verifiziert]**.

Microsoft MIT-licensed header-only Windows COM and RAII library. BCC64X compatibility requires a native consumer smoke before use by OpenCV.

**Koordinate:** `wil@1.0.260126.7` · **Kategorie:** `foundation` · **Lizenzampel:** `yellow`

### Deklarierte Abhängigkeiten (vollständig transitiv)

Die Diagrammkanten stammen aus den exakt gepinnten XML-Abhängigkeiten. Gestrichelte Kanten kennzeichnen Erweiterungen, durchgezogene Kanten Paketabhängigkeiten. Upstream-intern gebündelte Komponenten sind keine zusätzlichen BuildEngine-Koordinaten.

```mermaid
flowchart LR
   N0["wil 1.0.260126.7"]
```

**Direkte Abhängigkeiten:** Keine deklariert.

### Konfiguration und Besonderheiten

**CMake-Argumente:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_INSTALL_DATADIR:STRING=share`
- `-DWIL_BUILD_PACKAGING:BOOL=OFF`
- `-DWIL_BUILD_TESTS:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`

**Build-Umgebung:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Versionsgebundene Patches:**

- `{Workspace}\.buildengine\patches\bcc64x-sdk-atomics.patch`
- `{BuildFileDir}\patches\wil\1.0.260126.7\bcc64x-sdk-atomics.patch`

**Source-Downloads:**

- `https://github.com/microsoft/wil/archive/cbf677fb0a942557d08fd129f4c106a76247b2ec.zip`

**Deklarierte Smoke-Tests:**

- `consumer` / `Release` / `package` / `smokes\wil\consumer`

**Explizit geforderte Installationsartefakte:**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\wil\com.h`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\share\cmake\WIL\wilConfig.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\ThirdPartyNotices.txt`

**Verbindliche Quellen:** `admin/build-libraries.xml` für Konfiguration und Pins; FSM/Buildlogs für tatsächliche Ausführung. [Bibliotheksverzeichnis](/manual/libraries.md).
