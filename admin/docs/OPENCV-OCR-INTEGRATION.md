<!-- SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH -->
<!-- SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0 -->

# OpenCV Camera and OCR Integration (BCC64X)

## Scope and version policy

The production XML catalog in [build-libraries.xml](../build-libraries.xml) is the **only executable package configuration**. OpenCV remains version **5.0.0**. New source packages are **Leptonica 1.87.0** and **Tesseract 5.5.3**, each built as a dynamic library with BCC64X, CMake, and Ninja. No independent PowerShell or Python build recipe is introduced.

These changes have been committed on **main**, but are **not yet verified by a Windows BCC64X build or a successful smoke**. Explicitly treat the new OCR integration as provisional until those jobs pass.

## OpenCV expansion

The XML `BUILD_LIST` adds `ml` (machine learning) and `stitching` (panorama processing) to the existing core, image-processing, calibration, video, object detection, DNN, codecs, and UI modules. The following Windows camera options are enabled:

- `WITH_MSMF=OFF`: Microsoft Media Foundation is postponed: upstream `cap_msmf.cpp` requires MSVC-specific `<comdef.h>`, `_COM_SMARTPTR_TYPEDEF`, and `_com_error`, unavailable to the native BCC64X toolchain. A dedicated COM lifetime and import-library port is required before re-enabling it.
- `WITH_DSHOW=ON`: DirectShow capture, subject to BCC64X build and runtime verification.
- `WITH_MSMF_DXVA=OFF`: do not require hardware acceleration.
- `VIDEOIO_ENABLE_PLUGINS=OFF`: use the configured built-in backends, not runtime-downloaded plugins.

FFmpeg, GStreamer, CUDA, OpenCL, DirectML, and OpenCV's optional external ONNX Runtime remain disabled. OpenCV's internal DNN code stays enabled. A video input **backend** is not the same thing as an attached camera: automated CI checks backend registration; a separate interactive test must verify actual camera access and Windows privacy permissions.

The existing OpenCV consumer now requires `ml` and `stitching`, creates the corresponding objects, and reports both camera backends but requires registration only for `CAP_DSHOW`. It still verifies PNG image encoding/decoding and oneTBB runtime selection. Failing registration is an intentional smoke failure, not a silently skipped test.

## Leptonica as a separate DLL package

The installed Leptonica package supplies image preprocessing, transformations, and pixel operations. Upstream tag **1.87.0** is downloaded from its release archive with SHA-256 verification. The build uses `BUILD_SHARED_LIBS=ON`, `SW_BUILD=OFF`, and no utility programs.

Image codecs declared by the catalog are zlib, libpng, libjpeg-turbo, and libtiff. GIF, WebP, and OpenJPEG are disabled to avoid pulling in unapproved optional libraries. These optional formats can be introduced only with their own dependency contracts. Leptonica's original license is retained alongside the installed package.

The Leptonica consumer creates an 8-bit `PIX`, scales it, checks the resulting dimensions, and releases both images. This verifies real C API calls across the dynamic-library boundary, not only a header compile.

## Tesseract as a separate DLL package

Tesseract **5.5.3** is sourced from a pinned upstream commit matching its signed/tagged release reference. It depends on the installed Leptonica package and uses `find_package(Leptonica CONFIG)`.

The initial build disables training tools, upstream unit tests, optional libarchive/libcurl/TIFF integration in Tesseract itself, compiler-native optimization, and the upstream PCH optimization. Leptonica still retains TIFF image decoding. OpenMP is disabled initially to isolate the DLL/ABI integration; it may be enabled only after the basic BCC64X consumer passes. The Tesseract application and library are installed by upstream CMake with the configured package prefix.

The consumer links against `Tesseract::libtesseract`, obtains the API version, creates a Leptonica image, and cleans up both objects. This is deliberately a **model-free ABI smoke**.

## OCR language models

