# Version-bound DXC v1.9.2609 source adaptation for managed Khronos DLL packages.
# Replaces only the SPIR-V dependency block; never falls back to static builds.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT IS_DIRECTORY "${SOURCE_ROOT}")
   message(FATAL_ERROR "Pinned DXC source root required")
endif()
set(strFile "${SOURCE_ROOT}/external/CMakeLists.txt")
if(NOT EXISTS "${strFile}")
   message(FATAL_ERROR "Pinned DXC external/CMakeLists.txt missing")
endif()
file(READ "${strFile}" strSource)
set(strSentinel "BuildEngine: import installed dynamic Khronos packages; no nested builds.")
string(FIND "${strSource}" "${strSentinel}" iExisting)
if(NOT iExisting EQUAL -1)
   if(NOT strSource MATCHES "set\\(SPIRV_DEP_TARGETS SPIRV-Tools SPIRV-Tools-opt\\)")
      message(FATAL_ERROR "DXC shared SPIRV adapter is incomplete")
   endif()
   set(bAlreadyAdapted TRUE)
else()
   set(bAlreadyAdapted FALSE)
endif()
if(NOT bAlreadyAdapted)
set(strStart [=[if (${ENABLE_SPIRV_CODEGEN})]=])
string(FIND "${strSource}" "${strStart}" iBegin)
if(iBegin EQUAL -1)
   message(FATAL_ERROR "DXC source has no SPIRV codegen block")
endif()
string(SUBSTRING "${strSource}" 0 ${iBegin} strPrefix)
string(SUBSTRING "${strSource}" ${iBegin} -1 strTail)
# A tolerated prior-workspace edit may change whitespace or nested source paths,
# but the original source integration contract must still be recognizable.
foreach(strExpected IN ITEMS "DXC_SPIRV_HEADERS_DIR" "DXC_SPIRV_TOOLS_DIR"
      "SPIRV-Tools-static" "SPIRV_DEP_TARGETS")
   string(FIND "${strTail}" "${strExpected}" iExpected)
   if(iExpected EQUAL -1)
      message(FATAL_ERROR "Unexpected DXC SPIRV source contract: missing ${strExpected}")
   endif()
endforeach()
# In pinned DXC v1.9.2609 the SPIR-V dependency block is the last block.
# Never discard unrelated trailing CMake content.
string(REGEX MATCH "endif\\(\\)[ \t\r\n]*$" strEnd "${strTail}")
if(strEnd STREQUAL "")
   message(FATAL_ERROR "Unexpected DXC external/CMakeLists.txt suffix")
endif()
string(FIND "${strTail}" "${strEnd}" iEnd REVERSE)
string(LENGTH "${strEnd}" uEndLength)
string(LENGTH "${strTail}" uTailLength)
math(EXPR uRemainder "${uTailLength}-${iEnd}-${uEndLength}")
if(NOT uRemainder EQUAL 0)
   message(FATAL_ERROR "Unexpected data after DXC SPIRV block")
