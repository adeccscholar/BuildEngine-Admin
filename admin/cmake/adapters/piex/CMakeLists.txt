# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: Apache-2.0
#
# BuildEngine-only CMake adapter for Google's upstream GYP-based PIE-X.
# The upstream sources and source pin remain owned by admin/build-libraries.xml.
cmake_minimum_required(VERSION 3.31)
project(piex LANGUAGES CXX)

include(GNUInstallDirs)

add_library(piex SHARED
   src/piex.cc
   src/piex_cr3.cc
   src/tiff_parser.cc
   src/binary_parse/cached_paged_byte_array.cc
   src/binary_parse/range_checked_byte_ptr.cc
   src/image_type_recognition/image_type_recognition_lite.cc
   src/tiff_directory/tiff_directory.cc
)

target_compile_features(piex PRIVATE cxx_std_17)
target_include_directories(piex
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:include>
)
set_target_properties(piex PROPERTIES
   WINDOWS_EXPORT_ALL_SYMBOLS ON
   OUTPUT_NAME piex
   DEBUG_POSTFIX d
)

install(TARGETS piex
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(DIRECTORY src/
   DESTINATION include/src
   FILES_MATCHING PATTERN "*.h"
)
