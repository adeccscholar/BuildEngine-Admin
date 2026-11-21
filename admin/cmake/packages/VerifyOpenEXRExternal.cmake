# Verify that upstream OpenEXR resolved the independent codec packages.
# This is an evidence gate, not a fallback or a mechanism to disable features.
if(NOT DEFINED OPENEXR_CACHE OR NOT EXISTS "${OPENEXR_CACHE}")
   message(FATAL_ERROR "OpenEXR configuration cache is missing: ${OPENEXR_CACHE}")
endif()

file(STRINGS "${OPENEXR_CACHE}" vecOpenJphExternal
   REGEX "^OPENEXR_USE_INTERNAL_OPENJPH:INTERNAL=0$")
if(NOT vecOpenJphExternal)
   file(STRINGS "${OPENEXR_CACHE}" vecOpenJphEvidence
      REGEX "^(openjph_DIR|openjph_ROOT|OPENEXR_USE_INTERNAL_OPENJPH|OPENEXR_FORCE_INTERNAL_OPENJPH|CMAKE_PREFIX_PATH):")
   list(JOIN vecOpenJphEvidence "\n  " strEvidence)
   message(FATAL_ERROR
      "OpenEXR did not confirm external OpenJPH; bundled fallback is forbidden.\n"
      "CMake cache evidence:\n  ${strEvidence}\n"
      "Check the OpenEXR configure log and the installed OpenJPH package.")
endif()

foreach(strDependency IN ITEMS Imath libdeflate openjph zstd)
   file(STRINGS "${OPENEXR_CACHE}" vecPackageEntries REGEX "^${strDependency}_DIR:PATH=")
   if(NOT vecPackageEntries)
      message(FATAL_ERROR "OpenEXR external CMake package not discovered: ${strDependency}")
   endif()
   list(GET vecPackageEntries 0 strPackageEntry)
   string(REGEX REPLACE "^[^=]*=" "" strPackageDirectory "${strPackageEntry}")
   if(NOT IS_DIRECTORY "${strPackageDirectory}")
      message(FATAL_ERROR
         "OpenEXR external package ${strDependency} resolves to an invalid directory: ${strPackageDirectory}")
   endif()
endforeach()
message(STATUS "OpenEXR external CMake dependencies verified; native DLL linkage still requires consumer smoke")
