# Reject an OpenEXR build that silently fell back to bundled codecs.
# Run after upstream configure and before the native DLL build.
if(NOT DEFINED OPENEXR_CACHE OR NOT EXISTS "${OPENEXR_CACHE}")
   message(FATAL_ERROR "OpenEXR configuration cache is missing")
endif()
file(READ "${OPENEXR_CACHE}" strCache)
string(FIND "${strCache}" "OPENEXR_USE_INTERNAL_OPENJPH:INTERNAL=0" iExternalOpenJph)
if(iExternalOpenJph LESS 0)
   file(STRINGS "${OPENEXR_CACHE}" vecEvidence
      REGEX "^(openjph_DIR|openjph_ROOT|OPENEXR_USE_INTERNAL_OPENJPH|OPENEXR_FORCE_INTERNAL_OPENJPH|CMAKE_PREFIX_PATH):")
   list(JOIN vecEvidence "\\n  " strEvidence)
   message(FATAL_ERROR
      "OpenEXR did not link external OpenJPH. Bundled fallback is forbidden.\\n"
      "CMake cache evidence:\\n  ${strEvidence}\\n"
      "Inspect the OpenEXR configure log for the failed find_package(openjph) call.")
endif()
foreach(strDependency IN ITEMS Imath libdeflate openjph)
   string(FIND "${strCache}" "${strDependency}_DIR:PATH=" iDependency)
   if(iDependency LESS 0)
      message(FATAL_ERROR "OpenEXR CMake package discovery missing: ${strDependency}")
   endif()
endforeach()
message(STATUS "OpenEXR external package discovery verified; native linkage still requires a smoke test")
