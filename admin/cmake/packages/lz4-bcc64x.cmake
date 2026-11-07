# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: LicenseRef-PolyForm-Noncommercial-1.0.0
# Keep the upstream LZ4 CMake tree unchanged on BCC64X.
function(_AdeccConfigureLz4)
   if(NOT TARGET lz4_shared)
      message(FATAL_ERROR "LZ4 upstream target lz4_shared is missing")
   endif()
   if(TARGET lz4_static)
      message(FATAL_ERROR "Unexpected static LZ4 production target")
   endif()
   if(NOT WIN32)
      message(FATAL_ERROR "LZ4 BCC64X adapter requires Windows")
   endif()
   target_compile_definitions(lz4_shared PRIVATE LZ4_DLL_EXPORT=1)
   target_compile_definitions(lz4_shared INTERFACE LZ4_DLL_IMPORT=1)
   set_target_properties(lz4_shared PROPERTIES
      PREFIX "lib"
      OUTPUT_NAME "lz4"
      DEBUG_POSTFIX "d")
endfunction()
cmake_language(DEFER CALL _AdeccConfigureLz4)
