# libarchive 3.8.9: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="libarchive" version="3.8.9">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

**Versionshinweis:** Im Vertrag als ersetzt durch `3.8.10` gekennzeichnet.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- BCC64X compatibility backpatch for legacy __BORLANDC__ compiler branches. BCC64X also defines __clang__ and must therefore use the modern Clang-compatible paths for inline handling, integer types and literals, Windows lseek/mbstate_t handling, open mode arguments, and the corresponding tests. The patch also includes the upstream Windows/Clang fix from commit f75ed3a20526ef4ec2f…
- CMake resolves the resource compiler through its BCC64X platform integration.
- Complete native command line toolset. The product package contains the shared runtime and import library; libarchive's internal static target remains only because upstream tests link against it.
- Declared compression/Windows backends; no accidental host-library pickup.
- BCC64X on Windows provides no libc POSIX regex implementation. With AUTO, libarchive otherwise enters its PCRE/libgcc fallback even though both optional regex backends are disabled below.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `xz 5.8.3`
- `zstd 1.5.7`
- `bzip2 1.0.8`
- `openssl 3.5.9`
- `zlib 1.3.2`
- `xz 5.8.4`
- `zstd 1.5.7`
- `bzip2 1.0.8`
- `openssl 3.5.9`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/libarchive/libarchive/releases/download/v{LibraryVersion}/libarchive-{LibraryVersion}.zip`

**Archivnamen:**

- `libarchive-{LibraryVersion}.zip`

## Patches, Workarounds und Adapter

- `libarchive\3.8.9\bcc64x-modern-borland-compat.patch`
- `{LibraryVersion}\bcc64x-modern-borland-compat.patch`
- `libarchive\3.8.9\managed-bzip2-contract.patch`
- `{LibraryVersion}\managed-bzip2-contract.patch`
- `libarchive\3.8.10\bcc64x-modern-borland-compat.patch`
- `libarchive\3.8.10\managed-bzip2-contract.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DENABLE_INSTALL:BOOL=ON`
- `-DENABLE_TEST:BOOL={TestsEnabled}`
- `-DENABLE_TAR:BOOL=ON`
- `-DENABLE_TAR_SHARED:BOOL=ON`
- `-DENABLE_CPIO:BOOL=ON`
- `-DENABLE_CPIO_SHARED:BOOL=ON`
- `-DENABLE_CAT:BOOL=ON`
- `-DENABLE_CAT_SHARED:BOOL=ON`
- `-DENABLE_UNZIP:BOOL=ON`
- `-DENABLE_UNZIP_SHARED:BOOL=ON`
- `-DENABLE_ZLIB:BOOL=ON`
- `-DZLIB_INCLUDE_DIR:PATH={InstallRoot}\packages\zlib\1.3.2\include`
- `-DZLIB_LIBRARY:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibLibraryName}.lib`
- `-DENABLE_CNG:BOOL=ON`
- `-DENABLE_WIN32_XMLLITE:BOOL=ON`
- `-DENABLE_OPENSSL:BOOL=ON`
- `-DOPENSSL_ROOT_DIR:PATH={DependencyRoot:openssl}`
- `-DOPENSSL_INCLUDE_DIR:PATH={DependencyRoot:openssl}\include`
- `-DOPENSSL_SSL_LIBRARY:FILEPATH={DependencyRoot:openssl}\lib\win64\{Configuration}\libssl.lib`
- `-DOPENSSL_CRYPTO_LIBRARY:FILEPATH={DependencyRoot:openssl}\lib\win64\{Configuration}\libcrypto.lib`
- `-DOPENSSL_USE_STATIC_LIBS:BOOL=FALSE`
- `-DENABLE_MBEDTLS:BOOL=OFF`
- `-DENABLE_NETTLE:BOOL=OFF`
- `-DENABLE_LIBB2:BOOL=OFF`
- `-DENABLE_LZ4:BOOL=OFF`
- `-DENABLE_LZO:BOOL=OFF`
- `-DENABLE_LZMA:BOOL=ON`
- `-DLIBLZMA_INCLUDE_DIR:PATH={InstallRoot}\packages\xz\5.8.3\include`
- `-DLIBLZMA_LIBRARY:FILEPATH={InstallRoot}\packages\xz\5.8.3\lib\win64\{Configuration}\liblzma.lib`
- `-DENABLE_ZSTD:BOOL=ON`
- `-DZSTD_INCLUDE_DIR:PATH={InstallRoot}\packages\zstd\1.5.7\include`
- `-DZSTD_LIBRARY:FILEPATH={InstallRoot}\packages\zstd\1.5.7\lib\win64\{Configuration}\libzstd.lib`
- `-DENABLE_BZip2:BOOL=ON`
- `-DBZIP2_INCLUDE_DIR:PATH={InstallRoot}\packages\bzip2\1.0.8\include`
- `-DBZIP2_LIBRARIES:FILEPATH={InstallRoot}\packages\bzip2\1.0.8\lib\win64\{Configuration}\{Bzip2LibraryName}.lib`
- `-DENABLE_LIBXML2:BOOL=OFF`
- `-DENABLE_EXPAT:BOOL=OFF`
- `-DPOSIX_REGEX_LIB:STRING=NONE`
- `-DENABLE_PCREPOSIX:BOOL=OFF`
- `-DENABLE_PCRE2POSIX:BOOL=OFF`
- `-DENABLE_LIBGCC:BOOL=OFF`
- `-DENABLE_ICONV:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DLIBLZMA_INCLUDE_DIR:PATH={DependencyRoot:xz}\include`
- `-DLIBLZMA_LIBRARY:FILEPATH={DependencyRoot:xz}\lib\win64\{Configuration}\liblzma.lib`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CTEST_OUTPUT_ON_FAILURE=1`
- `PATH={InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{InstallRoot}\packages\xz\5.8.3\bin\win64\{Configuration};{InstallRoot}\packages\zstd\1.5.7\bin\win64\{Configuration};{InstallRoot}\packages\bzip2\1.0.8\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{ENV:PATH}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `CTEST_OUTPUT_ON_FAILURE=1`
- `PATH={InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{InstallRoot}\packages\xz\5.8.3\bin\win64\{Configuration};{InstallRoot}\packages\zstd\1.5.7\bin\win64\{Configuration};{InstallRoot}\packages\bzip2\1.0.8\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CTEST_OUTPUT_ON_FAILURE=1`
- `PATH={InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{DependencyRoot:xz}\bin\win64\{Configuration};{InstallRoot}\packages\zstd\1.5.7\bin\win64\{Configuration};{InstallRoot}\packages\bzip2\1.0.8\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{ENV:PATH}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `CTEST_OUTPUT_ON_FAILURE=1`
- `PATH={InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{DependencyRoot:xz}\bin\win64\{Configuration};{InstallRoot}\packages\zstd\1.5.7\bin\win64\{Configuration};{InstallRoot}\packages\bzip2\1.0.8\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `patch-check`
- `patch-apply`
- `patch-verify`
- `bzip2-patch-check`
- `bzip2-patch-apply`
- `bzip2-patch-verify`
- `remove-build`
- `bzip2-capability-test`
- `test`
- `remove-installed-bin`
- `remove-installed-lib`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-modern-borland-compat.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\managed-bzip2-contract.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\{ArchiveName}.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\pkgconfig`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\share`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/libarchive/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
