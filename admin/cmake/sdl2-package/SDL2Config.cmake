# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# Portable SDL2 package definition for the BuildEngine third-party prefix.
# Both BCC64X configurations use canonical import-library names generated
# from the installed DLLs.
get_filename_component(_SDL2_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

if(NOT TARGET SDL2::SDL2)
   add_library(SDL2::SDL2 SHARED IMPORTED)
   set_target_properties(SDL2::SDL2 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${_SDL2_PREFIX}/include/SDL2"
      IMPORTED_CONFIGURATIONS "Debug;Release"
      IMPORTED_LOCATION_DEBUG "${_SDL2_PREFIX}/bin/win64/Debug/SDL2d.dll"
      IMPORTED_IMPLIB_DEBUG "${_SDL2_PREFIX}/lib/win64/Debug/SDL2d.lib"
      IMPORTED_LOCATION_RELEASE "${_SDL2_PREFIX}/bin/win64/Release/SDL2.dll"
      IMPORTED_IMPLIB_RELEASE "${_SDL2_PREFIX}/lib/win64/Release/SDL2.lib")
endif()

set(SDL2_FOUND TRUE)
unset(_SDL2_PREFIX)
