# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# RAD Studio 13 Florence / BCC64X / Win64 Modern.
# BuildEngine BCC64X C/C++ profile derived from the proven project toolchain.
# It contains only generic compiler/linker integration. It does not add
# library-specific workarounds. ASM uses the same BCC64X driver, matching the
# proven Boost.Context/Coroutine/Fiber toolchain path.

if(NOT DEFINED ENV{CB_BCC64X} OR "$ENV{CB_BCC64X}" STREQUAL "")
   message(FATAL_ERROR "CB_BCC64X is not set")
endif()
if(NOT DEFINED ENV{CB_BDS} OR "$ENV{CB_BDS}" STREQUAL "")
   message(FATAL_ERROR "CB_BDS is not set")
endif()

get_filename_component(_TP_CMAKE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(_BUILDENGINE_PRODUCTION_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(_BCC64X_RULE_OVERRIDE "${_TP_CMAKE_ROOT}/overrides/bcc64x-rules.cmake")

if(NOT EXISTS "${_BCC64X_RULE_OVERRIDE}")
   message(FATAL_ERROR "BCC64X rule override is missing: ${_BCC64X_RULE_OVERRIDE}")
endif()

# The compatibility archive is provisioned once by BuildEngine's build-tools
# contract. Select the newest versioned generated-tool directory and only
# consume the resulting archive here; CMake does not generate it.
file(GLOB _BCC64X_UCRT_COMPAT_CANDIDATES
   "${_BUILDENGINE_PRODUCTION_ROOT}/tools/bcc64x-ucrt-compat/*/libbcc64x-ucrt-compat.a")
if(NOT _BCC64X_UCRT_COMPAT_CANDIDATES)
   message(FATAL_ERROR
      "BCC64X UCRT compatibility tool is missing below ${_BUILDENGINE_PRODUCTION_ROOT}/tools/bcc64x-ucrt-compat")
endif()
list(SORT _BCC64X_UCRT_COMPAT_CANDIDATES COMPARE NATURAL ORDER DESCENDING)
list(GET _BCC64X_UCRT_COMPAT_CANDIDATES 0 BCC64X_UCRT_COMPAT_LIBRARY)
set(BCC64X_UCRT_COMPAT_LIBRARY
   "${BCC64X_UCRT_COMPAT_LIBRARY}"
   CACHE INTERNAL "BCC64X Win64 UCRT compatibility archive" FORCE)
unset(_BCC64X_UCRT_COMPAT_CANDIDATES)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CROSSCOMPILING OFF)

set(_BCC64X_RAD_LIBRARY_PATHS
   "$ENV{CB_BDS}/lib/win64x/release"
   "$ENV{CB_BDS}/lib/win64x/release/psdk")
set(_BCC64X_RAD_INCLUDE_PATHS
   "$ENV{CB_BDS}/include/windows/sdk"
   "$ENV{CB_BDS}/include/windows/rtl")
list(REMOVE_ITEM CMAKE_LIBRARY_PATH ${_BCC64X_RAD_LIBRARY_PATHS})
list(REMOVE_ITEM CMAKE_INCLUDE_PATH ${_BCC64X_RAD_INCLUDE_PATHS})
list(PREPEND CMAKE_LIBRARY_PATH ${_BCC64X_RAD_LIBRARY_PATHS})
list(PREPEND CMAKE_INCLUDE_PATH ${_BCC64X_RAD_INCLUDE_PATHS})
unset(_BCC64X_RAD_LIBRARY_PATHS)
unset(_BCC64X_RAD_INCLUDE_PATHS)

# BCC64X ships its own windres implementation.  That is the default for
# CMake projects that do not explicitly request the Windows SDK resource
# compiler.  CB_WINDOWS_RC selects the SDK implementation for the few upstream
# projects that actually require Microsoft's RC and SDK include conventions.
#
# Never FORCE a new compiler into an existing CMake cache.  A change of RC
# language identity needs an explicit, per-library --fresh migration.
set(_CB_RC_KIND "bcc64x-windres")
set(_CB_RC_EXECUTABLE "$ENV{CB_BDS}/bin64/bcc64x-windres.exe")
if(DEFINED ENV{CB_WINDOWS_RC} AND NOT "$ENV{CB_WINDOWS_RC}" STREQUAL "")
   set(_CB_RC_KIND "windows-sdk-rc")
   set(_CB_RC_EXECUTABLE "$ENV{CB_WINDOWS_RC}")
endif()
if(NOT EXISTS "${_CB_RC_EXECUTABLE}")
   message(FATAL_ERROR
      "BuildEngine resource compiler not found (${_CB_RC_KIND}): ${_CB_RC_EXECUTABLE}")
endif()
if(DEFINED CACHE{CMAKE_RC_COMPILER} AND NOT "${CMAKE_RC_COMPILER}" STREQUAL "")
   if(NOT "${CMAKE_RC_COMPILER}" STREQUAL "${_CB_RC_EXECUTABLE}")
      message(STATUS
         "BuildEngine existing RC cache retained: ${CMAKE_RC_COMPILER}; "
         "contract preference: ${_CB_RC_EXECUTABLE}. "
         "Use per-library --fresh only when migrating this build tree.")
   endif()
else()
   set(CMAKE_RC_COMPILER "${_CB_RC_EXECUTABLE}"
      CACHE FILEPATH "BuildEngine resource compiler" )
endif()
message(STATUS "BuildEngine active resource compiler: ${CMAKE_RC_COMPILER}")

# SDK header paths and Microsoft /I flags must never reach BCC64X windres.
# They are only valid when the actual selected compiler is the SDK rc.exe.
if(_CB_RC_KIND STREQUAL "windows-sdk-rc" AND
   "${CMAKE_RC_COMPILER}" STREQUAL "${_CB_RC_EXECUTABLE}")
   get_filename_component(_CB_RC_BIN_DIR "${_CB_RC_EXECUTABLE}" DIRECTORY)
   get_filename_component(_CB_RC_VERSION_DIR "${_CB_RC_BIN_DIR}" DIRECTORY)
   get_filename_component(_CB_RC_VERSION "${_CB_RC_VERSION_DIR}" NAME)
   get_filename_component(_CB_RC_BIN_ROOT "${_CB_RC_VERSION_DIR}" DIRECTORY)
   get_filename_component(_CB_RC_SDK_ROOT "${_CB_RC_BIN_ROOT}" DIRECTORY)
   set(_CB_RC_INCLUDE_ROOT "${_CB_RC_SDK_ROOT}/Include/${_CB_RC_VERSION}")
   foreach(_CB_RC_INCLUDE um shared ucrt)
      if(NOT IS_DIRECTORY "${_CB_RC_INCLUDE_ROOT}/${_CB_RC_INCLUDE}")
         message(FATAL_ERROR
            "Windows SDK RC include directory missing: ${_CB_RC_INCLUDE_ROOT}/${_CB_RC_INCLUDE}")
      endif()
   endforeach()
   set(CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES
      "${_CB_RC_INCLUDE_ROOT}/um"
      "${_CB_RC_INCLUDE_ROOT}/shared"
      "${_CB_RC_INCLUDE_ROOT}/ucrt"
      CACHE STRING "Registered Windows SDK RC standard includes" FORCE)
   set(CMAKE_RC_FLAGS_INIT "/I\\\"${_CB_RC_INCLUDE_ROOT}/um\\\" /I\\\"${_CB_RC_INCLUDE_ROOT}/shared\\\"")
   set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS_INIT}"
      CACHE STRING "Windows SDK RC include arguments" FORCE)
