# SPDX-License-Identifier: MIT
# Constrain the upstream C++/WinRT test to the managed BCC64X Windows ABI.
# Must run after bcc64x-generate-test-component.patch.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT EXISTS "${SOURCE_ROOT}/test/CMakeLists.txt")
   message(FATAL_ERROR "Pinned C++/WinRT test CMakeLists.txt missing")
endif()
set(strSource "${SOURCE_ROOT}/test/CMakeLists.txt")
file(READ "${strSource}" strText)
set(strOriginal [=[
    # Windows Runtime error reporting is a Windows SDK UM interface.
    if(NOT EXISTS "${ADECC_CPPWINRT_IDL_ROOT}/um/roerrorapi.h")
        message(FATAL_ERROR "Pinned Windows SDK roerrorapi.h missing")
    endif()
    configure_file("${ADECC_CPPWINRT_IDL_ROOT}/um/roerrorapi.h"
        "${_adecc_test_headers}/roerrorapi.h" COPYONLY)
]=])
set(strReplacement [=[
    # Only this upstream test receives BuildEngine's native Windows ABI adapter.
    # The test still calls the real RoSetErrorReportingFlags OS function.
    if(NOT EXISTS "${ADECC_CPPWINRT_ROERRORAPI_HEADER}")
        message(FATAL_ERROR "Missing registered native WinRT ABI header")
    endif()
    configure_file("${ADECC_CPPWINRT_ROERRORAPI_HEADER}"
        "${_adecc_test_headers}/roerrorapi.h" COPYONLY)
]=])
string(FIND "${strText}" "${strReplacement}" iAdapted)
if(iAdapted EQUAL -1)
   string(FIND "${strText}" "${strOriginal}" iOriginal)
   if(iOriginal EQUAL -1)
      message(FATAL_ERROR "Unexpected upstream C++/WinRT test error API contract")
   endif()
   string(REPLACE "${strOriginal}" "${strReplacement}" strText "${strText}")
endif()

# The copied winrt/*.h files have transitive quoted includes such as
# windows.foundation.h -> windowscontracts.h. Supply the complete pinned
# Windows SDK winrt include root only to the isolated upstream test tree.
set(strWinrtAnchor [=[    include_directories(BEFORE "${_adecc_test_headers}")]=])
set(strWinrtAddition [=[
    if(NOT EXISTS "${ADECC_CPPWINRT_WINRT_SDK_INCLUDE}/windowscontracts.h")
        message(FATAL_ERROR "Registered WinRT SDK windowscontracts.h missing")
    endif()
    include_directories(AFTER "${ADECC_CPPWINRT_WINRT_SDK_INCLUDE}")
]=])
string(FIND "${strText}" "${strWinrtAddition}" iWinrtAdded)
if(iWinrtAdded EQUAL -1)
   string(FIND "${strText}" "${strWinrtAnchor}" iWinrtAnchor)
   if(iWinrtAnchor EQUAL -1)
      message(FATAL_ERROR "Upstream C++/WinRT isolated include anchor missing")
   endif()
   string(REPLACE "${strWinrtAnchor}" "${strWinrtAnchor}\n${strWinrtAddition}" strText "${strText}")
endif()
file(WRITE "${strSource}" "${strText}")
