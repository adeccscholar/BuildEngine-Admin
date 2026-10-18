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
adecc_stage_runtime(pdfium-consumer PACKAGE_ROOTS ${ADECC_RUNTIME_PACKAGE_ROOTS})
