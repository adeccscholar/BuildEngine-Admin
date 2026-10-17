# opencv 5.0.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="opencv" version="5.0.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Temporary toolchain baseline: IPPICV MSVC /GS symbols need a verified BCC64X runtime provider.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/opencv/opencv/archive/40738fb16ceddb5fb3fea747585f7ce6abb0605b.zip`

**Archivnamen:**

- `opencv-40738fb16ceddb5fb3fea747585f7ce6abb0605b.zip`

## Patches, Workarounds und Adapter

- `opencv\5.0.0\bcc64x-optional-mlas.patch`
- `{LibraryVersion}\bcc64x-optional-mlas.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DOPENCV_INCLUDE_INSTALL_PATH:STRING=include/opencv5`
- `-DOPENCV_LIB_INSTALL_PATH:STRING=lib/win64/{Configuration}`
- `-DOPENCV_BIN_INSTALL_PATH:STRING=bin/win64/{Configuration}`
- `-DOPENCV_CONFIG_INSTALL_PATH:STRING=lib/win64/{Configuration}/cmake/opencv5`
- `-DOPENCV_SKIP_CMAKE_ROOT_CONFIG:BOOL=ON`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_LIST:STRING=core,imgproc,geometry,flann,features,video,photo,stereo,ptcloud,calib,dnn,objdetect,imgcodecs,videoio,highgui`
- `-DBUILD_opencv_world:BOOL=OFF`
- `-DBUILD_TESTS:BOOL=OFF`
- `-DBUILD_PERF_TESTS:BOOL=OFF`
- `-DBUILD_EXAMPLES:BOOL=OFF`
- `-DBUILD_DOCS:BOOL=OFF`
- `-DBUILD_opencv_apps:BOOL=OFF`
- `-DBUILD_JAVA:BOOL=OFF`
- `-DBUILD_opencv_python3:BOOL=OFF`
- `-DWITH_IPP:BOOL=OFF`
- `-DOPENCV_DNN_MLAS:BOOL=OFF`
- `-DWITH_ONNXRUNTIME:BOOL=OFF`
- `-DWITH_PROTOBUF:BOOL=ON`
- `-DBUILD_PROTOBUF:BOOL=ON`
- `-DPROTOBUF_UPDATE_FILES:BOOL=OFF`
- `-DOPENCV_DISABLE_THREAD_SUPPORT:BOOL=OFF`
- `-DWITH_OPENCL:BOOL=OFF`
- `-DWITH_CUDA:BOOL=OFF`
- `-DWITH_DIRECTML:BOOL=OFF`
- `-DWITH_TBB:BOOL=OFF`
- `-DWITH_OPENMP:BOOL=OFF`
- `-DWITH_FFMPEG:BOOL=OFF`
- `-DWITH_GSTREAMER:BOOL=OFF`
- `-DWITH_DSHOW:BOOL=OFF`
- `-DWITH_MSMF:BOOL=OFF`
- `-DWITH_OBSENSOR:BOOL=OFF`
- `-DWITH_QT:BOOL=OFF`
- `-DWITH_GTK:BOOL=OFF`
- `-DWITH_OPENGL:BOOL=OFF`
- `-DWITH_UNIFONT:BOOL=OFF`
- `-DWITH_JPEG:BOOL=ON`
- `-DBUILD_JPEG:BOOL=ON`
- `-DWITH_PNG:BOOL=ON`
- `-DBUILD_PNG:BOOL=ON`
- `-DWITH_TIFF:BOOL=ON`
- `-DBUILD_TIFF:BOOL=ON`
- `-DWITH_WEBP:BOOL=ON`
- `-DBUILD_WEBP:BOOL=ON`
- `-DWITH_OPENJPEG:BOOL=ON`
- `-DBUILD_OPENJPEG:BOOL=ON`
- `-DBUILD_ZLIB:BOOL=OFF`
- `-DZLIB_INCLUDE_DIR:PATH={InstallRoot}\packages\zlib\1.3.2\include`
- `-DZLIB_LIBRARY:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibLibrary}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `patch-check`
- `patch-apply`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-optional-mlas.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/opencv/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
