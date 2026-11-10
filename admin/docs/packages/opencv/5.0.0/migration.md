## Updated BuildEngine contract (10 November 2026)

**Declared configuration; no new test PASS.**

### Complete transitive dependencies

```mermaid
flowchart LR
   N0["opencv 5.0.0"]
   N1["onetbb 2023.2.0"]
   N2["llvm-openmp 20.1.7"]
   N3["zlib 1.4"]
   N0 --> N1
   N0 --> N2
   N0 --> N3
```

Direkt: `onetbb@2023.2.0`, `llvm-openmp@20.1.7`, `zlib@1.4`

### Current configuration

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DOPENCV_INCLUDE_INSTALL_PATH:STRING=include/opencv5`
- `-DOPENCV_LIB_INSTALL_PATH:STRING=lib/win64/{Configuration}`
- `-DOPENCV_BIN_INSTALL_PATH:STRING=bin/win64/{Configuration}`
- `-DOPENCV_CONFIG_INSTALL_PATH:STRING=lib/win64/{Configuration}/cmake/opencv5`
- `-DOPENCV_SKIP_CMAKE_ROOT_CONFIG:BOOL=ON`
- `-DOPENCV_EXTRA_MODULES_PATH:PATH={Workspace}\opencv_contrib-755e50675d97db9b7d449d8bd6b09888646f6c6e\modules`
- `-DBUILD_SHARED_LIBS:BOOL=ON`
- `-DBUILD_LIST:STRING=core,imgproc,geometry,flann,features,video,photo,stereo,ptcloud,calib,ml,dnn,objdetect,imgcodecs,videoio,highgui,stitching`
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
- `-DWITH_TBB:BOOL=ON`
- `-DWITH_OPENMP:BOOL=ON`
- `-DTBB_DIR:PATH={DependencyRoot:onetbb}\lib\win64\{Configuration}\cmake\TBB`
- `-DCMAKE_PREFIX_PATH:PATH={DependencyRoot:onetbb}`
- `-DOpenMP_CXX_FLAGS:STRING=-fopenmp`
- `-DOpenMP_CXX_LIB_NAMES:STRING=omp`
- `-DOpenMP_omp_LIBRARY:FILEPATH={DependencyRoot:llvm-openmp}\lib\win64\{Configuration}\libomp.lib`
- `-DBUILD_TBB:BOOL=OFF`
- `-DWITH_FFMPEG:BOOL=OFF`
- `-DWITH_GSTREAMER:BOOL=OFF`
- `-DWITH_DSHOW:BOOL=ON`
- `-DWITH_MSMF:BOOL=OFF`
- `-DWITH_MSMF_DXVA:BOOL=OFF`
- `-DVIDEOIO_ENABLE_PLUGINS:BOOL=OFF`
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
- `-DZLIB_INCLUDE_DIR:PATH={DependencyRoot:zlib}\include`
- `-DZLIB_LIBRARY:FILEPATH={DependencyRoot:zlib}\lib\win64\{Configuration}\{ZlibLibrary}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2 -DNDEBUG`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2 -DNDEBUG`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`

---

## Historical integration notes

# opencv 5.0.0: BCC64X migration and integration record

[TOC|Content]

**Contract identity:** `opencv@5.0.0` · **Category:** graphics · **Target:** RAD Studio 13 / native BCC64X / Windows x64.

**Evidence policy:** This document describes the version-specific, declared integration and the documented engineering decisions. A switch being present does not prove the corresponding upstream tests passed. Any result that is not backed by the FSM evidence or a maintained integration report is marked **[not verified]**.

## Purpose and integration strategy

Keep upstream OpenCV's CMake build while controlling its optional graph and consuming managed zlib. Make MLAS optional for this toolchain instead of importing a foreign compiler/runtime dependency; the IPP archive path is handled with a targeted BCC64X repair.

**Why this boundary matters:** Each producer is controlled by the Admin XML and consumed via installed or published artifacts. Build variants and dependencies are explicit rather than discovered opportunistically from the developer workstation. Library product form (shared/static/header-only) is separate from the mandatory dynamic BCC64X Windows runtime for executable and DLL consumers (`-tR`).

## Dependency decisions

- `zlib@1.3.2`

These are declared direct dependencies. Transitive runtime DLLs and package CMake imports must be resolved through the BuildEngine dependency graph; they are not evidence for linking arbitrary host copies.

## Product profile: what is enabled and what is not

The entries below are actual flags in `build-libraries.xml`, not a generic upstream capability list. **Enabled** means the declared setting requests the feature; **disabled** means this integration profile explicitly excludes it. BuildEngine variables such as `{TestsEnabled}` are runtime-resolved and must not be interpreted as unconditional ON/OFF.

### Enabled switches

| Contract switch | Value |
| --- | --- |
| `OPENCV_SKIP_CMAKE_ROOT_CONFIG` | `ON` |
| `BUILD_SHARED_LIBS` | `ON` |
| `WITH_PROTOBUF` | `ON` |
| `BUILD_PROTOBUF` | `ON` |
| `WITH_JPEG` | `ON` |
| `BUILD_JPEG` | `ON` |
| `WITH_PNG` | `ON` |
| `BUILD_PNG` | `ON` |
| `WITH_TIFF` | `ON` |
| `BUILD_TIFF` | `ON` |
| `WITH_WEBP` | `ON` |
| `BUILD_WEBP` | `ON` |
| `WITH_OPENJPEG` | `ON` |
| `BUILD_OPENJPEG` | `ON` |

### Deliberately disabled switches

| Contract switch | Value |
| --- | --- |
| `BUILD_opencv_world` | `OFF` |
| `BUILD_TESTS` | `OFF` |
| `BUILD_PERF_TESTS` | `OFF` |
| `BUILD_EXAMPLES` | `OFF` |
| `BUILD_DOCS` | `OFF` |
| `BUILD_opencv_apps` | `OFF` |
| `BUILD_JAVA` | `OFF` |
| `BUILD_opencv_python3` | `OFF` |
| `WITH_IPP` | `OFF` |
| `OPENCV_DNN_MLAS` | `OFF` |
| `WITH_ONNXRUNTIME` | `OFF` |
| `PROTOBUF_UPDATE_FILES` | `OFF` |
| `OPENCV_DISABLE_THREAD_SUPPORT` | `OFF` |
| `WITH_OPENCL` | `OFF` |
| `WITH_CUDA` | `OFF` |
| `WITH_DIRECTML` | `OFF` |
| `WITH_TBB` | `OFF` |
| `WITH_OPENMP` | `OFF` |
| `WITH_FFMPEG` | `OFF` |
| `WITH_GSTREAMER` | `OFF` |
| `WITH_DSHOW` | `OFF` |
| `WITH_MSMF` | `OFF` |
| `WITH_OBSENSOR` | `OFF` |
| `WITH_QT` | `OFF` |
| `WITH_GTK` | `OFF` |
| `WITH_OPENGL` | `OFF` |
| `WITH_UNIFONT` | `OFF` |
| `BUILD_ZLIB` | `OFF` |

### Other selected build features and external settings

| Contract switch | Value |
| --- | --- |
| `OPENCV_INCLUDE_INSTALL_PATH` | `include/opencv5` |
| `OPENCV_LIB_INSTALL_PATH` | `lib/win64/{Configuration}` |
| `OPENCV_BIN_INSTALL_PATH` | `bin/win64/{Configuration}` |
| `OPENCV_CONFIG_INSTALL_PATH` | `lib/win64/{Configuration}/cmake/opencv5` |
| `BUILD_LIST` | `core,imgproc,geometry,flann,features,video,photo,stereo,ptcloud,calib,dnn,objdetect,imgcodecs,videoio,highgui` |
| `ZLIB_INCLUDE_DIR` | `{InstallRoot}\packages\zlib\1.3.2\include` |
| `ZLIB_LIBRARY` | `{InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}\{ZlibLibrary}` |


## Build architecture, variants and paths

**Selected build flow:** CMake/Ninja using BCC64X. **Declared variants:** Release, Debug. **State boundary:** Source acquisition and version-bound patches precede the build; install and publication consume completed variant artifacts.

**Representative compiler, installation and workflow arguments:**

- `-DCMAKE_TOOLCHAIN_FILE:FILEPATH={BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake`
- `-DCMAKE_MAKE_PROGRAM:FILEPATH={Tool:ninja}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_C_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_CXX_FLAGS_RELEASE:STRING=-O2`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`
- `-DCMAKE_C_FLAGS_DEBUG:STRING=-O0`
- `-DCMAKE_CXX_FLAGS_DEBUG:STRING=-O0`

**Versioned source locations:**

- `https://github.com/opencv/opencv/archive/40738fb16ceddb5fb3fea747585f7ce6abb0605b.zip`

**Declared extraction profiles:**

- `zip / opencv-40738fb16ceddb5fb3fea747585f7ce6abb0605b`

**Package-path convention:** `{InstallRoot}/packages/opencv/5.0.0/`; variant-specific binaries/import libraries generally use `lib/win64/{Configuration}` and `bin/win64/{Configuration}`, unless the actual per-library install contract specifies another layout. The XML is authoritative for each copied file; do not assume every package has all these directories.

## Difficulties, workarounds and unavoidable patches

| Version-bound patch | Problem and justification |
| --- | --- |
| `admin/patches/opencv/5.0.0/bcc64x-optional-mlas.patch` | Make the optional MLAS branch genuinely optional to avoid its unrelated compiler/runtime dependencies. |

### Declarative repairs and preparation actions

- `patch-check`
- `patch-apply`

These steps are owned by the XML-driven FSM, not by a separately invoked ad hoc build script. The presence of an action is not a license to run it concurrently from Debug and Release if it changes shared source.
### Engineering constraints recorded in the executable contract

- Temporary toolchain baseline: IPPICV MSVC /GS symbols need a verified BCC64X runtime provider.

**Patch criterion:** A source or build-system patch is justified only when the selected upstream revision cannot satisfy this BCC64X contract by configuration or packaging alone. Version-bound patches must be checked against the exact source state and applied in their declared order. Packaging omissions and generic toolchain defects belong to their respective layers instead.

## Installation, publication and consumer responsibilities

**Explicit copy/repair destinations from the contract:**

- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin\win64\{Configuration} -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\lib\win64\{Configuration} -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\include -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include`
- `{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\Release\etc\licenses -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\licenses`
- `{Workspace}\LICENSE -> {InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE`

**Declared smoke tests:**

- `consumer (Release, scope published; source smokes\opencv\consumer)`

**Selected environment inputs:**

- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `CB_LLVM_AR={Tool:llvm-ar}`
- `CB_LLVM_RANLIB={Tool:llvm-ranlib}`

**Consumer rule:** Use the actual package export/target when present and the matching Debug/Release artifact and transitive dependency closure. An installed archive or DLL alone does not prove the installed application links, loads and releases resources correctly.

## Verification and evidence boundaries

The contract includes one or more upstream tests, validation actions or consumer smokes. Their mere declaration does not establish a successful execution; use the FSM state evidence and process logs for the specific version and configuration.

**Gate checklist:** source pin/extract → ordered checked patches → BCC64X compile/link → installed headers and exact artifacts → configured test suite → external installed-package consumer → metadata/SBOM → documentation and ready state. A project-specific stage may be absent from the current contract; report it as a coverage gap rather than synthesizing a PASS.

## Maintenance and authoritative records

1. Confirm this version's active switches, source revisions and patch sequence against `admin/build-libraries.xml` before any upgrade.
2. For a version upgrade, rebase each necessary patch against the correct preceding source state; verify the complete resulting file and rerun the installed consumer.
3. Record any new failure as a specific compiler, test, packaging, ABI or runtime issue. Fix it at the owning layer and update this migration record with cause, decision and evidence.
4. Treat pending or unavailable local logs as **[not verified]**, not as historical success.

**Primary sources:** `admin/build-libraries.xml`; active version-bound patches in `admin/patches/`; `admin/server-docs/libraries.md`; `docs/bcc64x-library-integration-findings.md`. The present contract is the authority if a broad historical overview discusses an older release.
