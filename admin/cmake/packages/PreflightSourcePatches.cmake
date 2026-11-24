# Check every version-bound patch for parseable, complete Git unified-diff
# hunks before the first source patch or a costly build.
cmake_minimum_required(VERSION 3.31)

foreach(strRequired IN ITEMS GIT_EXECUTABLE PATCH_ROOT)
   if(NOT DEFINED ${strRequired} OR "${${strRequired}}" STREQUAL "")
      message(FATAL_ERROR "Missing ${strRequired} for source patch preflight")
   endif()
endforeach()
if(NOT IS_DIRECTORY "${PATCH_ROOT}")
   message(FATAL_ERROR "Patch root does not exist: ${PATCH_ROOT}")
endif()

file(GLOB_RECURSE vecPatches LIST_DIRECTORIES FALSE "${PATCH_ROOT}/*.patch")
if(NOT vecPatches)
   message(FATAL_ERROR "No version-bound .patch files found in ${PATCH_ROOT}")
endif()
list(SORT vecPatches)

# Success details belong exclusively to the file protocol, never stdout.
if(NOT DEFINED REPORT_FILE OR "${REPORT_FILE}" STREQUAL "")
   message(FATAL_ERROR "Missing REPORT_FILE for source patch preflight")
endif()
get_filename_component(strReportDir "${REPORT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${strReportDir}")
file(WRITE "${REPORT_FILE}" "DXC source patch preflight\nPatch root: ${PATCH_ROOT}\n")

foreach(strPatch IN LISTS vecPatches)
   # git apply --numstat parses complete hunks and validates line counts
   # without requiring source files or changing the workspace.
   execute_process(
      COMMAND "${GIT_EXECUTABLE}" apply --numstat --no-index "${strPatch}"
      RESULT_VARIABLE iResult
      OUTPUT_VARIABLE strStat
      ERROR_VARIABLE strError)
   if(NOT iResult EQUAL 0)
      file(APPEND "${REPORT_FILE}" "FAIL: ${strPatch} (git exit ${iResult})\n${strError}\n")
      message(FATAL_ERROR
         "SOURCE PATCH PREFLIGHT FAILED: ${strPatch}\n"
         "git apply --numstat returned ${iResult}\n${strError}")
   endif()
   if("${strStat}" STREQUAL "")
      file(APPEND "${REPORT_FILE}" "FAIL: empty patch ${strPatch}\n")
      message(FATAL_ERROR "Source patch contains no applicable file changes: ${strPatch}")
   endif()
   file(APPEND "${REPORT_FILE}" "PASS: ${strPatch}\n${strStat}")

endforeach()
list(LENGTH vecPatches iTotal)
file(APPEND "${REPORT_FILE}" "PASS: ${iTotal} patches parsed and verified\n")
