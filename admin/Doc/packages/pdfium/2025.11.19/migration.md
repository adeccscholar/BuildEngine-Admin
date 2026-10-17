# pdfium 2025.11.19: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="pdfium" version="2025.11.19">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Pinned Chromium GN sources from the PDFium DEPS snapshot.
- Pinned PDFium revision a84323421e94f484faca52dd9d027934eba42ab8 commit timestamp, used as reproducible standalone build timestamp.
- gclient_args.gni is generated deterministically instead of running gclient.
- Fail during Source rather than reporting success before the GN imports from this pinned Chromium revision have been provided. All inputs are local. This check never downloads or updates.
- BCC64X Windows intrinsics require a signed volatile LONG pointer. Keep the COM AddRef/Release ULONG return type unchanged.
- BCC64X SDK exposes the two-argument COM QueryInterface only. Adapt typed Skia COM pointers with exact IID and void** output.
- Windows BCC64X is Clang-based: compiler identity must not select POSIX allocation. Change alloc/free/header guards together.
- Prepare shared GN adapters exactly once in Source. Build variants generate separate outputs without modifying the shared workspace.
- Verify source-level GN targets used by both build variants.
- Validate source evidence on resumed builds before generating GN targets.
- Use the locally provisioned Windows toolchain; avoid Chromium depot_tools.
- Verify the GN inputs installed by the single Source scope, read-only.
- Validate the GENERATED build, not merely the requested GN args.
- Do not declare success without a generated GN build and PDFium target.

## Versionierte Abhängigkeiten

- `zlib 1.3.2`
- `libjpeg-turbo 3.2.0`
- `libpng 1.6.58`
- `freetype 2.14.3`
- `icu 78.3`
- `lcms2 2.19.1`
- `openjpeg 2.5.4`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://chromium.googlesource.com/chromium/src/build/+archive/371ecb089d96d60e5208ec2d25c9b4f0860e556c.tar.gz`
- `https://chromium.googlesource.com/chromium/src/buildtools/+archive/24b075a4d7ea447126ff322e3e8bfecb78012b75.tar.gz`
- `https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp/+archive/14a5b78f39684f85a19690e63c722d6bb680f2cd.tar.gz`
- `https://skia.googlesource.com/skia/+archive/a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0.tar.gz`
- `https://chromium.googlesource.com/external/github.com/fastfloat/fast_float/+archive/cb1d42aaa1e14b09e1452cfdef373d051b8c02a4.tar.gz`
- `https://chromium.googlesource.com/chromium/src/tools/win/+archive/24494b071e019a2baea4355d9870ffc5fc0bbafe.tar.gz`
- `https://github.com/chromium/pdfium/archive/a84323421e94f484faca52dd9d027934eba42ab8.tar.gz`

**Archivnamen:**

- `pdfium-deps\build-371ecb089d96d60e5208ec2d25c9b4f0860e556c.tar.gz`
- `pdfium-deps\buildtools-24b075a4d7ea447126ff322e3e8bfecb78012b75.tar.gz`
- `pdfium-deps\abseil-cpp-14a5b78f39684f85a19690e63c722d6bb680f2cd.tar.gz`
- `pdfium-deps\skia-a1ff58d34294ad2c1d2e74b796cd9b658be2cfe0.tar.gz`
- `pdfium-deps\fast_float-cb1d42aaa1e14b09e1452cfdef373d051b8c02a4.tar.gz`
- `pdfium-deps\tools-win-24494b071e019a2baea4355d9870ffc5fc0bbafe.tar.gz`
- `pdfium-a84323421e94f484faca52dd9d027934eba42ab8.tar.gz`

## Patches, Workarounds und Adapter

