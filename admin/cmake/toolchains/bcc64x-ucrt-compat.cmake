# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# Prepare a small compatibility archive from the BCC64X runtime installed with
# RAD Studio.  No Embarcadero binary is stored in this repository.  The archive
# is regenerated from the local libucrt.a and linked before the driver's normal
# runtime libraries.

function(AdeccPrepareBcc64xUcrtCompat theOutputVariable)
   if(NOT DEFINED ENV{CB_BDS} OR "$ENV{CB_BDS}" STREQUAL "")
      message(FATAL_ERROR "CB_BDS is not set; BCC64X UCRT compatibility archive cannot be prepared")
   endif()

   get_filename_component(theAdminRoot "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
   set(theHelper "${theAdminRoot}/programs/bcc64x/build-ucrt-compat.ps1")
   if(NOT EXISTS "${theHelper}")
      message(FATAL_ERROR "BCC64X UCRT compatibility helper is missing: ${theHelper}")
   endif()

   if(NOT DEFINED ENV{SystemRoot} OR "$ENV{SystemRoot}" STREQUAL "")
      message(FATAL_ERROR "SystemRoot is not set")
   endif()
   set(thePowerShell "$ENV{SystemRoot}/System32/WindowsPowerShell/v1.0/powershell.exe")
   if(NOT EXISTS "${thePowerShell}")
      message(FATAL_ERROR "Windows PowerShell is missing: ${thePowerShell}")
   endif()

   set(theOutputDirectory "${CMAKE_BINARY_DIR}/.buildengine/bcc64x-ucrt-compat")
   set(theArchive "${theOutputDirectory}/libbcc64x-ucrt-compat.a")
   file(MAKE_DIRECTORY "${theOutputDirectory}")

   execute_process(
      COMMAND
         "${thePowerShell}"
         -NoProfile
         -NonInteractive
         -ExecutionPolicy Bypass
         -File "${theHelper}"
         -Bds "$ENV{CB_BDS}"
         -OutputPath "${theArchive}"
      RESULT_VARIABLE theResult
      OUTPUT_VARIABLE theOutput
      ERROR_VARIABLE theError
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)

   if(NOT theResult EQUAL 0)
      message(FATAL_ERROR
         "Failed to prepare BCC64X UCRT compatibility archive (exit ${theResult})\n"
         "stdout: ${theOutput}\n"
         "stderr: ${theError}")
   endif()
   if(NOT EXISTS "${theArchive}")
      message(FATAL_ERROR "BCC64X UCRT compatibility archive was not created: ${theArchive}")
   endif()

   set(${theOutputVariable} "${theArchive}" PARENT_SCOPE)
endfunction()
