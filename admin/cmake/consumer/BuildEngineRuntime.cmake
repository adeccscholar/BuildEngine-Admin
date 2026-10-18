# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Application deployment for exact installed BCC64X package versions.
include_guard(GLOBAL)
include(CMakeParseArguments)

function(adecc_stage_runtime a_strTarget)
   if(NOT TARGET "${a_strTarget}")
      message(FATAL_ERROR "Unknown application target: ${a_strTarget}")
   endif()
   cmake_parse_arguments(a_Arg "" "" "PACKAGE_ROOTS" ${ARGN})
   if(a_Arg_UNPARSED_ARGUMENTS)
      message(FATAL_ERROR "Unexpected runtime arguments: ${a_Arg_UNPARSED_ARGUMENTS}")
   endif()
   if(a_Arg_PACKAGE_ROOTS)
      set(vecRoots ${a_Arg_PACKAGE_ROOTS})
   else()
      set(vecRoots ${CMAKE_PREFIX_PATH})
      if(DEFINED ADECC_BUILDENGINE_SDK_ROOT)
         list(PREPEND vecRoots "${ADECC_BUILDENGINE_SDK_ROOT}")
      endif()
   endif()
   list(REMOVE_DUPLICATES vecRoots)
   if(NOT vecRoots)
      message(FATAL_ERROR "Runtime staging requires PACKAGE_ROOTS or CMAKE_PREFIX_PATH")
   endif()
   foreach(theRoot IN LISTS vecRoots)
      if(NOT IS_DIRECTORY "${theRoot}")
         message(FATAL_ERROR "Package root is missing: ${theRoot}")
      endif()
      if("${theRoot}" MATCHES "[|]")
         message(FATAL_ERROR "Invalid pipe character in package root: ${theRoot}")
      endif()
   endforeach()
   # A command-line list separator ('|' or ';') passes through cmd.exe,
   # where '|' is a pipeline operator. Generate a manifest at configure time
   # and pass only its single filename to the CMake -P staging subprocess.
   # list(JOIN) keeps spaces inside individual paths unchanged.
   list(JOIN vecRoots "\n" strRoots)
   set(theRootsFile "${CMAKE_CURRENT_BINARY_DIR}/${a_strTarget}.buildengine-runtime-roots.txt")
   file(WRITE "${theRootsFile}" "${strRoots}\n")
   add_custom_command(TARGET "${a_strTarget}" POST_BUILD
      COMMAND "${CMAKE_COMMAND}"
         "-DADECC_RUNTIME_ROOTS_FILE:FILEPATH=${theRootsFile}"
         "-DADECC_RUNTIME_CONFIGURATION=$<CONFIG>"
         "-DADECC_RUNTIME_OUTPUT_DIR=$<TARGET_FILE_DIR:${a_strTarget}>"
         -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/BuildEngineStageRuntime.cmake"
      COMMENT "Stage BuildEngine runtime DLLs for ${a_strTarget}"
      VERBATIM)
endfunction()

function(adecc_install_application a_strTarget)
   if(NOT TARGET "${a_strTarget}")
      message(FATAL_ERROR "Unknown application target: ${a_strTarget}")
   endif()
   cmake_parse_arguments(a_Arg "" "DESTINATION" "" ${ARGN})
   if(a_Arg_UNPARSED_ARGUMENTS OR a_Arg_KEYWORDS_MISSING_VALUES)
      message(FATAL_ERROR "Invalid application installation arguments")
   endif()
   if(NOT a_Arg_DESTINATION)
      set(a_Arg_DESTINATION bin)
   endif()
   install(TARGETS "${a_strTarget}" RUNTIME DESTINATION "${a_Arg_DESTINATION}")
   install(DIRECTORY "$<TARGET_FILE_DIR:${a_strTarget}>/"
      DESTINATION "${a_Arg_DESTINATION}"
      FILES_MATCHING PATTERN "*.dll" PATTERN "*.DLL")
endfunction()
