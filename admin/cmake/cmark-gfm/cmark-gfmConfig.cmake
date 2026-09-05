# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

get_filename_component(_CMARK_GFM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

if(NOT TARGET cmark-gfm::cmark-gfm)
   add_library(cmark-gfm::cmark-gfm SHARED IMPORTED)
   set_target_properties(cmark-gfm::cmark-gfm PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_IMPLIB_RELEASE "${_CMARK_GFM_ROOT}/lib/win64/Release/libcmark-gfm.lib"
      IMPORTED_LOCATION_RELEASE "${_CMARK_GFM_ROOT}/bin/win64/Release/libcmark-gfm.dll"
      IMPORTED_IMPLIB_DEBUG "${_CMARK_GFM_ROOT}/lib/win64/Debug/libcmark-gfm.lib"
      IMPORTED_LOCATION_DEBUG "${_CMARK_GFM_ROOT}/bin/win64/Debug/libcmark-gfm.dll"
      INTERFACE_INCLUDE_DIRECTORIES "${_CMARK_GFM_ROOT}/include"
   )
endif()

if(NOT TARGET cmark-gfm::cmark-gfm-extensions)
   add_library(cmark-gfm::cmark-gfm-extensions SHARED IMPORTED)
   set_target_properties(cmark-gfm::cmark-gfm-extensions PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_IMPLIB_RELEASE "${_CMARK_GFM_ROOT}/lib/win64/Release/libcmark-gfm-extensions.lib"
      IMPORTED_LOCATION_RELEASE "${_CMARK_GFM_ROOT}/bin/win64/Release/libcmark-gfm-extensions.dll"
      IMPORTED_IMPLIB_DEBUG "${_CMARK_GFM_ROOT}/lib/win64/Debug/libcmark-gfm-extensions.lib"
      IMPORTED_LOCATION_DEBUG "${_CMARK_GFM_ROOT}/bin/win64/Debug/libcmark-gfm-extensions.dll"
      INTERFACE_INCLUDE_DIRECTORIES "${_CMARK_GFM_ROOT}/include"
      INTERFACE_LINK_LIBRARIES "cmark-gfm::cmark-gfm"
   )
endif()

unset(_CMARK_GFM_ROOT)
