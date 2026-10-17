# libarchive 3.8.10: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `libarchive@3.8.10`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- BCC64X compatibility backpatch for legacy __BORLANDC__ compiler branches. BCC64X also defines __clang__ and must therefore use the modern Clang-compatible paths for inline handling, integer types and literals, Windows lseek/mbstate_t handling, open mode arguments, and the corresponding tests. The patch also includes the upstream Windows/Clang fix from commit f75ed3a20526ef4ec2f46ee94216b6efd01eab0a for filter_fork_windows.c. In test_read_data_large.c, an existing ftruncate macro is explicitly undefined before libar
- CMake resolves the resource compiler through its BCC64X platform integration.
- Complete native command line toolset. The product package contains the shared runtime and import library; libarchive's internal static target remains only because upstream tests link against it.
- Declared compression/Windows backends; no accidental host-library pickup.
- BCC64X on Windows provides no libc POSIX regex implementation. With AUTO, libarchive otherwise enters its PCRE/libgcc fallback even though both optional regex backends are disabled below.

## Abhängigkeiten

- `zlib 1.3.2`
- `xz 5.8.4`
- `zstd 1.5.7`
- `bzip2 1.0.8`
- `openssl 3.5.9`

## Quellbeschaffung

- `https://github.com/libarchive/libarchive/releases/download/v{LibraryVersion}/libarchive-{LibraryVersion}.zip`

## Versionsgebundene Patches und Adapter

- `libarchive\3.8.10\bcc64x-modern-borland-compat.patch`
- `{LibraryVersion}\bcc64x-modern-borland-compat.patch`
- `libarchive\3.8.10\managed-bzip2-contract.patch`
- `{LibraryVersion}\managed-bzip2-contract.patch`

## Build-Parameter und Einstellungen

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
- `-DLIBLZMA_INCLUDE_DIR:PATH={DependencyRoot:xz}\include`
- `-DLIBLZMA_LIBRARY:FILEPATH={DependencyRoot:xz}\lib\win64\{Configuration}\liblzma.lib`
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

### Build-Umgebung

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CTEST_OUTPUT_ON_FAILURE=1`
- `PATH={InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{DependencyRoot:xz}\bin\win64\{Configuration};{InstallRoot}\packages\zstd\1.5.7\bin\win64\{Configuration};{InstallRoot}\packages\bzip2\1.0.8\bin\win64\{Configuration};{DependencyRoot:openssl}\bin\win64\{Configuration};{ENV:PATH}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`

### Technische Arbeitsschritte

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

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
