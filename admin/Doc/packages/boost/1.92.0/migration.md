# boost 1.92.0: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="boost" version="1.92.0">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Canonical Boost model from the target-machine evidence: - one official upstream CMake source tree; - one coherent Release graph and one coherent Debug graph; - 148 selected source modules covering the 171 verified logical libraries; - OpenCL/MPI/Python-dependent external ecosystems remain explicitly excluded; - component separation happens at the consumer/evidence gates, never …
- R189/R196 evidence classifies Boost.Random as header/INTERFACE. Upstream CMake otherwise enables the optional random_device binary by default.
- Exact managed dependency coordinates used by the evidence build.
- Evidence preflight: fail before the large Boost graph if BCC64X or Boost.Config no longer satisfies the proven native-Clang contract.

## Versionierte Abhängigkeiten

- `openssl 3.5.9`
- `zlib 1.3.2`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/boostorg/boost/releases/download/boost-{LibraryVersion}/boost-{LibraryVersion}-cmake.zip`

**Archivnamen:**

- `boost-{LibraryVersion}-cmake.zip`

## Patches, Workarounds und Adapter

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_PROJECT_INCLUDE:FILEPATH={BuildFileDir}\cmake\boost\BuildEngineBoostProject.cmake`
- `-DCMAKE_CXX_STANDARD:STRING=23`
- `-DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON`
- `-DCMAKE_CXX_EXTENSIONS:BOOL=OFF`
- `-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF`
- `-DCMAKE_INSTALL_PREFIX:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}`
- `-DCMAKE_INSTALL_INCLUDEDIR:STRING=include`
- `-DBOOST_INSTALL_LAYOUT:STRING=system`
- `-DBOOST_INSTALL_CMAKEDIR:STRING=lib/win64/{Configuration}/cmake`
- `-DBOOST_INCLUDE_LIBRARIES:STRING=filesystem;system;program_options;regex;thread;chrono;json;asio;beast;accumulators;decimal;endian;geometry;histogram;integer;math;multi_array;multiprecision;numeric/conversion;numeric/interval;numeric/odeint;numeric/ublas;polygon;qvm;random;ratio;rational;safe_numerics;units;utility;bind;callable_traits;core;describe;function_types;fusion;hana;hof;lambda;lambda2;local_function;metaparse;mp11;mpl;msm;parser;pfr;phoenix;preprocessor;proto;spirit;statechart;static_assert;tti;type_traits;typeof;vmd;yap;algorithm;any;array;bimap;bloom;circular_buffer;foreach;gil;graph;heap;icl;intrusive;iterator;multi_index;optional;poly_collection;property_map;property_tree;ptr_container;range;sort;tuple;unordered;uuid;variant;variant2;cobalt;context;coroutine;coroutine2;fiber;interprocess;lockfree;mqtt5;mysql;redis;assign;charconv;contract;convert;dll;exception;format;io;iostreams;leaf;lexical_cast;locale;log;nowide;process;serialization;stacktrace;static_string;test;timer;tokenizer;type_erasure;type_index;url;wave;winapi;xpressive;align;assert;compat;concept_check;config;container_hash;conversion;crc;detail;dynamic_bitset;flyweight;function;functional;hash2;logic;move;openmethod;outcome;parameter;pool;predef;scope;scope_exit;signals2;smart_ptr;stl_interfaces;throw_exception`
- `-DBOOST_ENABLE_MPI:BOOL=OFF`
- `-DBOOST_ENABLE_PYTHON:BOOL=OFF`
- `-DBOOST_IOSTREAMS_ENABLE_ZLIB:BOOL=ON`
- `-DBOOST_RANDOM_ENABLE_RANDOM_DEVICE:BOOL=OFF`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_TESTING:BOOL=OFF`
- `-DOPENSSL_INCLUDE_DIR:PATH={DependencyRoot:openssl}\include`
- `-DADECC_BOOST_OPENSSL_SSL_LIBRARY:FILEPATH={DependencyRoot:openssl}\lib\win64\{Configuration}\libssl.lib`
- `-DADECC_BOOST_OPENSSL_CRYPTO_LIBRARY:FILEPATH={DependencyRoot:openssl}\lib\win64\{Configuration}\libcrypto.lib`
- `-DOPENSSL_USE_STATIC_LIBS:BOOL=OFF`
- `-DZLIB_INCLUDE_DIR:PATH={InstallRoot}\packages\zlib\1.3.2\include`
- `-DZLIB_LIBRARY:FILEPATH={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibImportLibrary}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DNDEBUG`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Release`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib/win64/Debug`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin/win64/Debug`
- `-DBOOST_GATE:STRING=closure`
- `-DBOOST_PACKAGE_CMAKE_ROOT:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\Debug\cmake`
- `-DCMAKE_PROJECT_INCLUDE:FILEPATH={BuildFileDir}\cmake\boost\bcc64x-native-clang\project-include.cmake`
- `-DADECC_BOOST_MANAGED_INCLUDE_DIR:PATH={InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `-DBOOST_GATE:STRING=math-numerics`
- `-DBOOST_GATE:STRING=state-parsing-meta`
- `-DBOOST_GATE:STRING=algorithms-containers-data`
- `-DBOOST_GATE:STRING=concurrency-async`
- `-DBOOST_GATE:STRING=system-io-runtime`
- `-DBOOST_GATE:STRING=foundation-language`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`
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
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `preflight-directory`
- `preflight-native-noexcept`
- `preflight-clang-feature`
- `preflight-boost-config`
- `remove-installed-bin`
- `remove-installed-lib`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE_1_0.txt`

**Consumer-Smokes:**

- `closure`
- `math-numerics`
- `state-parsing-meta`
- `algorithms-containers-data`
- `concurrency-async`
- `system-io-runtime`
- `foundation-language`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Consumer: Include- und Headerreihenfolge

**Wichtig:** Das vom BuildEngine-Paket veröffentlichte Boost-Headerverzeichnis muss in der Include-Suchliste **vor** anderen Boost-Installationen stehen. Andernfalls können Header einer anderen Boost-Version mit den hier gebauten Binaries kombiniert werden. Die exakte Include-Root ist aus dem Publish-/Install-Vertrag der Version abzuleiten und im Consumer-Smoke zu prüfen.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/boost/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
