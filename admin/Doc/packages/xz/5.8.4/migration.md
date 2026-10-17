# xz 5.8.4: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `xz@5.8.4`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- BCC64X must emit liblzma.dll, not libliblzma.dll.
- Matches XZ's own Windows CMake build guidance: translations are disabled, while the full liblzma API, tools, docs, and tests remain.

## Abhängigkeiten

- Nicht explizit im lokalen Vertrag aufgeführt.

## Quellbeschaffung

- `https://github.com/tukaani-project/xz/releases/download/v{LibraryVersion}/xz-{LibraryVersion}.tar.gz`

## Versionsgebundene Patches und Adapter

- `xz\5.8.4\bcc64x-windres-package-name.patch`
- `{LibraryVersion}\bcc64x-windres-package-name.patch`
- `xz\5.8.4\bcc64x-liblzma-dll-name.patch`
- `{LibraryVersion}\bcc64x-liblzma-dll-name.patch`

## Build-Parameter und Einstellungen

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_TESTING:BOOL={TestsEnabled}`
- `-DXZ_NLS:BOOL=OFF`
- `-DXZ_DOC:BOOL=ON`
- `-DXZ_TOOL_XZ:BOOL=ON`
- `-DXZ_TOOL_XZDEC:BOOL=ON`
- `-DXZ_TOOL_LZMADEC:BOOL=ON`
- `-DXZ_TOOL_LZMAINFO:BOOL=ON`
- `-DXZ_EXTERNAL_SHA256:BOOL=OFF`
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
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration};{ENV:PATH}`

### Technische Arbeitsschritte

- `windres-patch-check`
- `windres-patch-apply`
- `windres-patch-verify`
- `dll-name-patch-check`
- `dll-name-patch-apply`
- `dll-name-patch-verify`
- `test`
- `remove-installed-bin`
- `remove-installed-lib`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
