# libpq 18.6: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `libpq@18.6`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- Reuse the proven generic Meson bootstrap already used by the native BCC64X Mesa build.
- Native BCC64X PostgreSQL client build. No MSVC, clang-cl or MinGW fallback.

## Abhängigkeiten

- `openssl 3.5.9`
- `zlib 1.3.2`

## Quellbeschaffung

- `https://ftp.postgresql.org/pub/source/v{LibraryVersion}/postgresql-{LibraryVersion}.tar.bz2`

## Versionsgebundene Patches und Adapter

- `libpq\18.6\bcc64x-ntsecapi-conflict.patch`
- `{LibraryVersion}\bcc64x-ntsecapi-conflict.patch`
- `libpq\18.6\bcc64x-winsock-link.patch`
- `{LibraryVersion}\bcc64x-winsock-link.patch`

## Build-Parameter und Einstellungen

- `-DWIN32`
- `-Dssl=openssl`
- `-Dzlib=enabled`
- `-Dreadline=disabled`
- `-Dicu=disabled`
- `-Dldap=disabled`
- `-Dgssapi=disabled`
- `-Dlibcurl=disabled`
- `-Dnls=disabled`
- `-Ddocs=disabled`
- `-Ddocs_pdf=disabled`
- `-Dtap_tests=disabled`
- `-DPERL={Tool:perl}`
- `-Dextra_include_dirs={DependencyRoot:openssl}\include,{InstallRoot}\packages\zlib\1.3.2\include`
- `-Dextra_lib_dirs={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\compat-lib,{DependencyRoot:openssl}\lib\win64\{Configuration},{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}`
- `-DPostgreSQL_INCLUDE_DIR:PATH={ConsumerRoot}\include`
- `-DPostgreSQL_LIBRARY:FILEPATH={ConsumerRoot}\lib\libpq.lib`

### Build-Umgebung

- `CC={Tool:bcc64x}`
- `CFLAGS=-DWIN32`
- `LDFLAGS=-L{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\compat-lib`
- `CXX={Tool:bcc64x}`
- `AR={Tool:llvm-ar}`
- `RANLIB={Tool:llvm-ranlib}`
- `WINDRES={Tool:bcc64x-windres}`
- `PKG_CONFIG={Tool:pkg-config}`
- `PKG_CONFIG_PATH={DependencyRoot:openssl}\lib\win64\{Configuration}\pkgconfig;{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\pkgconfig`
- `PATH={ToolDir:bcc64x};{ToolDir:llvm-ar};{ToolDir:llvm-ranlib};{ToolDir:bcc64x-windres};{ToolDir:ninja};{ToolDir:python};{ToolDir:perl};{ToolDir:pkg-config};{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={DependencyRoot:openssl}\bin\win64\Release;{InstallRoot}\packages\zlib\1.3.2\bin\win64\Release;{ENV:PATH}`

### Technische Arbeitsschritte

- `ntsecapi-patch-check`
- `ntsecapi-patch-apply`
- `winsock-patch-check`
- `winsock-patch-apply`
- `remove-build`
- `create-zlib-compat-dir`
- `stage-zlib-import-name`
- `stage-zlib-gnu-name`
- `meson-setup`
- `meson-compile-libpq`
- `create-bin-dir`
- `create-lib-dir`
- `libpq-def`
- `libpq-import`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
