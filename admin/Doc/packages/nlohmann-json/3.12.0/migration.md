# nlohmann-json 3.12.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="nlohmann-json" version="3.12.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Temporary replacement fragment for admin/build-libraries.xml. Apply together with: buildLibraries/@schemaVersion = "15" xsi:noNamespaceSchemaLocation = "schemas/build-libraries-v15.xsd" Replace the existing <library id="ace-tao" ...>...</library> block with the two library elements below. The file is intentionally temporary and can be deleted after the block has been copied int…
- No TAO download/extract or patching here: ACE owns and prepares the complete ACE_wrappers source tree, including TAO source repairs, in Source.
- Overlay the shared ACE payload; never clean shared target directories.
- Mesa mandatory Python configure-time modules. Keep them pinned and source-local so the build does not depend on global pip/site-packages.
- Native BCC64X Mesa build. No MSVC, clang-cl or MinGW fallback is permitted.
- Import libraries are derived only from the BCC64X-built Mesa DLLs.
- Reuse the proven generic Meson bootstrap already used by the native BCC64X Mesa build.
- Native BCC64X PostgreSQL client build. No MSVC, clang-cl or MinGW fallback.

## Versionierte Abhängigkeiten

- `openssl 3.5.9`
- `xerces-c 3.3.0`
- `zlib 1.3.2`
- `opengl 26.2.1`
- `opengl 26.2.1`
- `openssl 3.5.9`
- `zlib 1.3.2`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/nlohmann/json/releases/download/v{LibraryVersion}/include.zip`
- `https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-8_0_6/ACE%2BTAO-{LibraryVersion}.zip`
- `https://sourceware.org/pub/bzip2/bzip2-{LibraryVersion}.tar.gz`
- `https://github.com/nigels-com/glew/releases/download/glew-{LibraryVersion}/glew-{LibraryVersion}.zip`
- `https://archive.mesa3d.org/mesa-{LibraryVersion}.tar.xz`
- `https://files.pythonhosted.org/packages/source/m/meson/meson-1.12.0.tar.gz`
- `https://files.pythonhosted.org/packages/7d/fa/3944b40b07da9ce895c0e6303a5ab7d53da063554f534556b134a54d6093/packaging-26.3.tar.gz`
- `https://files.pythonhosted.org/packages/7e/99/7690b6d4034fffd95959cbe0c02de8deb3098cc577c67bb6a24fe5d7caa7/markupsafe-3.0.3.tar.gz`
- `https://files.pythonhosted.org/packages/2a/12/b5fa2353e2754cd67fb9f83793fa48ff42c213a5da7e719869d2301f6ab8/mako-1.4.1.tar.gz`
- `https://files.pythonhosted.org/packages/05/8e/961c0007c59b8dd7729d542c61a4d537767a59645b82a0b521206e1e25c2/pyyaml-6.0.3.tar.gz`
- `https://github.com/raysan5/raylib/archive/dbc56a87da87d973a9c5baa4e7438a9d20121d28.zip`
- `https://github.com/libsdl-org/SDL/releases/download/release-{LibraryVersion}/SDL2-{LibraryVersion}.zip`
- `https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip`
- `https://ftp.postgresql.org/pub/source/v{LibraryVersion}/postgresql-{LibraryVersion}.tar.bz2`

**Archivnamen:**

- `nlohmann-json-{LibraryVersion}-include.zip`
- `ACE+TAO-{LibraryVersion}.zip`
- `bzip2-{LibraryVersion}.tar.gz`
- `glew-{LibraryVersion}.zip`
- `mesa-{LibraryVersion}.tar.xz`
- `meson-1.12.0.tar.gz`
- `packaging-26.3.tar.gz`
- `markupsafe-3.0.3.tar.gz`
- `mako-1.4.1.tar.gz`
- `pyyaml-6.0.3.tar.gz`
- `raylib-dbc56a87da87d973a9c5baa4e7438a9d20121d28.zip`
- `SDL2-{LibraryVersion}.zip`
- `sqlite-amalgamation-3530400.zip`
- `postgresql-{LibraryVersion}.tar.bz2`

