# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

get_filename_component(_SKIA_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

function(_skia_import_component target_name binary_name)
   if(NOT TARGET Skia::${target_name})
      add_library(Skia::${target_name} SHARED IMPORTED)
      set_target_properties(Skia::${target_name} PROPERTIES
         IMPORTED_CONFIGURATIONS "Release;Debug"
         IMPORTED_LOCATION_RELEASE "${_SKIA_ROOT}/bin/win64/Release/${binary_name}.dll"
         IMPORTED_IMPLIB_RELEASE "${_SKIA_ROOT}/lib/win64/Release/${binary_name}.lib"
         IMPORTED_LOCATION_DEBUG "${_SKIA_ROOT}/bin/win64/Debug/${binary_name}.dll"
         IMPORTED_IMPLIB_DEBUG "${_SKIA_ROOT}/lib/win64/Debug/${binary_name}.lib"
         INTERFACE_INCLUDE_DIRECTORIES "${_SKIA_ROOT}/include"
      )
   endif()
endfunction()

_skia_import_component(skia skia)
set_target_properties(Skia::skia PROPERTIES
   INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SK_GANESH;SK_GL;SK_DIRECT3D;SK_CODEC_DECODES_PNG;SK_CODEC_DECODES_PNG_WITH_LIBPNG;SK_CODEC_ENCODES_PNG;SK_CODEC_ENCODES_PNG_WITH_LIBPNG;SK_CODEC_DECODES_JPEG;SK_CODEC_ENCODES_JPEG;SK_CODEC_DECODES_WEBP;SK_CODEC_ENCODES_WEBP;SK_TYPEFACE_FACTORY_DIRECTWRITE;SK_FONTMGR_DIRECTWRITE_AVAILABLE;SK_FONTMGR_GDI_AVAILABLE"
)

_skia_import_component(skunicode_core skunicode_core)
set_target_properties(Skia::skunicode_core PROPERTIES
   INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SKUNICODE_DLL;SK_UNICODE_AVAILABLE;SK_UNICODE_ICU_IMPLEMENTATION"
   INTERFACE_LINK_LIBRARIES "Skia::skia"
)

_skia_import_component(skunicode_icu skunicode_icu)
set_target_properties(Skia::skunicode_icu PROPERTIES
   INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SKUNICODE_DLL;SK_UNICODE_AVAILABLE;SK_UNICODE_ICU_IMPLEMENTATION"
   INTERFACE_LINK_LIBRARIES "Skia::skunicode_core;Skia::skia"
)

if(NOT TARGET Skia::skunicode)
   add_library(Skia::skunicode INTERFACE IMPORTED)
   set_target_properties(Skia::skunicode PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SKUNICODE_DLL;SK_UNICODE_AVAILABLE;SK_UNICODE_ICU_IMPLEMENTATION"
      INTERFACE_INCLUDE_DIRECTORIES "${_SKIA_ROOT}/include"
      INTERFACE_LINK_LIBRARIES "Skia::skunicode_icu;Skia::skunicode_core;Skia::skia"
   )
endif()

_skia_import_component(skshaper skshaper)
set_target_properties(Skia::skshaper PROPERTIES
   INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SKSHAPER_DLL;SK_SHAPER_PRIMITIVE_AVAILABLE;SK_SHAPER_HARFBUZZ_AVAILABLE;SK_SHAPER_UNICODE_AVAILABLE"
   INTERFACE_LINK_LIBRARIES "Skia::skunicode;Skia::skia"
)

_skia_import_component(svg svg)
set_target_properties(Skia::svg PROPERTIES
   INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SK_ENABLE_SVG"
   INTERFACE_LINK_LIBRARIES "Skia::skshaper;Skia::skunicode;Skia::skia"
)

_skia_import_component(skottie skottie)
set_target_properties(Skia::skottie PROPERTIES
   INTERFACE_COMPILE_DEFINITIONS "SKIA_DLL;SK_ENABLE_SKOTTIE;SK_ENABLE_SKOTTIE_SKSLEFFECT"
   INTERFACE_LINK_LIBRARIES "Skia::skshaper;Skia::skunicode;Skia::skia"
)

unset(_SKIA_ROOT)
