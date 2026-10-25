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
