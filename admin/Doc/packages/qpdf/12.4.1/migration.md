# qpdf 12.4.1: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="qpdf" version="12.4.1">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Keine zusätzlichen erklärenden Sonderentscheidungen aus den XML-Kommentaren extrahiert. [nicht verifiziert], ob weitere historische Entscheidungen bestehen.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `libjpeg-turbo 3.2.0`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/qpdf/qpdf/archive/refs/tags/v{LibraryVersion}.tar.gz`

**Archivnamen:**

- `qpdf-{LibraryVersion}.tar.gz`

## Patches, Workarounds und Adapter

- `qpdf\12.4.1\native-windows-qtest.patch`
- `{LibraryVersion}\native-windows-qtest.patch`
- `qpdf\12.4.1\prefer-explicit-managed-dependencies.patch`
- `{LibraryVersion}\prefer-explicit-managed-dependencies.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_STATIC_LIBS:BOOL=OFF`
- `-DBUILD_DOC:BOOL=OFF`
- `-DINSTALL_MANUAL:BOOL=OFF`
- `-DINSTALL_EXAMPLES:BOOL=OFF`
- `-DINSTALL_PKGCONFIG:BOOL=ON`
- `-DINSTALL_CMAKE_PACKAGE:BOOL=ON`
- `-DMAINTAINER_MODE:BOOL=OFF`
- `-DCI_MODE:BOOL=OFF`
- `-DWERROR:BOOL=OFF`
- `-DGENERATE_AUTO_JOB:BOOL=OFF`
- `-DENABLE_QTC:BOOL=OFF`
- `-DREQUIRE_SHELLS:BOOL=OFF`
- `-DUSE_IMPLICIT_CRYPTO:BOOL=OFF`
- `-DREQUIRE_CRYPTO_OPENSSL:BOOL=OFF`
- `-DREQUIRE_CRYPTO_GNUTLS:BOOL=OFF`
- `-DREQUIRE_CRYPTO_NATIVE:BOOL=ON`
- `-DUSE_INSECURE_RANDOM:BOOL=OFF`
- `-DSKIP_OS_SECURE_RANDOM:BOOL=OFF`
- `-DCMAKE_PREFIX_PATH:PATH={InstallRoot}\packages\zlib\1.3.2;{InstallRoot}\packages\libjpeg-turbo\3.2.0`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DZLIB_H_PATH:PATH={InstallRoot}\packages\zlib\1.3.2\include`
- `-DZLIB_LIB_PATH:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\Release\libz.lib`
- `-DLIBJPEG_H_PATH:PATH={InstallRoot}\packages\libjpeg-turbo\3.2.0\include`
- `-DLIBJPEG_LIB_PATH:FILEPATH={InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\Release\libjpeg.lib`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DZLIB_LIB_PATH:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\Debug\libzd.lib`
- `-DLIBJPEG_LIB_PATH:FILEPATH={InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\Debug\libjpegd.lib`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_DEBUG_POSTFIX:STRING=d`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-Dqpdf_DIR:PATH={ConsumerRoot}\lib\win64\{Configuration}\cmake\qpdf`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `QPDF_BCC64X=1`
- `QPDF_PERL={Tool:perl}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`
- `QPDF_NATIVE_WINDOWS_PERL=1`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\libqpdf;{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\qpdf;{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\compare-for-test;{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\zlib-flate;{ToolDir:perl};{ToolDir:bash};{ToolDir:bash}\..\usr\bin;{ENV:PATH}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\libqpdf;{ENV:PATH}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\libqpdf;{ENV:PATH}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\libqpdf;{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={ConsumerRoot}\bin;{InstallRoot}\packages\libjpeg-turbo\3.2.0\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ENV:PATH}`

**Technische Execute-Actions:**

- `qtest-patch-check`
- `qtest-patch-apply`
- `qtest-patch-verify`
- `managed-deps-patch-check`
- `managed-deps-patch-apply`
- `managed-deps-patch-verify`
- `upstream-tests`
- `compare-native-smoke`
- `qpdf-native-smoke`
- `zlib-flate-native-smoke`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\native-windows-qtest.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\prefer-explicit-managed-dependencies.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE.txt`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\NOTICE.md`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/qpdf/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
