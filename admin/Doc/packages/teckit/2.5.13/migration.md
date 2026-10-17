# teckit 2.5.13: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="teckit" version="2.5.13">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Native Windows/BCC64X build adapter. TECkit's upstream source inventory is small and explicitly supports Windows. The upstream Autotools files remain the source of truth for target membership, but no Bash/Autoconf/Automake/Libtool path is used. The adapter builds the same two libraries and three command-line tools directly with CMake/Ninja/BCC64X. Library dependencies are packa…

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `expat 2.9.0`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/silnrsi/teckit/releases/download/v{LibraryVersion}/teckit-{LibraryVersion}.tar.xz`

**Archivnamen:**

- `teckit-{LibraryVersion}.tar.xz`

## Patches, Workarounds und Adapter

- `teckit\2.5.13\fix-sfconv-byte-ambiguity.patch`
- `{LibraryVersion}\fix-sfconv-byte-ambiguity.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DTECKIT_SOURCE_DIR:PATH={Workspace}`
- `-DZLIB_DIR:PATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\cmake\zlib`
- `-Dexpat_DIR:PATH={DependencyRoot:expat}\lib\win64\{Configuration}\cmake\expat-{DependencyVersion:expat}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `BINDIR={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin`
- `SRCDIR={Workspace}\test`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin;{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{DependencyRoot:expat}\bin\win64\{Configuration};{ENV:PATH}`

**Technische Execute-Actions:**

- `sfconv-byte-patch-check`
- `sfconv-byte-patch-apply`
- `test`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-sfconv-byte-ambiguity.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`

**Consumer-Smokes:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/teckit/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
