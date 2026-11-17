# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# BuildEngine BCC64X native Box2D DLL package
get_filename_component(_BOX2D_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
if(NOT TARGET box2d::box2d)
   add_library(box2d::box2d SHARED IMPORTED)
   set_target_properties(box2d::box2d PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_LOCATION_RELEASE "${_BOX2D_ROOT}/bin/win64/Release/box2d.dll"
      IMPORTED_IMPLIB_RELEASE "${_BOX2D_ROOT}/lib/win64/Release/box2d.lib"
      IMPORTED_LOCATION_DEBUG "${_BOX2D_ROOT}/bin/win64/Debug/box2dd.dll"
      IMPORTED_IMPLIB_DEBUG "${_BOX2D_ROOT}/lib/win64/Debug/box2dd.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${_BOX2D_ROOT}/include"
      INTERFACE_COMPILE_DEFINITIONS "BOX2D_DLL")
endif()
set(box2d_FOUND TRUE)
unset(_BOX2D_ROOT)
