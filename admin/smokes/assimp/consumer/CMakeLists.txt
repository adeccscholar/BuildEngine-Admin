cmake_minimum_required(VERSION 3.31)
project(buildengine_assimp_consumer LANGUAGES CXX)

if(NOT DEFINED ASSIMP_PACKAGE_ROOT OR ASSIMP_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "ASSIMP_PACKAGE_ROOT is required")
endif()
if(NOT DEFINED ZLIB_PACKAGE_ROOT OR ZLIB_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "ZLIB_PACKAGE_ROOT is required")
endif()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Release|Debug)$")
   message(FATAL_ERROR "The Assimp consumer requires an explicit Release or Debug configuration")
endif()

add_executable(assimp-consumer main.cpp)
target_compile_features(assimp-consumer PRIVATE cxx_std_23)

list(PREPEND CMAKE_PREFIX_PATH "${ASSIMP_PACKAGE_ROOT}" "${ZLIB_PACKAGE_ROOT}")
find_package(assimp CONFIG REQUIRED PATHS "${ASSIMP_PACKAGE_ROOT}" NO_DEFAULT_PATH)
target_link_libraries(assimp-consumer PRIVATE assimp::assimp)

# Use the existing BuildEngine deployment mechanism to stage the exact
# installed shared libraries next to the consumer executable. This includes
# libassimpd.dll in Debug, its Release counterpart, and the managed zlib DLL.
# Do not use a global PATH, scan unrelated installations, or disable the
# executable's real model-import operation.
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/consumer/BuildEngineRuntime.cmake")
if(NOT DEFINED ADECC_RUNTIME_PACKAGE_ROOTS OR
   "${ADECC_RUNTIME_PACKAGE_ROOTS}" STREQUAL "")
   message(FATAL_ERROR "Missing BuildEngine-managed smoke runtime dependency closure")
endif()
adecc_stage_runtime(assimp-consumer
   PACKAGE_ROOTS ${ADECC_RUNTIME_PACKAGE_ROOTS})
