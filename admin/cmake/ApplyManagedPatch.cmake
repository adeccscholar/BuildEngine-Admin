# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Idempotent, verified application of an exact version-pinned upstream patch.
# Never suppress a mismatch: an already-applied patch must pass reverse check,
# while an unpatched source must pass check, apply, then reverse check.

foreach(strRequired IN ITEMS ADECC_PATCH_GIT ADECC_PATCH_WORKSPACE ADECC_PATCH_FILE)
   if(NOT DEFINED ${strRequired} OR "${${strRequired}}" STREQUAL "")
      message(FATAL_ERROR "ApplyManagedPatch: missing ${strRequired}")
   endif()
endforeach()
if(NOT IS_DIRECTORY "${ADECC_PATCH_WORKSPACE}")
   message(FATAL_ERROR "Patch workspace does not exist: ${ADECC_PATCH_WORKSPACE}")
endif()
if(NOT EXISTS "${ADECC_PATCH_FILE}")
   message(FATAL_ERROR "Patch file does not exist: ${ADECC_PATCH_FILE}")
endif()
function(adecc_git_apply_check aReverse aResult aMessage)
   if(aReverse)
      set(vecReverse --reverse)
   else()
      set(vecReverse)
   endif()
   execute_process(
      COMMAND "${ADECC_PATCH_GIT}" apply ${vecReverse} --check --no-index
         --whitespace=nowarn "${ADECC_PATCH_FILE}"
      WORKING_DIRECTORY "${ADECC_PATCH_WORKSPACE}"
      RESULT_VARIABLE iResult
      OUTPUT_VARIABLE strOutput
      ERROR_VARIABLE strError)
   set(${aResult} "${iResult}" PARENT_SCOPE)
   set(${aMessage} "${strOutput}\n${strError}" PARENT_SCOPE)
endfunction()

adecc_git_apply_check(TRUE iApplied strAppliedResult)
if(iApplied EQUAL 0)
   message(STATUS "[PATCH] Already applied and reverse-check verified: ${ADECC_PATCH_FILE}")
   return()
endif()

adecc_git_apply_check(FALSE iCanApply strCheckResult)
if(NOT iCanApply EQUAL 0)
   message(FATAL_ERROR
      "Upstream source does not match either the original or the patched version. "
      "Refusing to overwrite or silently skip patch.\n"
      "Patch: ${ADECC_PATCH_FILE}\n"
      "Forward check: ${strCheckResult}\n"
      "Reverse check: ${strAppliedResult}")
endif()

execute_process(
   COMMAND "${ADECC_PATCH_GIT}" apply --no-index --whitespace=nowarn
      "${ADECC_PATCH_FILE}"
   WORKING_DIRECTORY "${ADECC_PATCH_WORKSPACE}"
   RESULT_VARIABLE iResult
   OUTPUT_VARIABLE strOutput
   ERROR_VARIABLE strError)
if(NOT iResult EQUAL 0)
   message(FATAL_ERROR "Patch application failed: ${ADECC_PATCH_FILE}\n${strOutput}\n${strError}")
endif()
adecc_git_apply_check(TRUE iVerified strVerify)
if(NOT iVerified EQUAL 0)
   message(FATAL_ERROR "Patch applied, but reverse verification failed: ${ADECC_PATCH_FILE}\n${strVerify}")
endif()
message(STATUS "[PATCH] Applied and reverse-check verified: ${ADECC_PATCH_FILE}")
