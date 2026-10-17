# skia 153: Migration und BCC64X-Integration

[TOC|Content]

**Status:** Vertragsdokumentation, Stand 17.10.2026. Die Existenz eines BuildEngine-Vertrags beweist für sich allein keinen erfolgreichen Clean-Room-Lauf. Individuelle Testresultate sind anhand der FSM- und Log-Evidence zu verifizieren.

**Quelle:** `admin/build-libraries.xml`, `<library id="skia" version="153">`. **Compilerziel:** RAD Studio 13, BCC64X, Windows Win64. **Runtime:** dynamisch (`-tR`) für EXE/DLL/MODULE.

## Motivation und Bauentscheidungen

Das Ziel ist die reproduzierbare Bereitstellung als versioniertes BuildEngine-Paket. Der XML-Vertrag ist für tatsächlich gewählte Quellen, Schritte, Abhängigkeiten und Flags maßgeblich. Historische Gründe sind nur dann als Entscheidung dokumentiert, wenn eine Quelle sie belegt.

- Broad Windows desktop profile. BuildEngine-provided external dependencies: OpenGL 26.2.1 zlib 1.3.2 Brotli 1.2.0 libjpeg-turbo 3.2.0 libpng 1.6.58 HarfBuzz 14.4.0 FreeType 2.14.3 These components are NOT downloaded or rebuilt inside Skia. Their exact package versions are represented by BuildEngine dependencies above. Bundled Skia dependencies remain pinned to the commits select…
- Generic BuildEngine action. Inspect every pinned sibling download before any expensive source extraction or build starts.
- BuildEngine package dependencies are deliberately
- GitHub pinned revision, root folder SPIRV-Headers-942fe4b988359a0750b79f0ae7ed735994d3147d. Verify pinned source before copying and remove the staging root.
- BCC64X compatibility / system integration patches.
- Propagate raw BCC64X linker flags from Skia system dependency targets through source_set/group dependency chains to the final shared-library link.
- Restore the pinned Skia Vulkan debug validator source/dependency condition. bcc64x-desktop.patch had incorrectly tied this block to skia_enable_spirv_validation.
- Route Brotli, libjpeg-turbo and libpng through the already built and tested BuildEngine packages. Keep each integration in an independent patch so every patch can be checked against exactly one pinned upstream GN file.
- FreeType BuildEngine package integration.
- HarfBuzz BuildEngine package integration.
- libgav1 must use its Windows threading implementation.
- Embarcadero D3D12 structure-return ABI compatibility.
- Runtime DLL resolution for all BuildEngine package dependencies. They remain separate packages; no dependency DLL is copied into the Skia package here.
- BCC64X target toolchain.

## Versionierte Abhängigkeiten

- `opengl 26.2.1`
- `zlib 1.3.2`
- `brotli 1.2.0`
- `libjpeg-turbo 3.2.0`
- `libpng 1.6.58`
- `harfbuzz 14.4.0`
- `freetype 2.14.3`

## Beschaffung und reproduzierbare Quellen

**Upstream-Downloads:**

