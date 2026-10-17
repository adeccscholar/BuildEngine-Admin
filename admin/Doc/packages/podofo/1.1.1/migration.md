# podofo 1.1.1: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="podofo" version="1.1.1">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- In shared builds, XMP tests use the DLL without re-linking internal archives. The existing upstream tests retain their original private-library access.
- Repair the DLL-only test target without adding private static archives.
- PoDoFo sources and upstream tests contain UTF-8 characters in ordinary narrow string literals. BCC64X 20.1.7 requires the Embarcadero execution-code-page switch for UTF-8 literal bytes. This was machine-verified previously for Catch2; generic Clang -finput-charset/-fexec-charset did not change BCC64X behaviour.
- Keep the tested runtime closure deterministic. In particular, Release libz.dll must come from our zlib 1.3.2 package.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `openssl 3.5.9`
- `freetype 2.14.3`
- `libxml2 2.15.3`
- `libjpeg-turbo 3.2.0`
- `libpng 1.6.58`
- `libtiff 4.7.2`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/podofo/podofo/archive/refs/tags/{LibraryVersion}.tar.gz`

**Archivnamen:**

- `podofo-{LibraryVersion}.tar.gz`

## Patches, Workarounds und Adapter

- `podofo\1.1.1\bcc64x-charconv-route.patch`
- `{LibraryVersion}\bcc64x-charconv-route.patch`
- `podofo\1.1.1\bcc64x-catch-isnan-route.patch`
- `{LibraryVersion}\bcc64x-catch-isnan-route.patch`
- `podofo\1.1.1\bcc64x-cid-order.patch`
- `{LibraryVersion}\bcc64x-cid-order.patch`
- `podofo\1.1.1\bcc64x-test-output-isolation.patch`
- `{LibraryVersion}\bcc64x-test-output-isolation.patch`
- `podofo\1.1.1\bcc64x-xmp-dll-boundary.patch`
- `{LibraryVersion}\bcc64x-xmp-dll-boundary.patch`
- `podofo\1.1.1\bcc64x-xmp-dll-link-fix.patch`
- `{LibraryVersion}\bcc64x-xmp-dll-link-fix.patch`
- `podofo\1.1.1\bcc64x-dll-export.patch`
- `{LibraryVersion}\bcc64x-dll-export.patch`
- `podofo\1.1.1\bcc64x-test-resource-isolation.patch`
- `{LibraryVersion}\bcc64x-test-resource-isolation.patch`
- `podofo\1.1.1\bcc64x-xmp-schema-lifetime.patch`
- `{LibraryVersion}\bcc64x-xmp-schema-lifetime.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_CXX_FLAGS:STRING=-fborland-exec-code-page=65001`
- `-DPODOFO_BUILD_STATIC:BOOL=OFF`
- `-DPODOFO_BUILD_TEST:BOOL={TestsEnabled}`
- `-DPODOFO_BUILD_EXAMPLES:BOOL=OFF`
- `-DPODOFO_BUILD_UNSUPPORTED_TOOLS:BOOL=OFF`
- `-DPODOFO_WITH_AFDKO:BOOL=OFF`
- `-DPODOFO_WITH_FONTMANAGER:BOOL=ON`
- `-DPODOFO_WITH_WIN32GDI_FONT_SEARCH:BOOL=ON`
- `-DPODOFO_WITH_LCMS2:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{DependencyRoot:openssl};{InstallRoot}\packages\freetype\2.14.3;{InstallRoot}\packages\libxml2\2.15.3;{InstallRoot}\packages\libjpeg-turbo\3.2.0;{InstallRoot}\packages\libpng\1.6.58;{InstallRoot}\packages\libtiff\4.7.2`
- `-DPDF_TEST_RESOURCE_PATH:PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\resources`
- `-DPDF_TEST_OUTPUT_PATH:PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\out`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DLIBXML2_LIBRARY:FILEPATH={InstallRoot}\packages\libxml2\2.15.3\lib\win64\Release\libxml2.lib`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DLIBXML2_LIBRARY:FILEPATH={InstallRoot}\packages\libxml2\2.15.3\lib\win64\Debug\libxml2d.lib`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DPoDoFo_DIR:PATH={ConsumerRoot}\lib\win64\{Configuration}\cmake\podofo`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CTEST_PARALLEL_LEVEL=1`
- `PODOFO_TEST_OUTPUT_PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\test\out`
- `PATH={InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\freetype\2.14.3\bin\win64\{Configuration};{InstallRoot}\packages\libxml2\2.15.3\bin\win64\{Configuration};{InstallRoot}\packages\libjpeg-turbo\3.2.0\bin\win64\{Configuration};{InstallRoot}\packages\libpng\1.6.58\bin\win64\{Configuration};{InstallRoot}\packages\libtiff\4.7.2\bin\win64\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `charconv-patch-check`
- `charconv-patch-apply`
- `charconv-patch-verify`
- `catch-isnan-patch-check`
- `catch-isnan-patch-apply`
- `catch-isnan-patch-verify`
- `cid-order-patch-check`
- `cid-order-patch-apply`
- `test-output-patch-check`
- `test-output-patch-apply`
- `xmp-dll-patch-check`
- `xmp-dll-patch-apply`
- `xmp-dll-patch-verify`
- `xmp-dll-link-patch-check`
- `xmp-dll-link-patch-apply`
- `xmp-dll-link-patch-verify`
- `dll-export-patch-check`
- `dll-export-patch-apply`
- `resources-clone`
- `resources-archive`
- `resources-directory`
- `resources-extract`
- `test-resource-patch-check`
- `test-resource-patch-apply`
- `test-resource-patch-verify`
- `xmp-lifetime-patch-check`
- `xmp-lifetime-patch-apply`
- `xmp-lifetime-patch-verify`
- `isolate-test-resources`
- `test-output-directory`
- `upstream-tests`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-charconv-route.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-catch-isnan-route.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-cid-order.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-test-output-isolation.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-xmp-dll-boundary.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-xmp-dll-link-fix.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-dll-export.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-test-resource-isolation.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-xmp-schema-lifetime.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\NOTICE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Consumer: DLL-Grenzen und Runtime

Für BCC64X-Windows-EXE, DLL und MODULE gilt dynamische Runtime `-tR`. PoDoFo übergibt C++-Objekte und Ownership über DLL-Grenzen; statisch getrennte Runtime-Domänen sind nicht zulässig. Tests mit XMP-Objekten und Destruktion sind besonders wichtig.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/podofo/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
