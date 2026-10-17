# libtiff 4.7.2: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `libtiff@4.7.2`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- Weitere historische Begründungen sind [nicht verifiziert] und müssen bei Bedarf anhand von Commit- und Buildlog-Evidence ergänzt werden.

## Abhängigkeiten

- `zlib 1.3.2`
- `libjpeg-turbo 3.2.0`

## Quellbeschaffung

- `https://download.osgeo.org/libtiff/tiff-{LibraryVersion}.tar.xz`

## Versionsgebundene Patches und Adapter

- Nicht explizit im lokalen Vertrag aufgeführt.

## Build-Parameter und Einstellungen

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-Dtiff-static:BOOL=OFF`
- `-Dtiff-tools:BOOL=OFF`
- `-Dtiff-tests:BOOL=OFF`
- `-Dtiff-contrib:BOOL=OFF`
- `-Dtiff-docs:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{InstallRoot}\packages\libjpeg-turbo\3.2.0`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

### Build-Umgebung

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

### Technische Arbeitsschritte

- Nicht explizit im lokalen Vertrag aufgeführt.

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
