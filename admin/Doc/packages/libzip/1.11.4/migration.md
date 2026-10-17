# libzip 1.11.4: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="libzip" version="1.11.4">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Official release asset. Use tar.gz for bootstrap so libzip does not depend on the BuildEngine's not-yet-enabled XZ extraction path.
- Required deflate backend plus the two compression backends already produced by this BuildEngine contract.
- Native Windows AES implementation. Do not introduce an OpenSSL dependency when BCrypt is the upstream-preferred Windows backend.
- Build the upstream command-line tools. Documentation is disabled because libzip's documentation target copies generated HTML back into the shared source tree; parallel Release/Debug builds race on those files. Functional library gates remain enabled below.
- Release and Debug use separate directories. BCC64X/CMake emits libzip.dll and libzip.lib for the upstream target named zip.
- These tests run from the build tree before installation. The working directory contains zip.dll; dependency runtime directories are supplied generically by the BuildEngine.
- Upstream installs the three tools to literal bin rather than CMAKE_INSTALL_BINDIR. Keep that upstream location intact for libzip-bin-targets and additionally provide the configuration- specific package layout used by the BuildEngine.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `xz 5.8.4`
- `zstd 1.5.7`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/nih-at/libzip/releases/download/v{LibraryVersion}/libzip-{LibraryVersion}.tar.gz`

**Archivnamen:**

- `libzip-{LibraryVersion}.tar.gz`

## Patches, Workarounds und Adapter

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DLIBZIP_DO_INSTALL:BOOL=ON`
- `-DSHARED_LIB_VERSIONNING:BOOL=ON`
- `-DZLIB_INCLUDE_DIR:PATH={InstallRoot}\packages\zlib\1.3.2\include`
- `-DZLIB_LIBRARY:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibImportLibrary}`
- `-DENABLE_BZIP2:BOOL=OFF`
- `-DENABLE_LZMA:BOOL=ON`
- `-DLIBLZMA_INCLUDE_DIR:PATH={DependencyRoot:xz}\include`
- `-DLIBLZMA_LIBRARY:FILEPATH={DependencyRoot:xz}\lib\win64\{Configuration}\liblzma.lib`
- `-DENABLE_ZSTD:BOOL=ON`
- `-Dzstd_DIR:PATH={InstallRoot}\packages\zstd\1.5.7\lib\win64\{Configuration}\cmake\zstd`
- `-DENABLE_WINDOWS_CRYPTO:BOOL=ON`
- `-DENABLE_COMMONCRYPTO:BOOL=OFF`
- `-DENABLE_OPENSSL:BOOL=OFF`
- `-DENABLE_GNUTLS:BOOL=OFF`
- `-DENABLE_MBEDTLS:BOOL=OFF`
- `-DENABLE_FDOPEN:BOOL=ON`
- `-DBUILD_TOOLS:BOOL=ON`
- `-DBUILD_REGRESS:BOOL=OFF`
- `-DBUILD_OSSFUZZ:BOOL=OFF`
- `-DBUILD_EXAMPLES:BOOL=ON`
- `-DBUILD_DOC:BOOL=OFF`
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
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `remove-xz-smoke-archive`
- `xz-write`
- `xz-read`
- `remove-zstd-smoke-archive`
- `zstd-write`
- `zstd-read`
- `remove-aes-smoke-archive`
- `aes-write`
- `aes-read`
- `remove-installed-bin`
- `remove-installed-lib`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\zipcmp.exe`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\zipmerge.exe`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\ziptool.exe`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/libzip/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
