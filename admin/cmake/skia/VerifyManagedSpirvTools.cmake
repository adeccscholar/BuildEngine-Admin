# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
#
# Skia 153 must link the installed BCC64X SPIRV-Tools DLL import library.
# Validate the exact managed artifact before GN/Ninja, not after a long link.

if(NOT DEFINED ENV{CB_SKIA_SPIRV_TOOLS_LIB} OR "$ENV{CB_SKIA_SPIRV_TOOLS_LIB}" STREQUAL "")
   message(FATAL_ERROR
      "CB_SKIA_SPIRV_TOOLS_LIB was not provided by the BuildEngine dependency contract")
endif()

file(TO_CMAKE_PATH "$ENV{CB_SKIA_SPIRV_TOOLS_LIB}" _spv_import)
if(NOT EXISTS "${_spv_import}" OR IS_DIRECTORY "${_spv_import}")
   get_filename_component(_spv_lib_dir "${_spv_import}" DIRECTORY)
   file(GLOB _spv_existing LIST_DIRECTORIES FALSE "${_spv_lib_dir}/*.lib")
   list(SORT _spv_existing)
   if(_spv_existing)
      string(JOIN "; " _spv_available ${_spv_existing})
   else()
      set(_spv_available "(none)")
   endif()
   message(FATAL_ERROR
      "Missing installed SPIRV-Tools DLL import library: ${_spv_import}\n"
      "Import libraries present in ${_spv_lib_dir}: ${_spv_available}\n"
      "Fix the producer package or its declared import-library name; "
      "do not fall back to a bundled/static SPIRV-Tools copy.")
endif()

message(STATUS "Verified managed SPIRV-Tools import library: ${_spv_import}")
