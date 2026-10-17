# SPDX-License-Identifier: MIT
get_filename_component(_PDFIUM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
if(NOT TARGET PDFium::pdfium)
   add_library(PDFium::pdfium STATIC IMPORTED GLOBAL)
   set_target_properties(PDFium::pdfium PROPERTIES
      IMPORTED_CONFIGURATIONS "Release;Debug"
      IMPORTED_LOCATION_RELEASE "${_PDFIUM_ROOT}/lib/win64/Release/pdfium.lib"
      IMPORTED_LOCATION_DEBUG "${_PDFIUM_ROOT}/lib/win64/Debug/pdfium.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${_PDFIUM_ROOT}/include")
   foreach(_cfg IN ITEMS Release Debug)
      if(NOT EXISTS "${_PDFIUM_ROOT}/lib/win64/${_cfg}/pdfium.lib")
         message(FATAL_ERROR "Missing PDFium static archive for ${_cfg}")
      endif()
   endforeach()
   target_link_libraries(PDFium::pdfium INTERFACE advapi32 gdi32 user32)
endif()
