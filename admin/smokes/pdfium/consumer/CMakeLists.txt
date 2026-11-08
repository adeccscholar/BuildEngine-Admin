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
set(theSkia153Root "")
foreach(thePackage IN LISTS ADECC_RUNTIME_PACKAGE_ROOTS)
   file(TO_CMAKE_PATH "${thePackage}" strPackagePath)
   if(strPackagePath MATCHES "/skia/153/?$")
      set(theSkia153Root "${thePackage}")
      break()
   endif()
endforeach()
if(theSkia153Root STREQUAL "")
   message(FATAL_ERROR "PDFium runtime closure does not contain skia@153")
endif()
add_custom_command(TARGET pdfium-consumer POST_BUILD
   COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${theSkia153Root}/bin/win64/$<CONFIG>/icudtl.dat"
      "$<TARGET_FILE_DIR:pdfium-consumer>/icudtl.dat"
   COMMENT "Stage installed Skia 153 ICU data"
   VERBATIM)


# Explicit loader preflight: this executable intentionally does not import
# PDFium, so a Windows loader failure can be distinguished from the smoke
# executable itself failing before entering main().
add_executable(pdfium-loader-probe
   "${CMAKE_CURRENT_LIST_DIR}/../../../programs/bcc64x/RuntimeLoaderProbe.cpp")
set_property(TARGET pdfium-loader-probe PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(pdfium-loader-probe PRIVATE cxx_std_23)
# A loader probe is meaningful only after the PDFium dependency runtime closure is staged.
add_dependencies(pdfium-loader-probe buildengine-stage-pdfium-consumer)

