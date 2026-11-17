# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# BuildEngine BCC64X native SDL3 package
get_filename_component(_SDL3_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
if(NOT TARGET SDL3::SDL3)
   add_library(SDL3::SDL3 SHARED IMPORTED)
   set_target_properties(SDL3::SDL3 PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_LOCATION_RELEASE "${_SDL3_ROOT}/bin/win64/Release/SDL3.dll"
      IMPORTED_IMPLIB_RELEASE "${_SDL3_ROOT}/lib/win64/Release/SDL3.lib"
      IMPORTED_LOCATION_DEBUG "${_SDL3_ROOT}/bin/win64/Debug/SDL3d.dll"
      IMPORTED_IMPLIB_DEBUG "${_SDL3_ROOT}/lib/win64/Debug/SDL3d.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${_SDL3_ROOT}/include")
endif()
set(SDL3_FOUND TRUE)
unset(_SDL3_ROOT)
