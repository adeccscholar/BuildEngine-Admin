# SPDX-License-Identifier: MIT
# Version-bound DirectX-Headers 1.619.1 native Windows DLL conversion.
# Only the isolated extracted source workspace is modified.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT EXISTS "${SOURCE_ROOT}/CMakeLists.txt")
   message(FATAL_ERROR "SOURCE_ROOT must contain DirectX-Headers 1.619.1")
endif()
set(_adecc_file "${SOURCE_ROOT}/CMakeLists.txt")
file(READ "${_adecc_file}" _adecc_source)
string(REPLACE "\r\n" "\n" _adecc_source "${_adecc_source}")
function(adecc_replace_required strOld strNew)
   string(FIND "${_adecc_source}" "${strNew}" _already)
   if(NOT _already EQUAL -1)
      return()
   endif()
   string(FIND "${_adecc_source}" "${strOld}" _found)
   if(_found EQUAL -1)
      message(FATAL_ERROR "Unexpected DirectX-Headers 1.619.1 CMake source: ${strOld}")
   endif()
   string(REPLACE "${strOld}" "${strNew}" _updated "${_adecc_source}")
   set(_adecc_source "${_updated}" PARENT_SCOPE)
endfunction()
adecc_replace_required(
   "add_library(DirectX-Headers STATIC src/d3dx12_property_format_table.cpp)"
   "add_library(DirectX-Headers SHARED src/d3dx12_property_format_table.cpp)")
adecc_replace_required(
   "add_library(DirectX-Guids STATIC src/dxguids.cpp)"
   "add_library(DirectX-Guids SHARED src/dxguids.cpp)")
set(_adecc_shared_properties [=[
# BuildEngine: DLL runtime and matching COFF import library, never static archives.
foreach(_adecc_dll IN ITEMS DirectX-Headers DirectX-Guids)
   set_target_properties(${_adecc_dll} PROPERTIES
      WINDOWS_EXPORT_ALL_SYMBOLS ON
      PREFIX "" SUFFIX ".dll"
      IMPORT_PREFIX "" IMPORT_SUFFIX ".lib")
endforeach()
]=])
adecc_replace_required(
   "add_library(Microsoft::DirectX-Guids ALIAS DirectX-Guids)"
   "add_library(Microsoft::DirectX-Guids ALIAS DirectX-Guids)\n${_adecc_shared_properties}")
adecc_replace_required(
   [=[            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})]=]
   [=[            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}\n            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})]=])
file(WRITE "${_adecc_file}" "${_adecc_source}")
message(STATUS "DirectX-Headers 1.619.1: native DLL and import-library contract applied")
