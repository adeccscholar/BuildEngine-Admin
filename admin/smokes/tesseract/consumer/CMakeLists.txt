cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeTesseract LANGUAGES CXX)
find_package(Leptonica 1.87 CONFIG REQUIRED)
find_package(Tesseract 5.5 CONFIG REQUIRED)
add_executable(tesseract-consumer main.cpp)
set_property(TARGET tesseract-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(tesseract-consumer PRIVATE cxx_std_23)
# Upstream Tesseract 5.5 exports Tesseract_INCLUDE_DIRS but its imported
# target does not currently publish an INSTALL_INTERFACE include directory.
# Read the location from the installed package, never infer an SDK path.
if(NOT Tesseract_INCLUDE_DIRS OR NOT EXISTS "${Tesseract_INCLUDE_DIRS}/tesseract/baseapi.h")
   message(FATAL_ERROR "The installed Tesseract package has no tesseract/baseapi.h at Tesseract_INCLUDE_DIRS='${Tesseract_INCLUDE_DIRS}'")
endif()
target_include_directories(tesseract-consumer PRIVATE
   "${Tesseract_INCLUDE_DIRS}"
   "${Leptonica_INCLUDE_DIRS}")
target_link_libraries(tesseract-consumer PRIVATE Tesseract::libtesseract)
