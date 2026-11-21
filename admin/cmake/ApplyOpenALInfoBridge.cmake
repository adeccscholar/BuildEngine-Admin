# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Idempotent, source-verified correction of OpenAL Soft 1.25.2 openal-info.
# Retains ALSOFT_UTILS, Unicode wmain, the Windows backends and upstream tests.
if(NOT DEFINED ADECC_OPENAL_SOURCE OR NOT IS_DIRECTORY "${ADECC_OPENAL_SOURCE}")
   message(FATAL_ERROR "ADECC_OPENAL_SOURCE must be the exact extracted OpenAL Soft 1.25.2 root")
endif()
set(theCMakeLists "${ADECC_OPENAL_SOURCE}/CMakeLists.txt")
set(theBridge "${ADECC_OPENAL_SOURCE}/utils/bcc64x-openal-info-main-bridge.c")
if(NOT EXISTS "${theCMakeLists}" OR NOT EXISTS "${theBridge}")
   message(FATAL_ERROR "OpenAL bridge source or CMakeLists.txt is missing")
endif()
file(READ "${theCMakeLists}" strOriginal)
set(strUpdated "${strOriginal}")

set(strSourceOriginal [=[    add_executable(openal-info utils/openal-info.c)]=])
set(strSourcePatched [=[    if(ADECC_BCC64X_WINDOWS10_SDK)
        # Preserve upstream Unicode wmain; bridge the BCC64X console entry point.
        add_executable(openal-info utils/openal-info.c utils/bcc64x-openal-info-main-bridge.c)
    else()
        add_executable(openal-info utils/openal-info.c)
    endif()]=])
string(FIND "${strUpdated}" "utils/bcc64x-openal-info-main-bridge.c)" iSourcePatched)
if(iSourcePatched LESS 0)
   string(FIND "${strUpdated}" "${strSourceOriginal}" iSourceOriginal)
   if(iSourceOriginal LESS 0)
      message(FATAL_ERROR
         "OpenAL Soft 1.25.2 openal-info source registration has changed; "
         "expected exactly '${strSourceOriginal}'. Refusing an unverified edit.")
   endif()
   string(REPLACE "${strSourceOriginal}" "${strSourcePatched}" strUpdated "${strUpdated}")
endif()

set(strLinkOriginal [=[    target_link_libraries(openal-info PRIVATE Threads::Threads ${LINKER_FLAGS} OpenAL
        ${UNICODE_FLAG})]=])
set(strLinkPatched [=[    target_link_libraries(openal-info PRIVATE Threads::Threads ${LINKER_FLAGS} OpenAL
        ${UNICODE_FLAG})
    if(ADECC_BCC64X_WINDOWS10_SDK)
        target_link_libraries(openal-info PRIVATE shell32)
    endif()]=])
string(FIND "${strUpdated}" "target_link_libraries(openal-info PRIVATE shell32)" iLinkPatched)
if(iLinkPatched LESS 0)
   string(FIND "${strUpdated}" "${strLinkOriginal}" iLinkOriginal)
   if(iLinkOriginal LESS 0)
      message(FATAL_ERROR
         "OpenAL Soft 1.25.2 openal-info linker registration has changed. "
         "Cannot validate the exact Threads/OpenAL/Unicode block; refusing edit.")
   endif()
   string(REPLACE "${strLinkOriginal}" "${strLinkPatched}" strUpdated "${strUpdated}")
endif()

string(FIND "${strUpdated}" "utils/bcc64x-openal-info-main-bridge.c)" iSourceVerified)
string(FIND "${strUpdated}" "target_link_libraries(openal-info PRIVATE shell32)" iLinkVerified)
if(iSourceVerified LESS 0 OR iLinkVerified LESS 0)
   message(FATAL_ERROR "OpenAL Soft 1.25.2 bridge postconditions not satisfied")
endif()
if(NOT "${strOriginal}" STREQUAL "${strUpdated}")
   file(WRITE "${theCMakeLists}" "${strUpdated}")
   message(STATUS "[PATCH] OpenAL Soft 1.25.2 Unicode bridge applied and verified")
else()
   message(STATUS "[PATCH] OpenAL Soft 1.25.2 Unicode bridge already verified")
endif()
