# abseil 20260817.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="abseil" version="20260817.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Keine zusätzlichen erklärenden Sonderentscheidungen aus den XML-Kommentaren extrahiert. [nicht verifiziert], ob weitere historische Entscheidungen bestehen.

## Versionierte Abhängigkeiten

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/abseil/abseil-cpp/archive/refs/tags/20260817.0.tar.gz`

**Archivnamen:**

- `abseil-cpp-20260817.0.tar.gz`

## Patches, Workarounds und Adapter

- `abseil\20260817.0\bcc64x-win32-cctz.patch`
- `{LibraryVersion}\bcc64x-win32-cctz.patch`
- `abseil\20260817.0\bcc64x-charconv-digits10.patch`
- `{LibraryVersion}\bcc64x-charconv-digits10.patch`
- `abseil\20260817.0\bcc64x-dll-import-export.patch`
- `{LibraryVersion}\bcc64x-dll-import-export.patch`
- `abseil\20260817.0\bcc64x-dll-export-all.patch`
- `{LibraryVersion}\bcc64x-dll-export-all.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_TESTING:BOOL=OFF`
- `-DABSL_BUILD_TESTING:BOOL=OFF`
- `-DABSL_BUILD_TEST_HELPERS:BOOL=OFF`
- `-DABSL_ENABLE_INSTALL:BOOL=ON`
- `-DABSL_PROPAGATE_CXX_STD:BOOL=ON`
- `-DABSL_BUILD_MONOLITHIC_SHARED_LIBS:BOOL=ON`
- `-DABSL_USE_GOOGLETEST_HEAD:BOOL=OFF`
- `-DCMAKE_CXX_STANDARD:STRING=20`
- `-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-Dabsl_DIR:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\Release\cmake\absl`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `patch-check`
- `patch-apply`
- `digits10-check`
- `digits10-apply`
- `dll-import-export-check`
- `dll-import-export-apply`
- `dll-import-export-verify`
- `dll-export-all-check`
- `dll-export-all-apply`
- `dll-export-all-verify`
- `digits10-verify`
- `patch-verify`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-win32-cctz.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-charconv-digits10.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-dll-import-export.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-dll-export-all.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses\abseil\LICENSE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/abseil/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
