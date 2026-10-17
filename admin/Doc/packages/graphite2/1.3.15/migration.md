# graphite2 1.3.15: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="graphite2" version="1.3.15">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Keine zusätzlichen erklärenden Sonderentscheidungen aus den XML-Kommentaren extrahiert. [nicht verifiziert], ob weitere historische Entscheidungen bestehen.

## Versionierte Abhängigkeiten

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/silnrsi/graphite/releases/download/{LibraryVersion}/graphite2-{LibraryVersion}.tgz`

**Archivnamen:**

- `graphite2-{LibraryVersion}.tgz`

## Patches, Workarounds und Adapter

- `graphite2\1.3.15\fix-nametabletest-langtags.patch`
- `{LibraryVersion}\fix-nametabletest-langtags.patch`
- `graphite2\1.3.15\fix-fnttxtrender-advance.patch`
- `{LibraryVersion}\fix-fnttxtrender-advance.patch`
- `graphite2\1.3.15\diagnose-fnttxtrender-mismatch.patch`
- `{LibraryVersion}\diagnose-fnttxtrender-mismatch.patch`
- `graphite2\1.3.15\fix-gr2fonttest-log-line-endings.patch`
- `{LibraryVersion}\fix-gr2fonttest-log-line-endings.patch`
- `graphite2\1.3.15\fix-nametabletest-utf8-literals.patch`
- `{LibraryVersion}\fix-nametabletest-utf8-literals.patch`
- `graphite2\1.3.15\add-managed-fonttools-path.patch`
- `{LibraryVersion}\add-managed-fonttools-path.patch`
- `graphite2\1.3.15\fix-jsontest-log-line-endings.patch`
- `{LibraryVersion}\fix-jsontest-log-line-endings.patch`
- `graphite2\1.3.15\fix-bcc64x-padauk3-reference.patch`
- `{LibraryVersion}\fix-bcc64x-padauk3-reference.patch`
- `graphite2\1.3.15\bcc64x-python-dll-search.patch`
- `{LibraryVersion}\bcc64x-python-dll-search.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DLIB_SUFFIX:STRING=/win64/{Configuration}`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_TESTING:BOOL={TestsEnabled}`
- `-DPython3_EXECUTABLE:FILEPATH={Tool:python}`
- `-DGRAPHITE2_VM_TYPE:STRING=auto`
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
- `GRAPHITE2_FONTTOOLS_PATH={ToolDir:fonttools}`
- `GRAPHITE2_DLL_SEARCH_PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\src;{ToolDir:bcc64x}`
- `CTEST_PARALLEL_LEVEL={TestJobs}`

**Technische Execute-Actions:**

- `nametabletest-patch-check`
- `nametabletest-patch-apply`
- `nametabletest-patch-verify`
- `fnttxtrender-patch-check`
- `fnttxtrender-patch-apply`
- `fnttxtrender-patch-verify`
- `fnttxtrender-diagnostic-patch-check`
- `fnttxtrender-diagnostic-patch-apply`
- `fnttxtrender-diagnostic-patch-verify`
- `gr2fonttest-log-patch-check`
- `gr2fonttest-log-patch-apply`
- `gr2fonttest-log-patch-verify`
- `nametabletest-utf8-patch-check`
- `nametabletest-utf8-patch-apply`
- `nametabletest-utf8-patch-verify`
- `fonttools-path-patch-check`
- `fonttools-path-patch-apply`
- `fonttools-path-patch-verify`
- `jsontest-log-patch-check`
- `jsontest-log-patch-apply`
- `jsontest-log-patch-verify`
- `padauk3-reference-patch-check`
- `padauk3-reference-patch-apply`
- `padauk3-reference-patch-verify`
- `runtime-fix-check`
- `runtime-fix-apply`
- `runtime-fix-verify`
- `test`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-nametabletest-langtags.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-fnttxtrender-advance.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\diagnose-fnttxtrender-mismatch.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-gr2fonttest-log-line-endings.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-nametabletest-utf8-literals.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\add-managed-fonttools-path.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-jsontest-log-line-endings.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\fix-bcc64x-padauk3-reference.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-python-dll-search.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Consumer-Smokes:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/graphite2/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
