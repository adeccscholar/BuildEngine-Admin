# Check the syntax of an entire version-pinned patch set before applying any patch.
# Individual source-context and order checks remain in ApplyIdempotentSourcePatch.cmake.
cmake_minimum_required(VERSION 3.31)

foreach(strRequired IN ITEMS GIT_EXECUTABLE PATCH_DIRECTORY)
   if(NOT DEFINED ${strRequired} OR "${${strRequired}}" STREQUAL "")
      message(FATAL_ERROR "Missing ${strRequired} for source-patch preflight")
   endif()
endforeach()

if(NOT IS_DIRECTORY "${PATCH_DIRECTORY}")
   message(FATAL_ERROR "Source-patch directory does not exist: ${PATCH_DIRECTORY}")
endif()

file(GLOB_RECURSE vecSourcePatches LIST_DIRECTORIES FALSE "${PATCH_DIRECTORY}/*.patch")
list(SORT vecSourcePatches)
if(NOT vecSourcePatches)
   message(FATAL_ERROR "No version-pinned source patches found: ${PATCH_DIRECTORY}")
endif()

set(iChecked 0)
foreach(strPatch IN LISTS vecSourcePatches)
   # --numstat parses the complete unified diff, including every hunk's
   # declared old/new line counts, without requiring a source checkout.
   execute_process(
      COMMAND "${GIT_EXECUTABLE}" apply --numstat --no-index "${strPatch}"
      RESULT_VARIABLE iResult
      OUTPUT_VARIABLE strOutput
      ERROR_VARIABLE strError
   )
   if(NOT iResult EQUAL 0)
      message(FATAL_ERROR
         "Source patch preflight failed before any source mutation:\n"
         "Patch: ${strPatch}\n"
         "git apply --numstat exit: ${iResult}\n"
         "${strError}\n${strOutput}")
   endif()
   math(EXPR iChecked "${iChecked} + 1")
endforeach()

message(STATUS "Validated syntax and hunk counts for ${iChecked} source patches in ${PATCH_DIRECTORY}")