## Patches, Workarounds und Adapter

- `ace-tao\8.0.6\bcc64x-win64-upstream-backports.patch`
- `{LibraryVersion}\bcc64x-win64-upstream-backports.patch`
- `ace-tao\8.0.6\bcc64x-xerces-modern-borland.patch`
- `{LibraryVersion}\bcc64x-xerces-modern-borland.patch`
- `ace-tao\8.0.6\bcc64x-zlib-library-names.patch`
- `{LibraryVersion}\bcc64x-zlib-library-names.patch`
- `ace-tao\8.0.6\upstream-ssliop-params-dup-cpp.patch`
- `{LibraryVersion}\upstream-ssliop-params-dup-cpp.patch`
- `libpq\18.6\bcc64x-ntsecapi-conflict.patch`
- `{LibraryVersion}\bcc64x-ntsecapi-conflict.patch`
- `libpq\18.6\bcc64x-winsock-link.patch`
- `{LibraryVersion}\bcc64x-winsock-link.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DBZIP2_SOURCE_DIR:PATH={Workspace}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib\win64\{Configuration}`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin\win64\{Configuration}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DGLEW_SOURCE_DIR:PATH={Workspace}`
- `-DOPENGL_LIBRARY:FILEPATH={InstallRoot}\packages\opengl\26.2.1\lib\win64\{Configuration}\opengl32.lib`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-Dplatforms=windows`
- `-Dgallium-drivers=softpipe`
- `-Dgallium-wgl-dll-name=libgallium_wgl`
- `-Dopengl=true`
- `-Dgles1=disabled`
- `-Dgles2=disabled`
- `-Degl=disabled`
- `-Dglx=disabled`
- `-Dgbm=disabled`
- `-Dvulkan-drivers=`
- `-Dvulkan-layers=`
- `-Dllvm=disabled`
- `-Dzlib=disabled`
- `-Dzstd=disabled`
- `-Dxmlconfig=disabled`
- `-Dbuild-tests=true`
- `-Dtools=`
- `-Dgallium-rusticl=false`
- `-DRAYLIB_SOURCE_DIR:PATH={Workspace}`
- `-DBCC64X_EXECUTABLE:FILEPATH={Tool:bcc64x}`
- `-DSDL_SHARED:BOOL=ON`
- `-DSDL_STATIC:BOOL=OFF`
- `-DSDL_TEST:BOOL=OFF`
- `-DSDL_TEST_LIBRARY:BOOL=OFF`
- `-DSDL_DIRECTX:BOOL=OFF`
- `-DSDL_RENDER_D3D:BOOL=OFF`
- `-DSDL_RENDER_D3D11:BOOL=OFF`
- `-DSDL_RENDER_D3D12:BOOL=OFF`
- `-DSQLITE_SOURCE_DIR:PATH={Workspace}`
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

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `ACE_ROOT={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers`
- `SSL_ROOT={DependencyRoot:openssl}`
- `SSL_INCDIR={DependencyRoot:openssl}\include`
- `SSL_LIBDIR={DependencyRoot:openssl}\lib\win64\{Configuration}`
- `XERCESC_INCDIR={InstallRoot}\packages\xerces-c\3.3.0\include`
- `XERCESC_LIBDIR={InstallRoot}\packages\xerces-c\3.3.0\lib\win64\{Configuration}`
- `XERCESLIB={XercesLibrary}`
- `ZLIB_ROOT={InstallRoot}\packages\zlib\1.3.2`
- `ZLIB_INCDIR={InstallRoot}\packages\zlib\1.3.2\include`
- `ZLIB_LIBDIR={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}`
- `BCC32C=`
- `BCC64=`
- `BCC64X=1`
- `CODEGUARD=`
- `UNICODE=`
- `DEBUG={DebugFlag}`
- `RELEASE={ReleaseFlag}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers\lib;{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers\bin;{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\xerces-c\3.3.0\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ToolDir:perl};{ToolDir:bcc64x};{ToolDir:embarcadero-make};{ENV:PATH}`
- `ACE_ROOT={ExtensionProducerRoot}`
- `TAO_ROOT={ExtensionProducerRoot}\TAO`
- `SSL_ROOT={DependencyRoot:openssl}`
- `SSL_INCDIR={DependencyRoot:openssl}\include`
- `SSL_LIBDIR={DependencyRoot:openssl}\lib\win64\{Configuration}`
- `XERCESC_INCDIR={InstallRoot}\packages\xerces-c\3.3.0\include`
- `XERCESC_LIBDIR={InstallRoot}\packages\xerces-c\3.3.0\lib\win64\{Configuration}`
- `XERCESLIB={XercesLibrary}`
- `ZLIB_ROOT={InstallRoot}\packages\zlib\1.3.2`
- `ZLIB_INCDIR={InstallRoot}\packages\zlib\1.3.2\include`
- `ZLIB_LIBDIR={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}`
- `BCC32C=`
- `BCC64=`
- `BCC64X=1`
- `CODEGUARD=`
- `UNICODE=`
- `DEBUG={DebugFlag}`
- `RELEASE={ReleaseFlag}`
- `PATH={ExtensionProducerRoot}\lib;{ExtensionProducerRoot}\bin;{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\xerces-c\3.3.0\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ToolDir:perl};{ToolDir:bcc64x};{ToolDir:embarcadero-make};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`

