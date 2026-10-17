# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# PDFium 2025.11.19: explicit native BCC64X static archive consumer contract.
get_filename_component(_PDFIUM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
get_filename_component(_PDFIUM_PACKAGES_ROOT "${_PDFIUM_ROOT}/.." ABSOLUTE)
if(NOT TARGET PDFium::pdfium)
   add_library(PDFium::pdfium STATIC IMPORTED GLOBAL)
   set_target_properties(PDFium::pdfium PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_LOCATION_RELEASE "${_PDFIUM_ROOT}/lib/win64/Release/pdfium.lib"
      IMPORTED_LOCATION_DEBUG "${_PDFIUM_ROOT}/lib/win64/Debug/pdfium.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${_PDFIUM_ROOT}/include")
   foreach(_theConfiguration IN ITEMS Release Debug)
      if(NOT EXISTS "${_PDFIUM_ROOT}/lib/win64/${_theConfiguration}/pdfium.lib")
         message(FATAL_ERROR "PDFium archive not installed for ${_theConfiguration}")
      endif()
      if(_theConfiguration STREQUAL "Debug")
         set(_strIcu "libicuucd")
         set(_strJpeg "libjpegd")
         set(_strZlib "libzd")
         set(_strFreeType "libfreetyped")
         set(_strPng "libpng16d")
      else()
         set(_strIcu "libicuuc")
         set(_strJpeg "libjpeg")
         set(_strZlib "libz")
         set(_strFreeType "libfreetype")
         set(_strPng "libpng16")
      endif()
      # These five system imports exactly match the PDFium GN environment.
      set(_vecPdfiumImports
         "${_PDFIUM_PACKAGES_ROOT}/icu/78.3/lib/win64/${_theConfiguration}/${_strIcu}.lib"
         "${_PDFIUM_PACKAGES_ROOT}/freetype/2.14.3/lib/win64/${_theConfiguration}/${_strFreeType}.lib"
         "${_PDFIUM_PACKAGES_ROOT}/libpng/1.6.58/lib/win64/${_theConfiguration}/${_strPng}.lib"
         "${_PDFIUM_PACKAGES_ROOT}/libjpeg-turbo/3.2.0/lib/win64/${_theConfiguration}/${_strJpeg}.lib"
         "${_PDFIUM_PACKAGES_ROOT}/zlib/1.3.2/lib/win64/${_theConfiguration}/${_strZlib}.lib")
      foreach(_theImport IN LISTS _vecPdfiumImports)
         if(NOT EXISTS "${_theImport}")
            message(FATAL_ERROR "Missing pinned PDFium consumer dependency: ${_theImport}")
         endif()
         target_link_libraries(PDFium::pdfium INTERFACE
            "$<$<CONFIG:${_theConfiguration}>:${_theImport}>")
      endforeach()
   endforeach()
   target_link_libraries(PDFium::pdfium INTERFACE advapi32 gdi32 user32)
endif()