endif()
set(strReplacement [==[
if (${ENABLE_SPIRV_CODEGEN})
  # BuildEngine: import installed dynamic Khronos packages; no nested builds.
  foreach(_adecc_package_var IN ITEMS
      DXC_SPIRV_HEADERS_PACKAGE_ROOT DXC_SPIRV_TOOLS_PACKAGE_ROOT)
    if(NOT DEFINED ${_adecc_package_var} OR
       NOT IS_DIRECTORY "${${_adecc_package_var}}/include")
      message(FATAL_ERROR "DXC requires installed ${_adecc_package_var}")
    endif()
  endforeach()
  if(NOT CMAKE_BUILD_TYPE MATCHES "^(Release|Debug)$")
    message(FATAL_ERROR "DXC SPIR-V integration requires Release or Debug")
  endif()

  set(_adecc_spirv_root "${DXC_SPIRV_TOOLS_PACKAGE_ROOT}")
  set(_adecc_spirv_library_dir
      "${_adecc_spirv_root}/lib/win64/${CMAKE_BUILD_TYPE}")
  set(_adecc_spirv_bin_dir
      "${_adecc_spirv_root}/bin/win64/${CMAKE_BUILD_TYPE}")

  # Native Win32 Khronos installs configs at <target>/cmake;
  # other configurations use lib/<variant>/cmake/<target>.
  foreach(_adecc_target IN ITEMS SPIRV-Tools SPIRV-Tools-opt)
    set(_adecc_config "")
    foreach(_adecc_candidate IN ITEMS
        "${_adecc_spirv_root}/${_adecc_target}/cmake"
        "${_adecc_spirv_library_dir}/cmake/${_adecc_target}")
      if(EXISTS "${_adecc_candidate}/${_adecc_target}Config.cmake")
        set(_adecc_config "${_adecc_candidate}")
        break()
      endif()
    endforeach()
    if(_adecc_config STREQUAL "")
      message(FATAL_ERROR "Missing managed ${_adecc_target} CMake config")
    endif()
    set("${_adecc_target}_DIR" "${_adecc_config}"
        CACHE PATH "Exact BuildEngine SPIRV package" FORCE)
    find_package(${_adecc_target} CONFIG REQUIRED
      PATHS "${_adecc_config}" NO_DEFAULT_PATH)
    if(NOT TARGET ${_adecc_target})
      message(FATAL_ERROR "Managed ${_adecc_target} did not export a target")
    endif()
    get_target_property(_adecc_type ${_adecc_target} TYPE)
    if(NOT _adecc_type STREQUAL "SHARED_LIBRARY")
      message(FATAL_ERROR "DXC requires dynamic ${_adecc_target}, not ${_adecc_type}")
    endif()
    string(TOUPPER "${CMAKE_BUILD_TYPE}" _adecc_variant)
    get_target_property(_adecc_dll ${_adecc_target}
      "IMPORTED_LOCATION_${_adecc_variant}")
    get_target_property(_adecc_implib ${_adecc_target}
      "IMPORTED_IMPLIB_${_adecc_variant}")
    if(NOT _adecc_dll OR NOT EXISTS "${_adecc_dll}" OR
       NOT _adecc_implib OR NOT EXISTS "${_adecc_implib}")
      message(FATAL_ERROR "Managed ${_adecc_target} shared artifact missing:
  DLL=${_adecc_dll}
  LIB=${_adecc_implib}")
    endif()
    cmake_path(IS_PREFIX _adecc_spirv_bin_dir
      "${_adecc_dll}" NORMALIZE _adecc_dll_valid)
    cmake_path(IS_PREFIX _adecc_spirv_library_dir
      "${_adecc_implib}" NORMALIZE _adecc_implib_valid)
    if(NOT _adecc_dll_valid OR NOT _adecc_implib_valid)
      message(FATAL_ERROR "DXC ${_adecc_target} imported wrong package variant")
    endif()
  endforeach()

  set(SPIRV_HEADER_INCLUDE_DIR
      "${DXC_SPIRV_HEADERS_PACKAGE_ROOT}/include" PARENT_SCOPE)
  set(SPIRV_TOOLS_INCLUDE_DIR
      "${DXC_SPIRV_TOOLS_PACKAGE_ROOT}/include" PARENT_SCOPE)
  set(SPIRV_DEP_TARGETS SPIRV-Tools SPIRV-Tools-opt)
endif()
]==])
file(WRITE "${strFile}" "${strPrefix}${strReplacement}")
endif()
# Ensure the installed headers reach all subsequent DXC targets, not only the
# external subdirectory. Propagating a variable does not add compiler -I paths.
set(strRootFile "${SOURCE_ROOT}/CMakeLists.txt")
if(NOT EXISTS "${strRootFile}")
   message(FATAL_ERROR "Pinned DXC root CMakeLists.txt is missing")
endif()
file(READ "${strRootFile}" strRootSource)
set(strRootMarker "# BuildEngine: propagate installed SPIRV-Tools headers.")
string(FIND "${strRootSource}" "${strRootMarker}" iRootMarker)
if(iRootMarker EQUAL -1)
   set(strAnchor "  add_subdirectory(external) # SPIRV change")
   string(FIND "${strRootSource}" "${strAnchor}" iAnchor)
   if(iAnchor EQUAL -1)
      message(FATAL_ERROR "Pinned DXC root external CMake anchor missing")
   endif()
   set(strReplacementRoot [==[
  add_subdirectory(external) # SPIRV change
  # BuildEngine: propagate installed SPIRV-Tools headers.
  if(ENABLE_SPIRV_CODEGEN)
    if(NOT EXISTS "${SPIRV_TOOLS_INCLUDE_DIR}/spirv-tools/libspirv.h" OR
       NOT EXISTS "${SPIRV_TOOLS_INCLUDE_DIR}/spirv-tools/libspirv.hpp")
      message(FATAL_ERROR "Managed SPIRV-Tools C and C++ headers are missing: ${SPIRV_TOOLS_INCLUDE_DIR}")
    endif()
    include_directories(AFTER "${SPIRV_TOOLS_INCLUDE_DIR}")
  endif()
]==])
   string(REPLACE "${strAnchor}" "${strReplacementRoot}" strRootSource "${strRootSource}")
   file(WRITE "${strRootFile}" "${strRootSource}")
else()
   set(strExpectedInclude [=[include_directories(AFTER "${SPIRV_TOOLS_INCLUDE_DIR}")]=])
   string(FIND "${strRootSource}" "${strExpectedInclude}" iInclude)
   if(iInclude EQUAL -1)
      message(FATAL_ERROR "DXC managed SPIRV include adapter diverged")
   endif()
endif()
message(STATUS "DXC: managed SPIRV shared-package adapter applied")
