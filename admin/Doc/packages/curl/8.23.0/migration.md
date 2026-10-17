# curl 8.23.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="curl" version="8.23.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- curl/8.23.0 now tests the updated OpenSSL/3.5.9 dependency. Keep zlib enabled; Brotli/Zstd remain disabled until independently validated for this curl version and BCC64X runtime.
- Preserve the proven BCC64X module-based dependency lookup. OpenSSL 3.5.9 and zlib paths are selected per configuration.
- CMake 4.1.1 FindOpenSSL performs its own find_library() lookup on WIN32 and then rewrites OPENSSL_SSL_LIBRARY/OPENSSL_CRYPTO_LIBRARY. Do not set OPENSSL_ROOT_DIR here: that activates NO_DEFAULT_PATH inside FindOpenSSL and hides BuildEngine's configuration-specific lib/win64/Release directory. The generic producer integration supplies that exact directory through CMAKE_LIBRARY_P…
- Same FindOpenSSL rule as Release; CMAKE_LIBRARY_PATH is variant-specific.

## Versionierte Abhängigkeiten

- `openssl 3.5.9`
- `zlib 1.3.2`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://curl.se/download/curl-{LibraryVersion}.tar.xz`

**Archivnamen:**

- `curl-{LibraryVersion}.tar.xz`

## Patches, Workarounds und Adapter

- `curl\8.23.0\bcc64x-winsock-inline.patch`
- `{LibraryVersion}\bcc64x-winsock-inline.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILDENGINE_BCC64X_WINSDK_INLINE:BOOL=ON`
- `-DBUILD_STATIC_LIBS:BOOL=OFF`
- `-DBUILD_CURL_EXE:BOOL=OFF`
- `-DBUILD_EXAMPLES:BOOL=OFF`
- `-DBUILD_TESTING:BOOL=OFF`
- `-DBUILD_LIBCURL_DOCS:BOOL=OFF`
- `-DBUILD_MISC_DOCS:BOOL=OFF`
- `-DENABLE_CURL_MANUAL:BOOL=OFF`
- `-DCURL_ENABLE_EXPORT_TARGET:BOOL=ON`
- `-DCURL_USE_OPENSSL:BOOL=ON`
- `-DCURL_USE_SCHANNEL:BOOL=OFF`
- `-DENABLE_ARES:BOOL=OFF`
- `-DUSE_LIBIDN2:BOOL=OFF`
- `-DCURL_ZLIB:STRING=ON`
- `-DCURL_BROTLI:STRING=OFF`
- `-DCURL_ZSTD:STRING=OFF`
- `-DCURL_USE_PKGCONFIG:BOOL=OFF`
- `-DCURL_USE_CMAKECONFIG:BOOL=OFF`
- `-DCMAKE_FIND_PACKAGE_PREFER_CONFIG:BOOL=OFF`
- `-DOPENSSL_USE_STATIC_LIBS:BOOL=OFF`
- `-DZLIB_USE_STATIC_LIBS:BOOL=OFF`
- `-DCURL_USE_LIBPSL:BOOL=OFF`
- `-DCURL_USE_LIBSSH2:BOOL=OFF`
- `-DCURL_USE_LIBSSH:BOOL=OFF`
- `-DUSE_NGHTTP2:BOOL=OFF`
- `-DUSE_NGTCP2:BOOL=OFF`
- `-DUSE_QUICHE:BOOL=OFF`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DOPENSSL_INCLUDE_DIR:PATH={InstallRoot}\packages\openssl\3.5.9\include`
- `-DZLIB_INCLUDE_DIR:PATH={InstallRoot}\packages\zlib\1.3.2\include`
- `-DZLIB_LIBRARY:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\Release\libz.lib`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DZLIB_LIBRARY:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\Debug\libzd.lib`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `win-inline-patch-check`
- `win-inline-patch-apply`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-winsock-inline.patch`
- `{CurrentArtifact}\lib\bcc64x-winsock-compat.c`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\curl-COPYING.txt`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/curl/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
