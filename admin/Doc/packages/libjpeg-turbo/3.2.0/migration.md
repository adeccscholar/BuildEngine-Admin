# libjpeg-turbo 3.2.0: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `libjpeg-turbo@3.2.0`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- libjpeg-turbo 3.2.0 selects NSIS templates only for MSVC or MinGW. BCC64X is neither, so the Windows installer section otherwise attempts to read win/projectTargets-release.cmake.in. Suppress only that unsupported installer; upstream install(EXPORT) stays.
- SIMD remains deliberately separate from the first BCC64X proof. NASM is available in the tool contract and can be enabled as the next controlled extension after the baseline has passed.
- Upstream regression tests depend on the command line tools.

## Abhängigkeiten

- Nicht explizit im lokalen Vertrag aufgeführt.

## Quellbeschaffung

- `https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/{LibraryVersion}/libjpeg-turbo-{LibraryVersion}.tar.gz`

## Versionsgebundene Patches und Adapter

- `libjpeg-turbo\3.2.0\bcc64x-skip-unsupported-installer.patch`
- `{LibraryVersion}\bcc64x-skip-unsupported-installer.patch`

## Build-Parameter und Einstellungen

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DENABLE_SHARED:BOOL=ON`
- `-DENABLE_STATIC:BOOL=OFF`
- `-DWITH_SIMD:BOOL=OFF`
- `-DWITH_TURBOJPEG:BOOL=ON`
- `-DWITH_TOOLS:BOOL={TestsEnabled}`
- `-DWITH_TESTS:BOOL={TestsEnabled}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

### Build-Umgebung

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`

### Technische Arbeitsschritte

- `bcc64x-installer-patch-check`
- `bcc64x-installer-patch-apply`
- `bcc64x-installer-patch-verify`
- `test`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
