# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

get_filename_component(_SKIA_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

if(NOT TARGET Skia::skia)
   add_library(Skia::skia SHARED IMPORTED)
   set_target_properties(Skia::skia PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_LOCATION_RELEASE "${_SKIA_ROOT}/bin/win64/Release/skia.dll"
      IMPORTED_IMPLIB_RELEASE "${_SKIA_ROOT}/lib/win64/Release/skia.lib"
      IMPORTED_LOCATION_DEBUG "${_SKIA_ROOT}/bin/win64/Debug/skia.dll"
      IMPORTED_IMPLIB_DEBUG "${_SKIA_ROOT}/lib/win64/Debug/skia.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${_SKIA_ROOT}/include"
      INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL"
   )
endif()

unset(_SKIA_ROOT)
