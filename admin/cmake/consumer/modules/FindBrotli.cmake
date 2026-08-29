# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT

include(FindPackageHandleStandardArgs)

find_path(Brotli_INCLUDE_DIR NAMES brotli/decode.h brotli/encode.h)
find_library(Brotli_COMMON_LIBRARY NAMES libbrotlicommon brotlicommon)
find_library(Brotli_DECODER_LIBRARY NAMES libbrotlidec brotlidec)
find_library(Brotli_ENCODER_LIBRARY NAMES libbrotlienc brotlienc)

find_package_handle_standard_args(Brotli
   REQUIRED_VARS
      Brotli_INCLUDE_DIR
      Brotli_COMMON_LIBRARY
      Brotli_DECODER_LIBRARY
      Brotli_ENCODER_LIBRARY)

if(Brotli_FOUND)
   if(NOT TARGET Brotli::common)
      add_library(Brotli::common UNKNOWN IMPORTED)
      set_target_properties(Brotli::common PROPERTIES
         IMPORTED_LOCATION "${Brotli_COMMON_LIBRARY}"
         INTERFACE_INCLUDE_DIRECTORIES "${Brotli_INCLUDE_DIR}")
   endif()

   if(NOT TARGET Brotli::decoder)
      add_library(Brotli::decoder UNKNOWN IMPORTED)
      set_target_properties(Brotli::decoder PROPERTIES
         IMPORTED_LOCATION "${Brotli_DECODER_LIBRARY}"
         INTERFACE_INCLUDE_DIRECTORIES "${Brotli_INCLUDE_DIR}"
         INTERFACE_LINK_LIBRARIES Brotli::common)
   endif()

   if(NOT TARGET Brotli::encoder)
      add_library(Brotli::encoder UNKNOWN IMPORTED)
      set_target_properties(Brotli::encoder PROPERTIES
         IMPORTED_LOCATION "${Brotli_ENCODER_LIBRARY}"
         INTERFACE_INCLUDE_DIRECTORIES "${Brotli_INCLUDE_DIR}"
         INTERFACE_LINK_LIBRARIES Brotli::common)
   endif()
endif()

mark_as_advanced(
   Brotli_INCLUDE_DIR
   Brotli_COMMON_LIBRARY
   Brotli_DECODER_LIBRARY
   Brotli_ENCODER_LIBRARY)
