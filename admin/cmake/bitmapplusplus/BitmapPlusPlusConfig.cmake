# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

if(TARGET bmp::BitmapPlusPlus)
   return()
endif()

get_filename_component(_BITMAPPLUSPLUS_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(BitmapPlusPlus INTERFACE IMPORTED)
set_target_properties(BitmapPlusPlus PROPERTIES
   INTERFACE_INCLUDE_DIRECTORIES "${_BITMAPPLUSPLUS_PREFIX}/include"
   INTERFACE_COMPILE_FEATURES cxx_std_17
)
add_library(bmp::BitmapPlusPlus ALIAS BitmapPlusPlus)

unset(_BITMAPPLUSPLUS_PREFIX)