- `https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator/archive/169895d529dfce00390a20e69c2f516066fe7a3b.tar.gz`
- `https://android.googlesource.com/platform/external/dng_sdk.git/+archive/dbe0a676450d9b8c71bf00688bb306409b779e90.tar.gz`
- `https://chromium.googlesource.com/external/github.com/libexpat/libexpat.git/+archive/6154446fccefbf3ca644894f598969113b0c7bcd.tar.gz`
- `https://chromium.googlesource.com/external/github.com/google/highway.git/+archive/457c891775a7397bdb0376bb1031e6e027af1c48.tar.gz`
- `https://chromium.googlesource.com/chromium/deps/icu.git/+archive/d578f2e8b7bd5938e21cfb6bf15c079e0aa5b738.tar.gz`
- `https://github.com/AOMediaCodec/libavif/archive/55aab4ac0607ab651055d354d64c4615cf3d8000.tar.gz`
- `https://chromium.googlesource.com/codecs/libgav1.git/+archive/5cf722e659014ebaf2f573a6dd935116d36eadf1.tar.gz`
- `https://chromium.googlesource.com/external/gitlab.com/wg1/jpeg-xl.git/+archive/332feb17d17311c748445f7ee75c4fb55cc38530.tar.gz`
- `https://chromium.googlesource.com/webm/libwebp.git/+archive/845d5476a866141ba35ac133f856fa62f0b7445f.tar.gz`
- `https://chromium.googlesource.com/libyuv/libyuv.git/+archive/d248929c059ff7629a85333699717d7a677d8d96.tar.gz`
- `https://android.googlesource.com/platform/external/piex.git/+archive/bb217acdca1cc0c16b704669dd6f91a1b509c406.tar.gz`
- `https://github.com/google/wuffs-mirror-release-c/archive/e3f919ccfe3ef542cfc983a82146070258fb57f8.tar.gz`
- `https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Cross/+archive/b8fcf307f1f347089e3c46eb4451d27f32ebc8d3.tar.gz`
- `https://github.com/KhronosGroup/SPIRV-Headers/archive/942fe4b988359a0750b79f0ae7ed735994d3147d.tar.gz`
- `https://github.com/KhronosGroup/SPIRV-Tools/archive/388f5af1a6d4a4df4ab9bf69a1817255952e6ef1.tar.gz`
- `https://chromium.googlesource.com/external/github.com/KhronosGroup/glslang/+archive/1d47ffa8ac4374a19b302021e216a20f22a3de92.tar.gz`
- `https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Headers/+archive/f9973cd97e6f3584707e7ef1c425e336f1b92a5b.tar.gz`
- `https://chromium.googlesource.com/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/+archive/eb744ea7a2b17040121b4bbb4d6f9e8a77e3cae7.tar.gz`
- `https://github.com/google/skia/archive/2eed75b956045eb8603d3690a1e84bc582a2135d.zip`

**Archivnamen:**

- `skia-deps\d3d12allocator-169895d529dfce00390a20e69c2f516066fe7a3b.tar.gz`
- `skia-deps\dng_sdk-dbe0a676450d9b8c71bf00688bb306409b779e90.tar.gz`
- `skia-deps\expat-6154446fccefbf3ca644894f598969113b0c7bcd.tar.gz`
- `skia-deps\highway-457c891775a7397bdb0376bb1031e6e027af1c48.tar.gz`
- `skia-deps\icu-d578f2e8b7bd5938e21cfb6bf15c079e0aa5b738.tar.gz`
- `skia-deps\libavif-55aab4ac0607ab651055d354d64c4615cf3d8000.tar.gz`
- `skia-deps\libgav1-5cf722e659014ebaf2f573a6dd935116d36eadf1.tar.gz`
- `skia-deps\libjxl-332feb17d17311c748445f7ee75c4fb55cc38530.tar.gz`
- `skia-deps\libwebp-845d5476a866141ba35ac133f856fa62f0b7445f.tar.gz`
- `skia-deps\libyuv-d248929c059ff7629a85333699717d7a677d8d96.tar.gz`
- `skia-deps\piex-bb217acdca1cc0c16b704669dd6f91a1b509c406.tar.gz`
- `skia-deps\wuffs-e3f919ccfe3ef542cfc983a82146070258fb57f8.tar.gz`
- `skia-deps\spirv-cross-b8fcf307f1f347089e3c46eb4451d27f32ebc8d3.tar.gz`
- `skia-deps\spirv-headers-942fe4b988359a0750b79f0ae7ed735994d3147d.tar.gz`
- `skia-deps\spirv-tools-388f5af1a6d4a4df4ab9bf69a1817255952e6ef1.tar.gz`
- `skia-deps\glslang-1d47ffa8ac4374a19b302021e216a20f22a3de92.tar.gz`
- `skia-deps\vulkan-headers-f9973cd97e6f3584707e7ef1c425e336f1b92a5b.tar.gz`
- `skia-deps\vulkanmemoryallocator-eb744ea7a2b17040121b4bbb4d6f9e8a77e3cae7.tar.gz`
- `skia-2eed75b956045eb8603d3690a1e84bc582a2135d.zip`

