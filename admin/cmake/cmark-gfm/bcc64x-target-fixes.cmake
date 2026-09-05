# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# Export/import policy used by the managed adapters:
# - while building a shared library, define the build/export symbol expected by
#   the public export header;
# - while compiling the same sources directly/static, define the header's
#   STATIC_DEFINE symbol so API decoration expands to nothing;
# - normal consumers define neither symbol, so the public header selects import.
#
# cmark-gfm 0.29.0.gfm.13 sets DEFINE_SYMBOL to "cmark-gfm" for the
# extension shared-library target. That is not a valid C identifier and does
# not match the symbol expected by cmark-gfm_export.h. Keep upstream untouched
# and correct only the configured target.
function(AdeccFixCmarkGfmTargetProperties)
   foreach(theTarget IN ITEMS libcmark-gfm-extensions libcmark-gfmextensions)
      if(TARGET ${theTarget})
         get_target_property(theTargetType ${theTarget} TYPE)
         if(theTargetType STREQUAL "SHARED_LIBRARY" OR theTargetType STREQUAL "MODULE_LIBRARY")
            set_property(
               TARGET ${theTarget}
               PROPERTY DEFINE_SYMBOL "libcmark_gfm_EXPORTS")
         elseif(theTargetType STREQUAL "STATIC_LIBRARY" OR theTargetType STREQUAL "OBJECT_LIBRARY")
            target_compile_definitions(${theTarget} PRIVATE CMARK_GFM_STATIC_DEFINE)
         endif()
      endif()
   endforeach()
endfunction()

# CMAKE_PROJECT_INCLUDE loads this file during the upstream project() call.
# Defer until the top-level CMakeLists has created the extension target.
cmake_language(DEFER CALL AdeccFixCmarkGfmTargetProperties)
