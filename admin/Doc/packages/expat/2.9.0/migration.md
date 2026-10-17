# expat 2.9.0: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `expat@2.9.0`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- Expat assumes that every non-MSVC test build needs a POSIX shell. BCC64X is a native Windows Clang toolchain: the unit-test executable can and should be started directly on Windows.

## Abhängigkeiten

- Nicht explizit im lokalen Vertrag aufgeführt.

## Quellbeschaffung

- `https://github.com/libexpat/libexpat/releases/download/R_2_9_0/expat-{LibraryVersion}.tar.xz`

## Versionsgebundene Patches und Adapter

- `expat\2.9.0\native-windows-tests.patch`
- `{LibraryVersion}\native-windows-tests.patch`

## Build-Parameter und Einstellungen

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DEXPAT_SHARED_LIBS:BOOL=ON`
- `-DEXPAT_BUILD_TESTS:BOOL={TestsEnabled}`
- `-DEXPAT_BUILD_TOOLS:BOOL=ON`
- `-DEXPAT_BUILD_EXAMPLES:BOOL=ON`
- `-DEXPAT_BUILD_DOCS:BOOL=OFF`
- `-DEXPAT_BUILD_PKGCONFIG:BOOL=ON`
- `-DEXPAT_BUILD_FUZZERS:BOOL=OFF`
- `-DEXPAT_ENABLE_INSTALL:BOOL=ON`
- `-DEXPAT_WARNINGS_AS_ERRORS:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

### Build-Umgebung

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`

### Technische Arbeitsschritte

- `native-windows-tests-patch-check`
- `native-windows-tests-patch-apply`
- `native-windows-tests-patch-verify`
- `test`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
