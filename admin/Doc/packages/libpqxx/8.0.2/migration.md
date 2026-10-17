# libpqxx 8.0.2: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="libpqxx" version="8.0.2">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- BCC64X Release links ThreadTest against the Xerces DLL import library, but HandlerBase is an exported polymorphic class whose virtual member implementations are inline in the upstream header. Debug links, while Release expects the HandlerBase vtable from libxerces-c.lib and reports it undefined. For the BCC64X/Clang path move HandlerBase::characters(), the first virtual member …

## Versionierte Abhängigkeiten

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/jtv/libpqxx/archive/refs/tags/{LibraryVersion}.tar.gz`
- `https://downloads.apache.org/xerces/c/3/sources/xerces-c-{LibraryVersion}.zip`

**Archivnamen:**

- `libpqxx-{LibraryVersion}.tar.gz`
- `xerces-c-{LibraryVersion}.zip`

## Patches, Workarounds und Adapter

- `libpqxx\8.0.2\bcc64x-empty-declspec.patch`
- `{LibraryVersion}\bcc64x-empty-declspec.patch`
- `xerces-c\3.3.0\bcc64x-handlerbase-vtable.patch`
- `{LibraryVersion}\bcc64x-handlerbase-vtable.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_TEST:BOOL=OFF`
- `-DBUILD_EXAMPLES:BOOL=OFF`
- `-DBUILD_TOOLS:BOOL=OFF`
- `-DBUILD_DOC:BOOL=OFF`
- `-DCMAKE_CXX_STANDARD:STRING=23`
- `-DPostgreSQL_INCLUDE_DIR:PATH={InstallRoot}\packages\libpq\18.6\include`
- `-DPostgreSQL_LIBRARY:FILEPATH={InstallRoot}\packages\libpq\18.6\lib\win64\{Configuration}\libpq.lib`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-Dlibpqxx_DIR:PATH={ConsumerRoot}\lib\win64\{Configuration}\cmake\libpqxx`
- `-DPostgreSQL_LIBRARY:FILEPATH={InstallRoot}\packages\libpq\18.6\lib\win64\Release\libpq.lib`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-Dnetwork-accessor:STRING=winsock`
- `-Dtranscoder:STRING=windows`
- `-Dmessage-loader:STRING=inmemory`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DDOXYGEN_EXECUTABLE:FILEPATH={Tool:doxygen}`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={InstallRoot}\packages\libpq\18.6\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={InstallRoot}\packages\libpq\18.6\bin\win64\Release;{DependencyRoot:openssl}\bin\win64\Release;{InstallRoot}\packages\zlib\1.3.2\bin\win64\Release;{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src;{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `empty-declspec-patch-check`
- `empty-declspec-patch-apply`
- `patch-check`
- `patch-apply`
- `build-product`
- `build-thread-test`
- `build-upstream-validation`
- `test`
- `remove-installed-bin`
- `remove-installed-lib`
- `create-api-docs`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-empty-declspec.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-handlerbase-vtable.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\NOTICE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/libpqxx/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
