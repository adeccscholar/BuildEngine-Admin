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

# The installed Release and Debug packages have different CMake exports.
# Never allow CMake to map a missing Release imported configuration to Debug.
set(theAssimpConfigDir
   "${ASSIMP_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/cmake/assimp-6.0")
if(NOT EXISTS "${theAssimpConfigDir}/assimpConfig.cmake")
   message(FATAL_ERROR "Assimp ${CMAKE_BUILD_TYPE} config is missing: ${theAssimpConfigDir}")
endif()
set(assimp_DIR "${theAssimpConfigDir}" CACHE PATH "Exact managed Assimp variant" FORCE)
list(PREPEND CMAKE_PREFIX_PATH "${ZLIB_PACKAGE_ROOT}")
find_package(assimp 6.0.6 EXACT CONFIG REQUIRED
   PATHS "${theAssimpConfigDir}" NO_DEFAULT_PATH)
if(NOT TARGET assimp::assimp OR NOT "${assimp_DIR}" STREQUAL "${theAssimpConfigDir}")
   message(FATAL_ERROR "Assimp variant-specific package did not export the expected target")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" strAssimpConfigUpper)
get_target_property(vecAssimpConfigs assimp::assimp IMPORTED_CONFIGURATIONS)
if(NOT vecAssimpConfigs)
   message(FATAL_ERROR "Assimp import target has no configuration metadata")
endif()
list(FIND vecAssimpConfigs "${strAssimpConfigUpper}" iAssimpConfig)
if(iAssimpConfig LESS 0)
   message(FATAL_ERROR "Assimp ${CMAKE_BUILD_TYPE} package has only imported configs "
      "${vecAssimpConfigs}; refusing wrong-variant fallback")
endif()
get_target_property(theAssimpDll assimp::assimp "IMPORTED_LOCATION_${strAssimpConfigUpper}")
get_target_property(theAssimpImplib assimp::assimp "IMPORTED_IMPLIB_${strAssimpConfigUpper}")
if(NOT theAssimpDll OR NOT EXISTS "${theAssimpDll}" OR
   NOT theAssimpImplib OR NOT EXISTS "${theAssimpImplib}")
   message(FATAL_ERROR "Assimp ${CMAKE_BUILD_TYPE} imported DLL/implib is missing: "
      "DLL=${theAssimpDll}; LIB=${theAssimpImplib}")
endif()
set(theAssimpExpectedBin "${ASSIMP_PACKAGE_ROOT}/bin/win64/${CMAKE_BUILD_TYPE}")
set(theAssimpExpectedLib "${ASSIMP_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}")
cmake_path(IS_PREFIX theAssimpExpectedBin "${theAssimpDll}" NORMALIZE bDllIsSelectedVariant)
cmake_path(IS_PREFIX theAssimpExpectedLib "${theAssimpImplib}" NORMALIZE bLibIsSelectedVariant)
if(NOT bDllIsSelectedVariant OR NOT bLibIsSelectedVariant)
   message(FATAL_ERROR "Assimp imported DLL and import library must belong to ${CMAKE_BUILD_TYPE}: "
      "DLL=${theAssimpDll}; LIB=${theAssimpImplib}")
endif()
get_filename_component(strAssimpDllName "${theAssimpDll}" NAME)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND
   strAssimpDllName MATCHES "^(lib)?assimpd[.]dll$")
   message(FATAL_ERROR "Release Assimp target exports Debug DLL: ${strAssimpDllName}")
endif()
message(STATUS "[ASSIMP] ${CMAKE_BUILD_TYPE} CMake config: ${assimp_DIR}")
message(STATUS "[ASSIMP] Selected DLL: ${theAssimpDll}")
message(STATUS "[ASSIMP] Selected import library: ${theAssimpImplib}")

add_executable(assimp-consumer main.cpp)
target_compile_features(assimp-consumer PRIVATE cxx_std_23)
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

# Stage the exact imported runtime DLL, never a guessed filename.
add_custom_target(assimp-stage-imported-runtime ALL
   COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "$<TARGET_FILE:assimp::assimp>" "$<TARGET_FILE_DIR:assimp-consumer>"
   DEPENDS buildengine-stage-assimp-consumer
   COMMENT "Stage the actual selected Assimp DLL"
   VERBATIM)

# Audit PE imports before attempting to execute the smoke program.
add_executable(assimp-import-audit PeImportAudit.cpp)
target_compile_features(assimp-import-audit PRIVATE cxx_std_23)
add_custom_target(assimp-verify-imports ALL
   COMMAND "$<TARGET_FILE:assimp-import-audit>"
      "$<TARGET_FILE_DIR:assimp-consumer>" "${CMAKE_BUILD_TYPE}"
      "$<TARGET_FILE_NAME:assimp::assimp>"
   DEPENDS assimp-import-audit assimp-stage-imported-runtime
   COMMENT "Verify Assimp/zlib PE imports against the selected configuration"
   VERBATIM)
