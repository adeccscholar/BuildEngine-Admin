cmake_minimum_required(VERSION 3.31)
project(buildengine_assimp_consumer LANGUAGES CXX)

if(NOT DEFINED ASSIMP_PACKAGE_ROOT OR ASSIMP_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "ASSIMP_PACKAGE_ROOT is required")
endif()
if(NOT DEFINED ZLIB_PACKAGE_ROOT OR ZLIB_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "ZLIB_PACKAGE_ROOT is required")
endif()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Release|Debug)$")
   message(FATAL_ERROR "The Assimp consumer requires explicit Release or Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
   set(strZlibLibrary "libz.lib")
   set(strZlibDll "libz.dll")
   set(vecRetiredDlls "libzd.dll" "libassimpd.dll")
else()
   set(strZlibLibrary "libzd.lib")
   set(strZlibDll "libzd.dll")
   set(vecRetiredDlls "libz.dll" "libassimp.dll")
endif()
set(theZlibImport "${ZLIB_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/${strZlibLibrary}")
set(theZlibRuntime "${ZLIB_PACKAGE_ROOT}/bin/win64/${CMAKE_BUILD_TYPE}/${strZlibDll}")
foreach(theFile IN ITEMS "${theZlibImport}" "${theZlibRuntime}")
   if(NOT EXISTS "${theFile}")
      message(FATAL_ERROR "Expected managed ${CMAKE_BUILD_TYPE} zlib artifact missing: ${theFile}")
   endif()
endforeach()
message(STATUS "Assimp smoke zlib: ${theZlibImport}; runtime=${theZlibRuntime}")

add_executable(assimp-consumer main.cpp)
target_compile_features(assimp-consumer PRIVATE cxx_std_23)
list(PREPEND CMAKE_PREFIX_PATH "${ASSIMP_PACKAGE_ROOT}" "${ZLIB_PACKAGE_ROOT}")
find_package(assimp CONFIG REQUIRED PATHS "${ASSIMP_PACKAGE_ROOT}" NO_DEFAULT_PATH)
target_link_libraries(assimp-consumer PRIVATE assimp::assimp)

# Stage only the exact package closure and reject obsolete opposite-variant DLLs.
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/consumer/BuildEngineRuntime.cmake")
if(NOT DEFINED ADECC_RUNTIME_PACKAGE_ROOTS OR
   "${ADECC_RUNTIME_PACKAGE_ROOTS}" STREQUAL "")
   message(FATAL_ERROR "Missing BuildEngine-managed smoke runtime dependency closure")
endif()
adecc_stage_runtime(assimp-consumer
   PACKAGE_ROOTS ${ADECC_RUNTIME_PACKAGE_ROOTS}
   EXCLUDE_DLLS ${vecRetiredDlls})

# This tool runs before the smoke executable: a loader failure cannot report
# its own missing DLL. Parse actual x64 PE import descriptors for every staged
# module and fail if a Release binary imports libzd.dll (or vice versa).
add_executable(assimp-import-audit PeImportAudit.cpp)
target_compile_features(assimp-import-audit PRIVATE cxx_std_23)
add_custom_target(assimp-verify-imports ALL
   COMMAND "$<TARGET_FILE:assimp-import-audit>"
      "$<TARGET_FILE_DIR:assimp-consumer>" "${CMAKE_BUILD_TYPE}"
   DEPENDS assimp-import-audit buildengine-stage-assimp-consumer
   COMMENT "Verify Assimp/zlib PE imports against the selected configuration"
   VERBATIM)
