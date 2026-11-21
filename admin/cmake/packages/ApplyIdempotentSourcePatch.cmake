# Apply a version-pinned source patch exactly once.
# In a reused workspace, reverse-check recognizes an already applied patch.
cmake_minimum_required(VERSION 3.31)
foreach(strRequired IN ITEMS GIT_EXECUTABLE SOURCE_ROOT PATCH_FILE)
   if(NOT DEFINED ${strRequired} OR "${${strRequired}}" STREQUAL "")
      message(FATAL_ERROR "Missing ${strRequired} for idempotent patch")
   endif()
endforeach()
if(NOT EXISTS "${PATCH_FILE}" OR NOT IS_DIRECTORY "${SOURCE_ROOT}")
   message(FATAL_ERROR "Patch file or source directory does not exist: ${PATCH_FILE}; ${SOURCE_ROOT}")
endif()

execute_process(
   COMMAND "${GIT_EXECUTABLE}" apply --check --no-index --whitespace=nowarn "${PATCH_FILE}"
   WORKING_DIRECTORY "${SOURCE_ROOT}"
   RESULT_VARIABLE iCheck
   OUTPUT_VARIABLE strCheckOutput
   ERROR_VARIABLE strCheckError)
if(iCheck EQUAL 0)
   execute_process(
      COMMAND "${GIT_EXECUTABLE}" apply --no-index --whitespace=nowarn "${PATCH_FILE}"
      WORKING_DIRECTORY "${SOURCE_ROOT}"
      RESULT_VARIABLE iApply
      OUTPUT_VARIABLE strApplyOutput
      ERROR_VARIABLE strApplyError)
   if(NOT iApply EQUAL 0)
      message(FATAL_ERROR "Source patch apply failed: ${strApplyError}")
   endif()
   message(STATUS "Applied source patch: ${PATCH_FILE}")
   return()
endif()

execute_process(
   COMMAND "${GIT_EXECUTABLE}" apply --reverse --check --no-index --whitespace=nowarn "${PATCH_FILE}"
   WORKING_DIRECTORY "${SOURCE_ROOT}"
   RESULT_VARIABLE iReverse
   OUTPUT_VARIABLE strReverseOutput
   ERROR_VARIABLE strReverseError)
if(iReverse EQUAL 0)
   message(STATUS "Source patch already applied: ${PATCH_FILE}")
   return()
endif()
message(FATAL_ERROR
   "Source does not match either the original or the fully patched tree for ${PATCH_FILE}.\n"
   "Forward check: ${strCheckError}\nReverse check: ${strReverseError}")