- `pdfium\2025.11.19\icu.BUILD.gn`
- `pdfium\2025.11.19\libjpeg_turbo.BUILD.gn`
- `pdfium\2025.11.19\zlib.BUILD.gn`
- `pdfium\2025.11.19\libpng.BUILD.gn`
- `pdfium\2025.11.19\bcc64x.BUILD.gn`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_PYTHON={Tool:python}`
- `CB_GN_ARGUMENT_ADAPTER={BuildFileDir}\\programs\\bcc64x\\gn_argument_adapter.py`
- `DEPOT_TOOLS_WIN_TOOLCHAIN=0`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_PDFIUM_ICU_INCLUDE={InstallRoot}\\packages\\icu\\78.3\\include`
- `CB_PDFIUM_ICU_LIB={InstallRoot}\\packages\\icu\\78.3\\lib\\win64\\{Configuration}\\{PdfiumIcuImport}.lib`
- `CB_PDFIUM_FREETYPE_INCLUDE={InstallRoot}\packages\freetype\2.14.3\include\freetype2`
- `CB_PDFIUM_FREETYPE_LIB={InstallRoot}\packages\freetype\2.14.3\lib\win64\{Configuration}\{PdfiumFreeTypeImport}.lib`
- `CB_PDFIUM_ZLIB_INCLUDE={InstallRoot}\packages\zlib\1.3.2\include`
- `CB_PDFIUM_ZLIB_LIB={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{PdfiumZlibImport}.lib`
- `CB_PDFIUM_PNG_INCLUDE={InstallRoot}\packages\libpng\1.6.58\include`
- `CB_PDFIUM_PNG_LIB={InstallRoot}\packages\libpng\1.6.58\lib\win64\{Configuration}\{PdfiumPngImport}.lib`
- `CB_PDFIUM_JPEG_INCLUDE={InstallRoot}\packages\libjpeg-turbo\3.2.0\include`
- `CB_PDFIUM_JPEG_LIB={InstallRoot}\packages\libjpeg-turbo\3.2.0\lib\win64\{Configuration}\{PdfiumJpegImport}.lib`

**Technische Execute-Actions:**

- `prepare-chromium-build-dir`
- `extract-chromium-build`
- `prepare-chromium-buildtools-dir`
- `extract-chromium-buildtools`
- `prepare-abseil-cpp-source-dir`
- `extract-abseil-cpp-upstream`
- `prepare-skia-source-dir`
- `extract-skia-upstream`
- `prepare-fast_float-source-dir`
- `extract-fast_float-upstream`
- `prepare-chromium-tools-win-dir`
- `extract-chromium-tools-win`
- `prepare-icu-gn-adapter`
- `generate-pdfium-build-timestamp`
- `generate-offline-gclient-args`
- `verify-offline-gn-inputs`
- `adapt-skia-dwrite-refcount-bcc64x`
- `adapt-skia-directwrite-com-bcc64x`
- `adapt-pdfium-win32-aligned-memory`
- `prepare-jpeg-gn-dir`
- `prepare-zlib-gn-dir`
- `prepare-libpng-gn-dir`
- `install-jpeg-gn-adapter`
- `install-zlib-gn-adapter`
- `install-libpng-gn-adapter`
- `bind-local-freetype`
- `exclude-standalone-test-fonts`
- `adapt-local-clang-revision`
- `prepare-bcc64x-toolchain-dir`
- `install-bcc64x-toolchain`
- `verify-pdfium-gn-adapters`
- `verify-bcc64x-ninja-toolchain`
- `ninja-build`

**Targets:**

- `gn-gen`

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{CurrentArtifact}\\third_party\\icu\\BUILD.gn`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\pdfium.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses\pdfium\LICENSE`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\sbom\bundled-components.json`

**Consumer-Smokes:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Consumer: gegenwärtiger Verifikationsstatus

PDFium befindet sich am 17.10.2026 noch in der GN/Ninja-Buildintegration. Erfolgreiche GN-Generierung ist **kein** Nachweis eines vollständig gebauten und getesteten Pakets. Shared Source wird ausschließlich im Source-State verändert, nicht parallel durch Debug und Release.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/pdfium/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
