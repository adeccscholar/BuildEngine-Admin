# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# zlib 1.3.2 names its independent static library "zs" on Windows.  Defer
# the target adjustment until the upstream top-level CMakeLists.txt has
# declared zlibstatic.  The shared target and its import library are unchanged.

function(_adecc_configure_zlib_artifact_names)
   if(NOT TARGET zlibstatic)
      message(FATAL_ERROR "zlibstatic target is missing")
   endif()

   set_target_properties(zlibstatic PROPERTIES
      OUTPUT_NAME "z-static"
      DEBUG_POSTFIX "-debug")
endfunction()

cmake_language(DEFER CALL _adecc_configure_zlib_artifact_names)
