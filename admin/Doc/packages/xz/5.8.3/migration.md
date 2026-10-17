# xz 5.8.3: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="xz" version="5.8.3">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

**Versionshinweis:** Im Vertrag als ersetzt durch `5.8.4` gekennzeichnet.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- BCC64X must emit liblzma.dll, not libliblzma.dll.
- Matches XZ's own Windows CMake build guidance: translations are disabled, while the full liblzma API, tools, docs, and tests remain.

## Versionierte Abhängigkeiten

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/tukaani-project/xz/releases/download/v{LibraryVersion}/xz-{LibraryVersion}.tar.gz`

**Archivnamen:**

- `xz-{LibraryVersion}.tar.gz`

## Patches, Workarounds und Adapter

- `xz\5.8.3\bcc64x-windres-package-name.patch`
- `{LibraryVersion}\bcc64x-windres-package-name.patch`
- `xz\5.8.3\bcc64x-liblzma-dll-name.patch`
- `{LibraryVersion}\bcc64x-liblzma-dll-name.patch`
- `xz\5.8.4\bcc64x-windres-package-name.patch`
- `xz\5.8.4\bcc64x-liblzma-dll-name.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

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

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `windres-patch-check`
- `windres-patch-apply`
- `windres-patch-verify`
- `dll-name-patch-check`
- `dll-name-patch-apply`
- `dll-name-patch-verify`
- `test`
- `remove-installed-bin`
- `remove-installed-lib`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-windres-package-name.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-liblzma-dll-name.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING.0BSD`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING.GPLv2`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/xz/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