endif()
unset(_CB_RC_KIND)
unset(_CB_RC_EXECUTABLE)
unset(_CB_RC_BIN_DIR)
unset(_CB_RC_VERSION_DIR)
unset(_CB_RC_VERSION)
unset(_CB_RC_BIN_ROOT)
unset(_CB_RC_SDK_ROOT)
unset(_CB_RC_INCLUDE_ROOT)
unset(_CB_RC_INCLUDE)

set(CMAKE_ASM_COMPILER "$ENV{CB_BCC64X}" CACHE FILEPATH "BCC64X assembler driver" FORCE)
set(CMAKE_C_COMPILER   "$ENV{CB_BCC64X}" CACHE FILEPATH "BCC64X C compiler" FORCE)
set(CMAKE_CXX_COMPILER "$ENV{CB_BCC64X}" CACHE FILEPATH "BCC64X C++ compiler" FORCE)

# CMake 4.x automatically scans C++23 translation units for C++ modules.
# For BuildEngine's non-module BCC64X consumers this invokes clang-scan-deps,
# which fails in the compatibility headers before the actual compiler pass.
# Keep BCC64X as Clang/EMBT and use its regular compilation path.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF CACHE BOOL
   "Disable automatic C++ module scanning for BCC64X consumers" FORCE)

set(CMAKE_USER_MAKE_RULES_OVERRIDE
   "${_BCC64X_RULE_OVERRIDE}"
   CACHE FILEPATH "BCC64X CMake rule override" FORCE)

# Configuration-specific optimization and preprocessor flags are supplied by
# the BuildEngine build-variant contract. The toolchain does not override them.

set(CMAKE_TRY_COMPILE_CONFIGURATION Release CACHE STRING "" FORCE)

get_property(_BCC64X_TOOLCHAIN_SUMMARY_EMITTED GLOBAL PROPERTY ADECC_BCC64X_TOOLCHAIN_SUMMARY_EMITTED)
if(NOT _BCC64X_TOOLCHAIN_SUMMARY_EMITTED)
   message(STATUS "BCC64X C/C++/ASM toolchain: $ENV{CB_BCC64X}")
   message(STATUS "BCC64X rules override: ${_BCC64X_RULE_OVERRIDE}")
   message(STATUS "BCC64X UCRT compatibility archive: ${BCC64X_UCRT_COMPAT_LIBRARY}")
   set_property(GLOBAL PROPERTY ADECC_BCC64X_TOOLCHAIN_SUMMARY_EMITTED TRUE)
endif()
unset(_BCC64X_TOOLCHAIN_SUMMARY_EMITTED)
unset(_BUILDENGINE_PRODUCTION_ROOT)
