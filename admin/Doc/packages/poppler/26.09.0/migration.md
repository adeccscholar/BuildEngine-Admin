# poppler 26.09.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="poppler" version="26.09.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Poppler's Win32 backend includes the Microsoft-style Windows SDK headers. BCC64X identifies the target with _WIN64/__MINGW64__, while winnt.h requires the SDK architecture selector _AMD64_ on x64. Keep this bridge local to Poppler; the proven generic BCC64X toolchain remains unchanged.
- Upstream release tarballs do not contain poppler/test-data. Test integration stays explicit and will be added when that separate upstream data repository becomes a pinned BuildEngine participant.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `freetype 2.14.3`
- `libjpeg-turbo 3.2.0`
- `libpng 1.6.58`
- `libtiff 4.7.2`
- `win-iconv 0.0.8`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://poppler.freedesktop.org/poppler-{LibraryVersion}.tar.xz`

**Archivnamen:**

- `poppler-{LibraryVersion}.tar.xz`

## Patches, Workarounds und Adapter

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-D_AMD64_`
- `-DNOMINMAX`
- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF`
- `-DFONT_CONFIGURATION:STRING=win32`
- `-DENABLE_CPP:BOOL=ON`
- `-DENABLE_UNSTABLE_API_ABI_HEADERS:BOOL=ON`
- `-DENABLE_UTILS:BOOL=OFF`
- `-DENABLE_GLIB:BOOL=OFF`
- `-DENABLE_QT5:BOOL=OFF`
- `-DENABLE_QT6:BOOL=OFF`
- `-DENABLE_BOOST:BOOL=OFF`
- `-DENABLE_BROTLI:BOOL=OFF`
- `-DENABLE_LIBOPENJPEG:BOOL=OFF`
- `-DENABLE_LCMS:BOOL=OFF`
- `-DENABLE_LIBCURL:BOOL=OFF`
- `-DENABLE_NSS3:BOOL=OFF`
- `-DENABLE_GPGME:BOOL=OFF`
- `-DENABLE_LIBJPEG:BOOL=ON`
- `-DENABLE_LIBTIFF:BOOL=ON`
- `-DBUILD_GTK_TESTS:BOOL=OFF`
- `-DBUILD_QT5_TESTS:BOOL=OFF`
- `-DBUILD_QT6_TESTS:BOOL=OFF`
- `-DBUILD_CPP_TESTS:BOOL=OFF`
- `-DBUILD_MANUAL_TESTS:BOOL=OFF`
- `-DRUN_GPERF_IF_PRESENT:BOOL=OFF`
- `-DENABLE_RELOCATABLE:BOOL=ON`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DIconv_INCLUDE_DIR:PATH={InstallRoot}\packages\win-iconv\0.0.8\include`
- `-DIconv_LIBRARY:FILEPATH={InstallRoot}\packages\win-iconv\0.0.8\lib\win64\Release\libiconv.lib`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DIconv_LIBRARY:FILEPATH={InstallRoot}\packages\win-iconv\0.0.8\lib\win64\Debug\libiconv.lib`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CXXFLAGS=-D_AMD64_ -DNOMINMAX`

**Technische Execute-Actions:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`

**Consumer-Smokes:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Consumer: Lizenzentscheidung

Die GPL-Lizenzbewertung ist eine eigenständige Freigabeschranke für die Verwendung in proprietären Produkten, unabhängig von der technischen Übersetzbarkeit.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/poppler/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
