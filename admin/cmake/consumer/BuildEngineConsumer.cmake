# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
#
# Shared consumer integration for the BuildEngine Win64x publish tree.
# This file is copied to <InstallRoot>/Win64x/cmake by the publish phase.

if(DEFINED ADECC_BUILDENGINE_CONSUMER_INCLUDED)
   return()
endif()
set(ADECC_BUILDENGINE_CONSUMER_INCLUDED TRUE)

get_filename_component(ADECC_BUILDENGINE_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

list(PREPEND CMAKE_PREFIX_PATH "${ADECC_BUILDENGINE_SDK_ROOT}")
list(PREPEND CMAKE_LIBRARY_PATH "${ADECC_BUILDENGINE_SDK_ROOT}/lib")
list(PREPEND CMAKE_INCLUDE_PATH "${ADECC_BUILDENGINE_SDK_ROOT}/include")
list(PREPEND CMAKE_PROGRAM_PATH "${ADECC_BUILDENGINE_SDK_ROOT}/bin")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")

# BCC64X packages in this SDK intentionally use the lib*.lib naming convention.
# Keep the native empty Windows prefix as a fallback for system/import libraries.
list(PREPEND CMAKE_FIND_LIBRARY_PREFIXES "lib")
if(WIN32)
   list(APPEND CMAKE_FIND_LIBRARY_PREFIXES "")
endif()

list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
list(REMOVE_DUPLICATES CMAKE_LIBRARY_PATH)
list(REMOVE_DUPLICATES CMAKE_INCLUDE_PATH)
list(REMOVE_DUPLICATES CMAKE_PROGRAM_PATH)
list(REMOVE_DUPLICATES CMAKE_MODULE_PATH)

set(ADECC_BUILDENGINE_RUNTIME_PATH "${ADECC_BUILDENGINE_SDK_ROOT}/bin")

# Boost 1.92.0 uses a capability-based BCC64X/Clang Boost.Config policy.  The
# consumer hook activates itself only when Boost has actually been published.
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/boost/BuildEngineBoostConsumer.cmake")
   include("${CMAKE_CURRENT_LIST_DIR}/boost/BuildEngineBoostConsumer.cmake")
endif()
