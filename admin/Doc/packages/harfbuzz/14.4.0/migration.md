# harfbuzz 14.4.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="harfbuzz" version="14.4.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- HarfBuzz' CMake export contains Threads::Threads in the public link interface when CMake detects pthread support. The upstream generated package file does not call find_dependency(Threads), so preserve the generated export as harfbuzzTargets.cmake and install a small package wrapper as harfbuzzConfig.cmake.
- HarfBuzz 14.4.0's community-maintained CMake installation installs hb-subset.h and hb-subset-serialize.h but omits hb-subset-depend.h even though hb-subset.h includes it. Meson's public subset-header list contains all three. Complete the installed CMake package here rather than weakening the consumer smoke test.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `bzip2 1.0.8`
- `brotli 1.2.0`
- `libpng 1.6.58`
- `harfbuzz 14.4.0`
- `zlib 1.3.2`
- `libjpeg-turbo 3.2.0`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/harfbuzz/harfbuzz/releases/download/{LibraryVersion}/harfbuzz-{LibraryVersion}.tar.xz`
- `https://sourceforge.net/projects/freetype/files/freetype2/{LibraryVersion}/freetype-{LibraryVersion}.tar.xz/download`
- `https://download.osgeo.org/libtiff/tiff-{LibraryVersion}.tar.xz`

**Archivnamen:**

- `harfbuzz-{LibraryVersion}.tar.xz`
- `freetype-{LibraryVersion}.tar.xz`
- `tiff-{LibraryVersion}.tar.xz`

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
- `-DHB_HAVE_FREETYPE:BOOL=OFF`
- `-DHB_HAVE_CAIRO:BOOL=OFF`
- `-DHB_HAVE_GRAPHITE2:BOOL=OFF`
- `-DHB_HAVE_GLIB:BOOL=OFF`
- `-DHB_HAVE_ICU:BOOL=OFF`
- `-DHB_HAVE_GOBJECT:BOOL=OFF`
- `-DHB_HAVE_INTROSPECTION:BOOL=OFF`
- `-DHB_HAVE_UNISCRIBE:BOOL=OFF`
- `-DHB_HAVE_GDI:BOOL=OFF`
- `-DHB_HAVE_DIRECTWRITE:BOOL=OFF`
- `-DHB_BUILD_SUBSET:BOOL=ON`
- `-DHB_BUILD_RASTER:BOOL=OFF`
- `-DHB_BUILD_VECTOR:BOOL=OFF`
- `-DHB_BUILD_GPU:BOOL=OFF`
- `-DHB_BUILD_GPU_DEMO:STRING=OFF`
- `-DHB_BUILD_UTILS:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DFT_DISABLE_ZLIB:BOOL=OFF`
- `-DFT_REQUIRE_ZLIB:BOOL=ON`
- `-DFT_DISABLE_BZIP2:BOOL=OFF`
- `-DFT_REQUIRE_BZIP2:BOOL=ON`
- `-DFT_DISABLE_PNG:BOOL=OFF`
- `-DFT_REQUIRE_PNG:BOOL=ON`
- `-DFT_DISABLE_BROTLI:BOOL=OFF`
- `-DFT_REQUIRE_BROTLI:BOOL=ON`
- `-DFT_DISABLE_HARFBUZZ:BOOL=OFF`
- `-DFT_DYNAMIC_HARFBUZZ:BOOL=OFF`
- `-DFT_REQUIRE_HARFBUZZ:BOOL=ON`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{InstallRoot}\packages\bzip2\1.0.8;{InstallRoot}\packages\brotli\1.2.0;{InstallRoot}\packages\libpng\1.6.58;{InstallRoot}\packages\harfbuzz\14.4.0`
- `-Dtiff-static:BOOL=OFF`
- `-Dtiff-tools:BOOL=OFF`
- `-Dtiff-tests:BOOL=OFF`
- `-Dtiff-contrib:BOOL=OFF`
- `-Dtiff-docs:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{InstallRoot}\packages\libjpeg-turbo\3.2.0`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\harfbuzz\harfbuzzTargets.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\harfbuzz\harfbuzzConfig.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\harfbuzz\hb-subset-depend.h`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.TXT`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.md`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/harfbuzz/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