## Patches, Workarounds und Adapter

- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-gn.patch`
- `{LibraryVersion}\bcc64x-gn.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-system-link-propagation.patch`
- `{LibraryVersion}\bcc64x-system-link-propagation.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-desktop.patch`
- `{LibraryVersion}\bcc64x-desktop.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-debug-spirv-validator.patch`
- `{LibraryVersion}\bcc64x-debug-spirv-validator.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-skarenaalloc-export.patch`
- `{LibraryVersion}\bcc64x-skarenaalloc-export.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-zlib-link.patch`
- `{LibraryVersion}\bcc64x-zlib-link.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-system-brotli.patch`
- `{LibraryVersion}\bcc64x-system-brotli.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-system-libjpeg-turbo.patch`
- `{LibraryVersion}\bcc64x-system-libjpeg-turbo.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-system-libpng.patch`
- `{LibraryVersion}\bcc64x-system-libpng.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-system-freetype.patch`
- `{LibraryVersion}\bcc64x-system-freetype.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-harfbuzz-system.patch`
- `{LibraryVersion}\bcc64x-harfbuzz-system.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-libgav1-winthreads.patch`
- `{LibraryVersion}\bcc64x-libgav1-winthreads.patch`
- `skia\2eed75b956045eb8603d3690a1e84bc582a2135d\bcc64x-d3d-getdesc.patch`
- `{LibraryVersion}\bcc64x-d3d-getdesc.patch`

Die aufgelisteten Adapter oder Patchpfade sind aus dem Vertrag abgeleitet. Bei indirekt erzeugten Dateien ist zusätzlich die ausführende XML-Action maßgeblich. Die Reihenfolge ist aus der Sequenz der Actions abzuleiten; die Patches sind nicht beliebig vertauschbar.

## Build-Parameter und Variablen

**Explizite CMake-/Compileroptionen (Auszug der im XML vorkommenden Flags):**

- `-DWIN32_LEAN_AND_MEAN`
- `-DNOMINMAX`

**Explizite Umgebungsvariablen:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_PYTHON={Tool:python}`
- `CB_BCC64X_UCRT_COMPAT={Tool:bcc64x-ucrt-compat}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X_UCRT_COMPAT={Tool:bcc64x-ucrt-compat}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_BCC64X_UCRT_COMPAT={Tool:bcc64x-ucrt-compat}`

**Technische Execute-Actions:**

