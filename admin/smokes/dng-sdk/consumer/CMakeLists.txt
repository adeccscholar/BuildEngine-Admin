cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeDngSdk LANGUAGES CXX)

if(NOT DEFINED ENV{DNG_SDK_ROOT} OR "$ENV{DNG_SDK_ROOT}" STREQUAL "")
   message(FATAL_ERROR "DNG_SDK_ROOT must select the installed versioned package")
endif()
set(DNG_SDK_ROOT "$ENV{DNG_SDK_ROOT}")
if(NOT EXISTS "${DNG_SDK_ROOT}/include/dng_sdk/source/dng_host.h")
   message(FATAL_ERROR "Installed DNG SDK header is missing")
endif()
find_library(DNG_SDK_IMPORT_LIBRARY
   NAMES dng_sdk
   PATHS "${DNG_SDK_ROOT}/lib/win64/Release"
   NO_DEFAULT_PATH REQUIRED)

add_executable(dng-sdk-consumer main.cpp)
target_compile_features(dng-sdk-consumer PRIVATE cxx_std_23)
target_compile_definitions(dng-sdk-consumer PRIVATE
   qDNGBigEndian=0 qWinOS=1 qDNGUseXMP=0 qDNGThreadSafe=1
   qDNGUseLibJPEG=1 qDNGValidate=0 qDNGValidateTarget=1)
target_include_directories(dng-sdk-consumer PRIVATE
   "${DNG_SDK_ROOT}/include/dng_sdk/source")
target_link_libraries(dng-sdk-consumer PRIVATE "${DNG_SDK_IMPORT_LIBRARY}")