**Tesseract cannot recognize text without a suitable `.traineddata` model.** Language data is a separate data dependency, not an additional C++ library and not proof of a successful Tesseract runtime integration. It must be provisioned independently using pinned, hash-verified assets with their original notices, preferably from official `tessdata_fast` or `tessdata_best` collections, after the BCC64X library consumers pass.

A subsequent deterministic OCR test should use a generated, non-personal English text image and a pinned English model. It should validate character content and optionally confidence, rather than making unverifiable claims about reading real-world plates or identifying people.

## BuildEngine execution and verification

The libraries are inserted into `admin/build-libraries.xml`, preserving `leptonica -> tesseract` dependency ordering. BuildEngine controls their download, source extraction, CMake configuration, build, install, publish, and consumer smoke. Debug and Release builds use separate library directories; DLL dependencies remain dynamic.

Mandatory acceptance checks before treating integration as finished:

1. Parse the XML contract with the actual BuildEngine configuration validator and verify that all declared libraries/dependencies resolve.
2. Rebuild both OpenCV variants and verify `opencv_ml500` and `opencv_stitching500` import libraries and DLLs, plus DirectShow registration in the consumer smoke.
3. Build both Leptonica variants and verify headers, exported CMake targets, import libraries, DLLs, and license text.
4. Build both Tesseract variants and verify `TesseractConfig.cmake`, `Tesseract::libtesseract`, headers, import libraries, DLL, and correct closure to Leptonica.
5. Run the model-free C and C++ consumers without leaking other installed versions through PATH.
6. Stage the actual PE-import closure. Verify SHA-256, package SBOM, and original licenses.
7. Only then introduce pinned `eng.traineddata`, reproducible OCR assertions, and the optional user-permission-based webcam test.

**Known unverified integration risks:** Windows BCC64X/Clang support for DirectShow, with MSMF still blocked by MSVC COM helpers, Leptonica's package target export in Release/Debug, import-library filename conventions, and Tesseract's upstream Windows build. Any failure must be fixed in the XML-driven build contract and versioned source patches, not by falling back to MSVC, static linking, ad-hoc scripts, or PATH workarounds.

## Microsoft WIL as a first-class header-only BuildEngine package

The Windows Implementation Library (WIL) has been added as a separately
versioned header-only package (`wil@1.0.260126.7`). The source is pinned
to the official GitHub release commit `cbf677fb0a942557d08fd129f4c106a76247b2ec`.
Its upstream CMake interface target `WIL::WIL` is installed without
building a binary library. WIL uses the MIT license; both `LICENSE` and
`ThirdPartyNotices.txt` are retained.

A native BCC64X consumer in `admin/smokes/wil/consumer` compiles
`wil/com.h` and `wil/resource.h`, then tests COM interface ownership:
attach, AddRef/copy, move, QueryInterface, detach, and final Release.
The consumer does not use a synthetic replacement for WIL, any MSVC
C++ runtime, or a camera. It is a **mandatory compatibility gate** before
introducing WIL into OpenCV's Media Foundation code.

OpenCV MSMF remains disabled until WIL's real BCC64X smoke passes.
The subsequent versioned OpenCV patch must replace the upstream
`_COM_SMARTPTR_TYPEDEF` / `_com_ptr_t` dependencies with
`wil::com_ptr_nothrow<T>` and explicit `put()`, `get()`, `reset()`,
`query_to()`, and `attach()` operations, preserving COM ownership rules.
It must also replace the upstream `_com_error` catches and declare
Media Foundation system libraries explicitly for the non-MSVC linker.
Only after **both** compilation and native runtime tests pass may
`WITH_MSMF=ON` be restored.

**Status: not verified.** The repository has no native BCC64X runtime
available to run the consumer. Enabling MSMF now would bypass the
explicit compatibility gate and repeat the previous failed build.

## Integration fixes after first OpenCV / WIL / Tesseract smoke run

The first integration run exposed three independent issues:

