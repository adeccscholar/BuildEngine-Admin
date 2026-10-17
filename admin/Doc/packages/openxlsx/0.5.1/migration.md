# openxlsx 0.5.1: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="openxlsx" version="0.5.1">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Diagnose the nested CMake package/type probe without fetching dependencies.
- Keep the upstream CMake export relative to its original install prefix.

## Versionierte Abhängigkeiten

- `pugixml 1.16`
- `libzip 1.11.4`
- `boost 1.92.0`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/troldal/OpenXLSX/archive/refs/tags/v{LibraryVersion}.tar.gz`

**Archivnamen:**

- `openxlsx-{LibraryVersion}.tar.gz`

## Patches, Workarounds und Adapter

- `openxlsx\0.5.1\forward-managed-boost-dirs.patch`
- `{LibraryVersion}\forward-managed-boost-dirs.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_CXX_STANDARD:STRING=23`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DOPENXLSX_CREATE_DOCS:BOOL=OFF`
- `-DOPENXLSX_BUILD_SAMPLES:BOOL=OFF`
- `-DOPENXLSX_BUILD_TESTS:BOOL=OFF`
- `-DOPENXLSX_BUILD_BENCHMARKS:BOOL=OFF`
- `-DOPENXLSX_ENABLE_LIBZIP:BOOL=ON`
- `-DOPENXLSX_NOWIDE_STANDALONE:BOOL=OFF`
- `-DOPENXLSX_LOCAL_PACKAGES_ONLY:BOOL=ON`
- `-DUSE_SYSTEM_LIBS:BOOL=ON`
- `-DFETCH_DEPS_AUTO:BOOL=OFF`
- `-DTestPackage_LOG_LEVEL:STRING=DEBUG`
- `-DCMAKE_MESSAGE_LOG_LEVEL:STRING=VERBOSE`
- `-DFORCE_FETCH_ALL:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:STRING={InstallRoot}\packages\boost\1.92.0\lib\win64\{Configuration}\cmake\boost_nowide-1.92.0-shared;{InstallRoot}\packages\boost\1.92.0\lib\win64\{Configuration}\cmake;{InstallRoot}\packages\pugixml\1.16\lib\win64\{Configuration}\cmake;{InstallRoot}\packages\libzip\1.11.4\lib\win64\{Configuration}\cmake`
- `-Dboost_nowide_DIR:PATH={InstallRoot}\packages\boost\1.92.0\lib\win64\{Configuration}\cmake\boost_nowide-1.92.0-shared`
- `-DBoost_DIR:PATH={InstallRoot}\packages\boost\1.92.0\lib\win64\{Configuration}\cmake\Boost-1.92.0`
- `-DLibZip_DIR:PATH={InstallRoot}\packages\libzip\1.11.4\lib\win64\{Configuration}\cmake\libzip`
- `-Dpugixml_DIR:PATH={InstallRoot}\packages\pugixml\1.16\lib\win64\{Configuration}\cmake\pugixml`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DOpenXLSX_DIR:PATH={InstallRoot}\packages\openxlsx\0.5.1\lib\win64\Release\cmake\OpenXLSX`
- `-DBoost_DIR:PATH={InstallRoot}\packages\boost\1.92.0\lib\win64\Release\cmake\Boost-1.92.0`
- `-DLibZip_DIR:PATH={InstallRoot}\packages\libzip\1.11.4\lib\win64\Release\cmake\libzip`
- `-Dpugixml_DIR:PATH={InstallRoot}\packages\pugixml\1.16\lib\win64\Release\cmake\pugixml`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={InstallRoot}\packages\openxlsx\0.5.1\bin\win64\Release;{InstallRoot}\packages\boost\1.92.0\bin\win64\Release;{InstallRoot}\packages\libzip\1.11.4\bin\win64\Release;{InstallRoot}\packages\pugixml\1.16\bin\win64\Release;{ENV:PATH}`

**Technische Execute-Actions:**

- `boost-dirs-patch-check`
- `boost-dirs-patch-apply`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\forward-managed-boost-dirs.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.md`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/openxlsx/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
