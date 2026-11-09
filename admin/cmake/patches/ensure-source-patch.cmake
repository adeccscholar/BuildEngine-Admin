# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Reconcile a managed source patch with an already completed Source FSM phase.
cmake_minimum_required(VERSION 3.25)

foreach(strParameter IN ITEMS BUILDENGINE_PATCH_WORKSPACE BUILDENGINE_PATCH_FILE BUILDENGINE_GIT_EXECUTABLE)
   if(NOT DEFINED ${strParameter} OR "${${strParameter}}" STREQUAL "")
      message(FATAL_ERROR "Missing required BuildEngine patch parameter: ${strParameter}")
   endif()
endforeach()

if(NOT IS_DIRECTORY "${BUILDENGINE_PATCH_WORKSPACE}")
   message(FATAL_ERROR "Patch workspace does not exist: ${BUILDENGINE_PATCH_WORKSPACE}")
endif()
if(NOT EXISTS "${BUILDENGINE_PATCH_FILE}")
   message(FATAL_ERROR "Patch file does not exist: ${BUILDENGINE_PATCH_FILE}")
endif()
if(NOT EXISTS "${BUILDENGINE_GIT_EXECUTABLE}")
   message(FATAL_ERROR "Git executable does not exist: ${BUILDENGINE_GIT_EXECUTABLE}")
endif()

execute_process(
   COMMAND "${BUILDENGINE_GIT_EXECUTABLE}" apply --reverse --check --no-index --whitespace=nowarn "${BUILDENGINE_PATCH_FILE}"
   WORKING_DIRECTORY "${BUILDENGINE_PATCH_WORKSPACE}"
   RESULT_VARIABLE iAlreadyApplied
   OUTPUT_QUIET ERROR_QUIET
)
if(iAlreadyApplied EQUAL 0)
   message(STATUS "Source patch already applied: ${BUILDENGINE_PATCH_FILE}")
   return()
endif()

execute_process(
   COMMAND "${BUILDENGINE_GIT_EXECUTABLE}" apply --check --no-index --whitespace=nowarn "${BUILDENGINE_PATCH_FILE}"
   WORKING_DIRECTORY "${BUILDENGINE_PATCH_WORKSPACE}"
   RESULT_VARIABLE iCheckResult
   OUTPUT_VARIABLE strCheckStdout
   ERROR_VARIABLE strCheckStderr
)
if(NOT iCheckResult EQUAL 0)
   message(FATAL_ERROR "Source patch cannot be applied: ${BUILDENGINE_PATCH_FILE}\n${strCheckStdout}\n${strCheckStderr}")
endif()

execute_process(
   COMMAND "${BUILDENGINE_GIT_EXECUTABLE}" apply --no-index --whitespace=nowarn "${BUILDENGINE_PATCH_FILE}"
   WORKING_DIRECTORY "${BUILDENGINE_PATCH_WORKSPACE}"
   RESULT_VARIABLE iApplyResult
   OUTPUT_VARIABLE strApplyStdout
   ERROR_VARIABLE strApplyStderr
)
if(NOT iApplyResult EQUAL 0)
   message(FATAL_ERROR "Source patch apply failed: ${BUILDENGINE_PATCH_FILE}\n${strApplyStdout}\n${strApplyStderr}")
endif()

execute_process(
   COMMAND "${BUILDENGINE_GIT_EXECUTABLE}" apply --reverse --check --no-index --whitespace=nowarn "${BUILDENGINE_PATCH_FILE}"
   WORKING_DIRECTORY "${BUILDENGINE_PATCH_WORKSPACE}"
   RESULT_VARIABLE iVerifyResult
   OUTPUT_VARIABLE strVerifyStdout
   ERROR_VARIABLE strVerifyStderr
)
if(NOT iVerifyResult EQUAL 0)
   message(FATAL_ERROR "Source patch verification failed: ${BUILDENGINE_PATCH_FILE}\n${strVerifyStdout}\n${strVerifyStderr}")
endif()
message(STATUS "Source patch applied and verified: ${BUILDENGINE_PATCH_FILE}")
