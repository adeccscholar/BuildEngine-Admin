# bitmapplusplus 1.1.1: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="bitmapplusplus" version="1.1.1">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- BitmapPlusPlus is a header-only C++ library. Upstream defines only an INTERFACE target and intentionally creates no binary artifact. Therefore this package uses BuildEngine's direct install contract rather than inventing configuration-specific build or install artifacts. BCC64X compatibility is verified independently for Release and Debug by the package consumer smokes below.
- Upstream 1.1.1 has no install()/package-config contract. These files add only package discovery metadata; the upstream header itself remains unmodified.
- libjpeg-turbo 3.2.0 selects NSIS templates only for MSVC or MinGW. BCC64X is neither, so the Windows installer section otherwise attempts to read win/projectTargets-release.cmake.in. Suppress only that unsupported installer; upstream install(EXPORT) stays.
- SIMD remains deliberately separate from the first BCC64X proof. NASM is available in the tool contract and can be enabled as the next controlled extension after the baseline has passed.
- Upstream regression tests depend on the command line tools.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/baderouaich/BitmapPlusPlus/archive/refs/tags/v1.1.1.zip`
- `https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/{LibraryVersion}/libjpeg-turbo-{LibraryVersion}.tar.gz`
- `https://download.sourceforge.net/libpng/libpng-{LibraryVersion}.tar.xz`

**Archivnamen:**

- `bitmapplusplus-{LibraryVersion}.zip`
- `libjpeg-turbo-{LibraryVersion}.tar.gz`
- `libpng-{LibraryVersion}.tar.xz`

## Patches, Workarounds und Adapter

- `libjpeg-turbo\3.2.0\bcc64x-skip-unsupported-installer.patch`
- `{LibraryVersion}\bcc64x-skip-unsupported-installer.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

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
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2`
- `-DPNG_SHARED:BOOL=ON`
- `-DPNG_STATIC:BOOL=OFF`
- `-DPNG_TESTS:BOOL=OFF`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `bcc64x-installer-patch-check`
- `bcc64x-installer-patch-apply`
- `bcc64x-installer-patch-verify`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\BitmapPlusPlus.hpp`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses\bitmapplusplus\LICENSE.md`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\cmake\BitmapPlusPlus\BitmapPlusPlusConfig.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\cmake\BitmapPlusPlus\BitmapPlusPlusConfigVersion.cmake`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-skip-unsupported-installer.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.md`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/bitmapplusplus/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
