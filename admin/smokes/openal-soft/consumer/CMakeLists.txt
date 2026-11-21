cmake_minimum_required(VERSION 3.31)
project(buildengine_openal_soft_consumer LANGUAGES CXX)

if(NOT DEFINED OPENAL_PACKAGE_ROOT OR OPENAL_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "OPENAL_PACKAGE_ROOT is required")
endif()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Release|Debug)$")
   message(FATAL_ERROR "OpenAL Soft smoke requires explicit Release or Debug")
endif()

find_library(OPENAL_LIBRARY
   NAMES OpenAL32 libOpenAL32 OpenAL libopenal
   PATHS "${OPENAL_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}"
   NO_DEFAULT_PATH REQUIRED)
message(STATUS "OpenAL smoke import library: ${OPENAL_LIBRARY}")
add_executable(openal-soft-consumer main.cpp)
target_compile_features(openal-soft-consumer PRIVATE cxx_std_23)
target_include_directories(openal-soft-consumer PRIVATE "${OPENAL_PACKAGE_ROOT}/include")
target_link_libraries(openal-soft-consumer PRIVATE "${OPENAL_LIBRARY}")

include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/consumer/BuildEngineRuntime.cmake")
if(NOT DEFINED ADECC_RUNTIME_PACKAGE_ROOTS OR
   "${ADECC_RUNTIME_PACKAGE_ROOTS}" STREQUAL "")
   message(FATAL_ERROR "Missing BuildEngine-managed OpenAL runtime dependency closure")
endif()
adecc_stage_runtime(openal-soft-consumer
   PACKAGE_ROOTS ${ADECC_RUNTIME_PACKAGE_ROOTS})
