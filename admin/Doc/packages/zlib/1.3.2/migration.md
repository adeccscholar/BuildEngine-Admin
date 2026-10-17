# zlib 1.3.2: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="zlib" version="1.3.2">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Pin the zlib 1.3.2 upstream CTest fix. Its DLL PATH setup excludes BCC64X unless WIN32 is explicitly supported.
- Clear obsolete static-artifact project include cached by the previous zlib configuration.
- CMake resolves the resource compiler through its BCC64X platform integration.
- zlib appends /zlib to CMAKE_INSTALL_DOCDIR itself.
- Product contract: shared zlib DLL plus its import library.
- Remove the configuration-specific product directories so old static artifacts cannot survive a shared-only rebuild.
- The production contract is shared-only. Upstream therefore runs the shared-library and matching package-integration tests; static-only coverage/tests are not part of this product shape. Nested CMake integration tests reuse Ninja through PATH. CMAKE_PREFIX_PATH points at zlib's configuration-specific test installation so find_package(... COMPONENTS shared) can locate the generat…

## Versionierte Abhängigkeiten

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/madler/zlib/releases/download/v{LibraryVersion}/zlib132.zip`

**Archivnamen:**

- `zlib-{LibraryVersion}.zip`

## Patches, Workarounds und Adapter

- `zlib\1.3.2\bcc64x-ctest-runtime-path.patch`
- `{LibraryVersion}\bcc64x-ctest-runtime-path.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_PROJECT_INCLUDE:FILEPATH=`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_INSTALL_DOCDIR:STRING=share/doc`
- `-DZLIB_BUILD_SHARED:BOOL=ON`
- `-DZLIB_BUILD_STATIC:BOOL=OFF`
- `-DZLIB_BUILD_TESTING:BOOL={TestsEnabled}`
- `-DZLIB_INSTALL:BOOL=ON`
- `-DZLIB_PREFIX:BOOL=OFF`
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

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `PATH={ToolDir:ninja};{ENV:PATH}`
- `CMAKE_PREFIX_PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\test_install\lib\win64\{Configuration}\cmake\zlib`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `ctest-runtime-path-check`
- `ctest-runtime-path-apply`
- `ctest-runtime-path-verify`
- `remove-installed-bin`
- `remove-installed-lib`
- `remove-test-install`
- `test`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-ctest-runtime-path.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/zlib/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
