# xerces-c 3.3.0: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `xerces-c@3.3.0`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- BCC64X Release links ThreadTest against the Xerces DLL import library, but HandlerBase is an exported polymorphic class whose virtual member implementations are inline in the upstream header. Debug links, while Release expects the HandlerBase vtable from libxerces-c.lib and reports it undefined. For the BCC64X/Clang path move HandlerBase::characters(), the first virtual member function, out of the header and into sax/Dummy.cpp. This gives the class an out-of-line key-function candidate in the DLL without changing t

## Abhängigkeiten

- Nicht explizit im lokalen Vertrag aufgeführt.

## Quellbeschaffung

- `https://downloads.apache.org/xerces/c/3/sources/xerces-c-{LibraryVersion}.zip`

## Versionsgebundene Patches und Adapter

- `xerces-c\3.3.0\bcc64x-handlerbase-vtable.patch`
- `{LibraryVersion}\bcc64x-handlerbase-vtable.patch`

## Build-Parameter und Einstellungen

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-Dnetwork-accessor:STRING=winsock`
- `-Dtranscoder:STRING=windows`
- `-Dmessage-loader:STRING=inmemory`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DDOXYGEN_EXECUTABLE:FILEPATH={Tool:doxygen}`

### Build-Umgebung

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src;{ENV:PATH}`

### Technische Arbeitsschritte

- `patch-check`
- `patch-apply`
- `build-product`
- `build-thread-test`
- `build-upstream-validation`
- `test`
- `remove-installed-bin`
- `remove-installed-lib`
- `create-api-docs`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
