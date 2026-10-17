# openssl 3.5.9: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="openssl" version="3.5.9">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- BCC64X defines both __BORLANDC__ and __clang__. In the current Embarcadero Win64/COFF headers the WinSDK MemoryBarrier macro is not exposed. OpenSSL 3.5.8 uses it in both Windows thread implementations. The patch supplies the missing full fence locally for Clang-based Embarcadero compilers through __atomic_thread_fence with sequentially consistent ordering; upstream call sites …
- BCC64X uses GNU/Clang response-file tokenization. A backslash in OpenSSL's linker inline response file escapes the following character, so crypto\aes\foo.obj arrives as cryptoaesfoo.obj. The patch adds an opt-in target property that converts only paths written to linker response files to forward slashes. Make paths, prerequisites and compiler command lines remain unchanged.
- OpenSSL places Configure-supplied external compression libraries in LIB_EX_LIBS / DSO_EX_LIBS / BIN_EX_LIBS. These values are written into the same BCC64X/LLD response files, but they bypass the path lists normalized by bcc64x-linker-response-paths.patch. Keep the proven response-path patch unchanged and normalize only these response-file copies of external library arguments.
- The Microsoft SDK mstcpip.h and mswsockdef.h headers use GNU-style extern inline declarations for non-MSVC compilers. BCC64X uses Clang C inline semantics, which causes unused header helpers to become emitted definitions with unresolved SDK dependencies. OpenSSL's quic_reactor.c only needs SIO_UDP_CONNRESET and SIO_UDP_NETRESET from mstcpip.h/mswsock.h. The patch keeps winsock2…
- OpenSSL's Windows::cppbuilder platform still models the legacy C++Builder debug sidecar as .tds. BCC64X is the modern COFF/LLD toolchain and uses PDB debug information instead. The patch adds a dedicated Windows::bcc64x platform that inherits the existing C++Builder behavior and overrides only the debug-file extension.
- Native BCC64X OpenSSL build. All OpenSSL target knowledge remains in this XML contract. The perl target materializes the OpenSSL Configure target file in the variant build tree.
- OpenSSL 3.5.8 expects unprefixed Brotli import-library filenames on Windows (brotlicommon.lib, brotlidec.lib, brotlienc.lib). The upstream Brotli/BCC64X producer correctly installs libbrotli*.lib. Keep the producer artifacts untouched and create private consumer-side aliases in the OpenSSL build tree. The COFF import descriptors still reference the original libbrotli*.dll names…
- First native proof keeps test/install in the existing perVariant sequence. Scheduler-level Release/Debug test parallelism remains a separate generic change.
- Generate BCC64X COFF import libraries directly from the produced native DLLs.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `brotli 1.2.0`
- `zstd 1.5.7`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/openssl/openssl/releases/download/openssl-{LibraryVersion}/openssl-{LibraryVersion}.tar.gz`

**Archivnamen:**

- `openssl-{LibraryVersion}.tar.gz`

## Patches, Workarounds und Adapter

- `openssl\3.5.9\bcc64x-memory-barrier.patch`
- `{LibraryVersion}\bcc64x-memory-barrier.patch`
- `openssl\3.5.9\bcc64x-linker-response-paths.patch`
- `{LibraryVersion}\bcc64x-linker-response-paths.patch`
- `openssl\3.5.9\bcc64x-linker-external-libs.patch`
- `{LibraryVersion}\bcc64x-linker-external-libs.patch`
- `openssl\3.5.9\bcc64x-quic-winsock-inline.patch`
- `{LibraryVersion}\bcc64x-quic-winsock-inline.patch`
- `openssl\3.5.9\bcc64x-pdb-platform.patch`
- `{LibraryVersion}\bcc64x-pdb-platform.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DNEAR`
- `-D_WINDLL`
- `-D_DLL`
- `-DNDEBUG`

**Explizite Umgebungsvariablen:**

- `PATH={ToolDir:embarcadero-make};{ToolDir:bcc64x};{ToolDir:nasm};{ToolDir:llvm-ar};{ToolDir:llvm-ranlib};{ToolDir:bcc64x-windres};{ToolDir:perl};{ENV:PATH}`
- `HARNESS_JOBS={TestJobs}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`

**Technische Execute-Actions:**

- `patch-check`
- `patch-apply`
- `response-patch-check`
- `response-patch-apply`
- `external-libs-response-patch-check`
- `external-libs-response-patch-apply`
- `quic-inline-patch-check`
- `quic-inline-patch-apply`
- `pdb-platform-patch-check`
- `pdb-platform-patch-apply`
- `verify-bcc64x-platform`
- `remove-build`
- `create-build`
- `create-compression-imports`
- `brotli-common-import-alias`
- `brotli-dec-import-alias`
- `brotli-enc-import-alias`
- `ssl-def`
- `ssl-import-lib`
- `crypto-def`
- `crypto-import-lib`

**Targets:**

- `configure`
- `build`
- `test`
- `install-upstream`

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-memory-barrier.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-linker-response-paths.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-linker-external-libs.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-quic-winsock-inline.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-pdb-platform.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\OpenSSL-LICENSE.txt`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/openssl/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