**Technische Execute-Actions:**

- `win64-backports-check`
- `win64-backports-apply`
- `xerces-modern-borland-check`
- `xerces-modern-borland-apply`
- `zlib-library-names-check`
- `zlib-library-names-apply`
- `ssliop-backport-check`
- `ssliop-backport-apply`
- `remove-build-tree`
- `copy-build-tree`
- `remove-build`
- `create-libdir`
- `opengl-def`
- `opengl-import`
- `wgl-def`
- `wgl-import`
- `create-import-def`
- `create-import-lib`
- `remove-upstream-import-libs`
- `ntsecapi-patch-check`
- `ntsecapi-patch-apply`
- `winsock-patch-check`
- `winsock-patch-apply`
- `create-zlib-compat-dir`
- `stage-zlib-import-name`
- `stage-zlib-gnu-name`
- `create-bin-dir`
- `create-lib-dir`
- `libpq-def`
- `libpq-import`

**Targets:**

- `generate-workspace`
- `build-workspace`
- `generate-naming-service`
- `build-naming-service`
- `generate-cos-event-service`
- `build-cos-event-service`
- `generate-rt-event-service`
- `build-rt-event-service`
- `meson-setup`
- `meson-compile`
- `meson-test`
- `meson-install`
- `meson-compile-libpq`

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\nlohmann`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.MIT`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-win64-upstream-backports.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-xerces-modern-borland.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-zlib-library-names.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\upstream-ssliop-params-dup-cpp.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\tools\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\ace`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\ACE_VERSION.txt`
- `{ExtensionInstallRoot}\lib\win64\{Configuration}`
- `{ExtensionInstallRoot}\bin\win64\{Configuration}`
- `{ExtensionInstallRoot}\tools\bin\win64\{Configuration}`
- `{ExtensionInstallRoot}\services\bin\win64\{Configuration}\tao_cosnaming.exe`
- `{ExtensionInstallRoot}\services\bin\win64\{Configuration}\tao_cosevent.exe`
- `{ExtensionInstallRoot}\services\bin\win64\{Configuration}\tao_rtevent.exe`
- `{ExtensionInstallRoot}\include\tao`
- `{ExtensionInstallRoot}\include\orbsvcs`
- `{ExtensionInstallRoot}\TAO_VERSION.txt`
- `{ExtensionInstallRoot}\.buildengine\extensions\{LibraryId}\{LibraryVersion}\COPYING`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\bzlib.h`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.txt`
- `{Workspace}\.buildengine\meson_bootstrap.py`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\OpenGL\OpenGLConfig.cmake`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\KHR`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE-MESA.rst`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/nlohmann-json/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
