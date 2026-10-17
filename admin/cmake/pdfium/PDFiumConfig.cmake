# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# PDFium 2025.11.19: native BCC64X shared DLL and COFF import library.
get_filename_component(_PDFIUM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

if(NOT TARGET PDFium::pdfium)
   add_library(PDFium::pdfium SHARED IMPORTED GLOBAL)
   set_target_properties(PDFium::pdfium PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_LOCATION_RELEASE "${_PDFIUM_ROOT}/bin/win64/Release/pdfium.dll"
      IMPORTED_IMPLIB_RELEASE "${_PDFIUM_ROOT}/lib/win64/Release/pdfium.lib"
      IMPORTED_LOCATION_DEBUG "${_PDFIUM_ROOT}/bin/win64/Debug/pdfium.dll"
      IMPORTED_IMPLIB_DEBUG "${_PDFIUM_ROOT}/lib/win64/Debug/pdfium.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${_PDFIUM_ROOT}/include"
      INTERFACE_COMPILE_DEFINITIONS "COMPONENT_BUILD")

   foreach(_theConfiguration IN ITEMS Release Debug)
      foreach(_theArtifact IN ITEMS
            "${_PDFIUM_ROOT}/bin/win64/${_theConfiguration}/pdfium.dll"
            "${_PDFIUM_ROOT}/lib/win64/${_theConfiguration}/pdfium.lib")
         if(NOT EXISTS "${_theArtifact}")
            message(FATAL_ERROR "PDFium shared package artifact is missing: ${_theArtifact}")
         endif()
      endforeach()
   endforeach()
endif()
