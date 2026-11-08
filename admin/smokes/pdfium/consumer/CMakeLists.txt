cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokePDFium LANGUAGES CXX)
find_package(PDFium CONFIG REQUIRED)
add_executable(pdfium-consumer main.cpp)
set_property(TARGET pdfium-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(pdfium-consumer PRIVATE cxx_std_23)
target_link_libraries(pdfium-consumer PRIVATE PDFium::pdfium)

# Deploy the exact selected PDFium/dependency package DLLs beside the smoke EXE.
# Package roots are supplied by BuildEngine from the XML dependency closure.
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/consumer/BuildEngineRuntime.cmake")
if(NOT DEFINED ADECC_RUNTIME_PACKAGE_ROOTS OR ADECC_RUNTIME_PACKAGE_ROOTS STREQUAL "")
   message(FATAL_ERROR "PDFium smoke requires ADECC_RUNTIME_PACKAGE_ROOTS")
endif()
# The retired Skia 144 DLL may remain from a previous smoke run: purge only
# that exact filename. Runtime staging still rejects genuine DLL collisions.
adecc_stage_runtime(pdfium-consumer
   PACKAGE_ROOTS ${ADECC_RUNTIME_PACKAGE_ROOTS}
   EXCLUDE_DLLS skia144.dll)

# Skia 153's ICU data is a runtime resource, not a DLL. Stage the exact
# declared Skia package asset, without PATH or latest-version discovery.
# The XML dependency resolver supplies the exact selected Skia package root.
# Do not guess a version or search PATH / newest installed packages.
if(NOT DEFINED ENV{ADECC_PDFIUM_SKIA_ROOT} OR
   "$ENV{ADECC_PDFIUM_SKIA_ROOT}" STREQUAL "")
   message(FATAL_ERROR "PDFium smoke requires the resolved Skia package root")
endif()
set(theSkia153Root "$ENV{ADECC_PDFIUM_SKIA_ROOT}")
if(NOT IS_DIRECTORY "${theSkia153Root}")
   message(FATAL_ERROR "Selected Skia package root is missing: ${theSkia153Root}")
endif()
file(TO_CMAKE_PATH "${theSkia153Root}" strSelectedSkiaRoot)
set(bFoundSelectedSkiaRoot FALSE)
foreach(thePackage IN LISTS ADECC_RUNTIME_PACKAGE_ROOTS)
   file(TO_CMAKE_PATH "${thePackage}" strPackagePath)
   if(strPackagePath STREQUAL strSelectedSkiaRoot)
      set(bFoundSelectedSkiaRoot TRUE)
      break()
   endif()
endforeach()
if(NOT bFoundSelectedSkiaRoot)
   message(FATAL_ERROR "Selected Skia package root is absent from PDFium's runtime closure")
endif()
# Stage on every build, not only when the consumer executable is relinked.
add_custom_target(pdfium-skia153-data ALL
   COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${theSkia153Root}/bin/win64/$<CONFIG>/icudtl.dat"
      "$<TARGET_FILE_DIR:pdfium-consumer>/icudtl.dat"
   DEPENDS pdfium-consumer
   COMMENT "Refresh installed Skia 153 ICU data"
   VERBATIM)


# Explicit loader preflight: this executable intentionally does not import
# PDFium, so a Windows loader failure can be distinguished from the smoke
# executable itself failing before entering main().
add_executable(pdfium-loader-probe
   "${CMAKE_CURRENT_LIST_DIR}/../../../programs/bcc64x/RuntimeLoaderProbe.cpp")
set_property(TARGET pdfium-loader-probe PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(pdfium-loader-probe PRIVATE cxx_std_23)
# A loader probe is meaningful only after the PDFium dependency runtime closure is staged.
add_dependencies(pdfium-loader-probe buildengine-stage-pdfium-consumer pdfium-skia153-data)