- `mkdir-d3d12allocator`
- `extract-d3d12allocator`
- `flatten-d3d12allocator-github-archive`
- `cleanup-d3d12allocator-github-root`
- `mkdir-dng-sdk`
- `extract-dng-sdk`
- `mkdir-expat`
- `extract-expat`
- `mkdir-highway`
- `extract-highway`
- `mkdir-icu`
- `extract-icu`
- `mkdir-libavif`
- `extract-libavif`
- `flatten-libavif-github-archive`
- `cleanup-libavif-github-root`
- `mkdir-libgav1`
- `extract-libgav1`
- `mkdir-libjxl`
- `extract-libjxl`
- `mkdir-libwebp`
- `extract-libwebp`
- `mkdir-libyuv`
- `extract-libyuv`
- `mkdir-piex`
- `extract-piex`
- `mkdir-wuffs`
- `extract-wuffs`
- `flatten-wuffs-github-archive`
- `cleanup-wuffs-github-root`
- `mkdir-spirv-cross`
- `extract-spirv-cross`
- `mkdir-spirv-headers`
- `extract-spirv-headers`
- `flatten-spirv-headers-github-archive`
- `cleanup-spirv-headers-github-root`
- `mkdir-spirv-tools`
- `extract-spirv-tools`
- `flatten-spirv-tools-github-archive`
- `cleanup-spirv-tools-github-root`
- `mkdir-glslang`
- `extract-glslang`
- `mkdir-vulkan-headers`
- `extract-vulkan-headers`
- `mkdir-vulkanmemoryallocator`
- `extract-vulkanmemoryallocator`
- `patch-check`
- `patch-apply`
- `system-link-propagation-patch-check`
- `system-link-propagation-patch-apply`
- `desktop-patch-check`
- `desktop-patch-apply`
- `debug-spirv-validator-patch-check`
- `debug-spirv-validator-patch-apply`
- `skarenaalloc-export-patch-check`
- `skarenaalloc-export-patch-apply`
- `zlib-link-patch-check`
- `zlib-link-patch-apply`
- `system-brotli-patch-check`
- `system-brotli-patch-apply`
- `system-libjpeg-turbo-patch-check`
- `system-libjpeg-turbo-patch-apply`
- `system-libpng-patch-check`
- `system-libpng-patch-apply`
- `system-freetype-patch-check`
- `system-freetype-patch-apply`
- `harfbuzz-system-patch-check`
- `harfbuzz-system-patch-apply`
- `libgav1-winthreads-patch-check`
- `libgav1-winthreads-patch-apply`
- `d3d-getdesc-patch-check`
- `d3d-getdesc-patch-apply`
- `fetch-gn`

**Targets:**

- Im untersuchten Vertragsabschnitt nicht explizit aufgeführt.

## Installations-, Paket- und Testkontrakt

**Explizite Zielpfade im XML (soweit als Attribute vorhanden):**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-gn.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-system-link-propagation.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-desktop.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-debug-spirv-validator.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-skarenaalloc-export.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-zlib-link.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-system-brotli.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-system-libjpeg-turbo.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-system-libpng.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-system-freetype.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-harfbuzz-system.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-libgav1-winthreads.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-d3d-getdesc.patch`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skia.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skia.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skunicode_core.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skunicode_core.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skunicode_icu.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skunicode_icu.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skshaper.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skshaper.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\svg.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\svg.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\skottie.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\skottie.lib`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\icudtl.dat`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\include`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\modules\skcms`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\modules\skunicode`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\modules\skshaper`

**Consumer-Smokes:**

- `consumer`

Die aktiven Produktpfade hängen von `{InstallRoot}`, `{BuildRoot}`, `{LibraryId}`, `{LibraryVersion}` und gegebenenfalls `{Configuration}` ab. Für Release und Debug sind die jeweils installierten Importbibliotheken und DLLs zu verwenden. Kein ungeprüfter Fallback auf global installierte SDKs.

## Consumer: ABI und GN

Den versionsgebundenen Skia-GN-Adapter und die tatsächliche Component-/Exportkonfiguration berücksichtigen. Systempakete nicht zusätzlich in einem zweiten Skia-Build bündeln. Die BCC64X-UCRT-Kompatibilität stammt aus dem Toolchain-Layer.

## Quellen, Wartung und offene Evidenz

- Normativer Vertrag: `admin/build-libraries.xml`.
- Versionierte Anpassungen: `admin/patches/skia/` und die im XML angegebenen Adapter.
- Architektur und Querverweise: `admin/server-docs/libraries.md` sowie `docs/bcc64x-library-integration-findings.md`.
- **[nicht verifiziert]** Ein lückenloser Nachweis *aller* historischen Diskussionen und Gründe ist aus dem XML allein nicht möglich. Fehlende Begründungen sind anhand von Commits und Buildlogs nachzutragen, ohne Vermutungen als Tatsachen darzustellen.
- Die maßgeblichen Build-Erfolge und -Fehler sind im BuildEngine-FSM-State und den originalen Prozesslogs nachzuweisen.
