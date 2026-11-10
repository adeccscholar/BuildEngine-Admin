# Tesseract OCR 5.5.4

[TOC|Content]

## Aktueller BuildEngine-Vertrag (10. November 2026)

**Vertragsdaten, keine bestätigte Build-Ausführung.** Ergebnisse aus früheren Tests sind versions- und buildgebunden; neue Ausführungen bleiben **[nicht verifiziert]**.

Dynamically linked C++ OCR engine; language traineddata is provisioned and licensed separately.

**Koordinate:** `tesseract@5.5.4` · **Kategorie:** `text` · **Lizenzampel:** `yellow`

### Deklarierte Abhängigkeiten (vollständig transitiv)

Die Diagrammkanten stammen aus den exakt gepinnten XML-Abhängigkeiten. Gestrichelte Kanten kennzeichnen Erweiterungen, durchgezogene Kanten Paketabhängigkeiten. Upstream-intern gebündelte Komponenten sind keine zusätzlichen BuildEngine-Koordinaten.

```mermaid
flowchart LR
   N0["tesseract 5.5.4"]
   N1["leptonica 1.87.0"]
   N2["libjpeg-turbo 3.2.1"]
   N3["libpng 1.6.59"]
   N4["libtiff 4.7.2"]
   N5["zlib 1.4"]
   N0 --> N1
   N1 --> N5
   N1 --> N3
   N1 --> N2
   N1 --> N4
   N3 --> N5
   N4 --> N5
   N4 --> N2
```

**Direkte Abhängigkeiten:** `leptonica@1.87.0`

### Konfiguration und Besonderheiten

**CMake-Argumente:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/{Configuration}`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_TRAINING_TOOLS:BOOL=OFF`
- `-DBUILD_TESTS:BOOL=OFF`
- `-DOPENMP_BUILD:BOOL=OFF`
- `-DENABLE_NATIVE:BOOL=OFF`
- `-DENABLE_PRECOMPILED_HEADERS:BOOL=OFF`
- `-DENABLE_CCACHE:BOOL=OFF`
- `-DDISABLE_TIFF:BOOL=ON`
- `-DDISABLE_ARCHIVE:BOOL=ON`
- `-DDISABLE_CURL:BOOL=ON`
- `-DLeptonica_DIR:PATH={DependencyRoot:leptonica}\lib\win64\{Configuration}\cmake\leptonica`
- `-DCMAKE_PREFIX_PATH:PATH={DependencyRoot:leptonica}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2 -DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`

**Build-Umgebung:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Versionsgebundene Patches:**

- `{Workspace}\.buildengine\patches\bcc64x-runtime-install.patch`
- `{BuildFileDir}\patches\tesseract\5.5.4\bcc64x-runtime-install.patch`

**Source-Downloads:**

- `https://github.com/tesseract-ocr/tesseract/archive/7e548e6eff172f38f97664afe7f7ce40db36c8d6.zip`
- `https://raw.githubusercontent.com/tesseract-ocr/tessdata_fast/87416418657359cb625c412a48b6e1d6d41c29bd/eng.traineddata` (SHA-256: `7d4322bd2a7749724879683fc3912cb542f19906c83bcc1a52132556427170b2`)

**Deklarierte Smoke-Tests:**

- `consumer` / `Release` / `package` / `smokes\tesseract\consumer`
- `ocr` / `Release` / `package` / `smokes\tesseract\ocr`

**Explizit geforderte Installationsartefakte:**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\tesseract\TesseractConfig.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\tesseract\baseapi.h`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\share\tessdata\eng.traineddata`

**Verbindliche Quellen:** `admin/build-libraries.xml` für Konfiguration und Pins; FSM/Buildlogs für tatsächliche Ausführung. [Bibliotheksverzeichnis](/manual/libraries.md).