1. **Tesseract**: the installed upstream `Tesseract::libtesseract`
   export does not provide the include root expected by the consumer.
   The consumer now takes `Tesseract_INCLUDE_DIRS` from the actual
   `TesseractConfig.cmake`, verifies `tesseract/baseapi.h` at that
   location and adds it explicitly. No hardcoded system include path.
2. **WIL**: `wil/com.h` includes the Windows SDK's
   `WeakReference.h`, which resides under the SDK `winrt` include
   directory. The WIL consumer derives that SDK version and root from
   BuildEngine's registered `windows-rc` executable, verifies the
   header and adds only that include root. WIL itself remains unchanged.
   The compiler/runtime result is still unverified.
3. **OpenCV**: OpenCV 5.0 moved the `ml` module to
   `opencv_contrib`. The source contract now downloads the separately
   pinned matching `opencv_contrib` 5.0.0 commit
   `755e50675d97db9b7d449d8bd6b09888646f6c6e` and extracts it
   alongside the primary OpenCV source. The OpenCV configure step uses
   `OPENCV_EXTRA_MODULES_PATH` to build its `ml` module. This
   retains the complete intended 17-module list rather than deleting
   an expected DLL or bypassing the install requirement. Source patch
   checks remain anchored to the primary OpenCV workspace after the
   second archive extraction.

All changes are made through the production XML contract and versioned
consumer sources on `main`. A passing BCC64X build, registered DirectShow
backend, and full PE runtime closure are still required before calling the
integration complete.

## DLL loader and WIL compatibility follow-up

In the next BuildEngine run, Tesseract's model-free consumer built but exited
with Windows `0xC0000135` before producing smoke evidence. This code means
a DLL could not be loaded; it does **not** identify the specific DLL.
The original upstream Tesseract install script hardcodes `bin`, whereas
BuildEngine's package-scope smoke assembles `PATH` from version-specific
`bin/win64/Release` directories for the package and its entire declared
dependency closure. A versioned source patch now changes Tesseract's DLL and
CLI install destinations to `CMAKE_INSTALL_BINDIR` and the XML selects
`bin/win64/{Configuration}`. The loader's exact missing DLL remains
**unverified** until the smoke reruns. Do not work around this by copying
MSVC DLLs, adding arbitrary PATH locations, or linking statically.

WIL's native consumer encountered `WriteRelease` type conflicts in the
upstream MinGW compatibility section of `include/wil/common.h`. A narrow
source patch guards only that group of fallback atomic declarations with
`ADECC_BCC64X_WIL_SDK_ATOMICS`. The BCC64X smoke defines the flag so it uses
the installed Windows SDK facilities instead. Other consumers retain the
unmodified default behavior. The Windows SDK and subsequent WIL compile
results must still be validated by a real BCC64X run.

OpenCV's ML import check also failed because the existing FSM evidence
resumed at `install` and did not rebuild the previously absent
`opencv_contrib` module. The OpenCV catalog timestamp has been increased
to invalidate the source and build evidence and allow a fresh
OpenCV 5.0.0 + pinned contrib build before installation. The ML DLL
`require` remains mandatory.

**Acceptance:** A green installation alone is not enough; the Tesseract
process must execute and print its checks, the WIL COM lifetime consumer
must pass, and both OpenCV variants must build, install and pass the
17-module consumer smoke.


## OCR recognition smoke and WIL COM correction (October 2026)

New `tesseract@5.5.3` smoke `ocr` checks actual text `OCR TEST 4711` via pinned `tessdata_fast` model commit `87416418657359cb625c412a48b6e1d6d41c29bd` (SHA-256 `7d4322bd2a7749724879683fc3912cb542f19906c83bcc1a52132556427170b2`). The existing model-free consumer remains separate. Model retrieval uses BuildEngine XML and SHA-256 validation; the PNG is synthetic. Native BCC64X execution is pending.

WIL's original query check wrongly expected the compatible IUnknown-to-IUnknown fast path to call QueryInterface. Upstream uses AddRef directly. The smoke now validates this path and tests explicit IID QueryInterface independently. No WIL upstream patch was changed.
