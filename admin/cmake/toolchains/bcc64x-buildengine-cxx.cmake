# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# RAD Studio 13 Florence / BCC64X / Win64 Modern.
# BuildEngine BCC64X C/C++ profile derived from the proven project toolchain.
# It contains only generic compiler/linker integration.  It does not add
# library-specific workarounds.  ASM uses the same BCC64X driver, matching the
# proven Boost.Context/Coroutine/Fiber toolchain path.

if(NOT DEFINED ENV{CB_BCC64X} OR "$ENV{CB_BCC64X}" STREQUAL "")
   message(FATAL_ERROR "CB_BCC64X is not set")
endif()

get_filename_component(_TP_CMAKE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_BCC64X_RULE_OVERRIDE "${_TP_CMAKE_ROOT}/overrides/bcc64x-rules.cmake")

if(NOT EXISTS "${_BCC64X_RULE_OVERRIDE}")
   message(FATAL_ERROR "BCC64X rule override is missing: ${_BCC64X_RULE_OVERRIDE}")
endif()

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CROSSCOMPILING OFF)

if(DEFINED ENV{CB_BDS} AND NOT "$ENV{CB_BDS}" STREQUAL "")
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
endif()

set(CMAKE_ASM_COMPILER "$ENV{CB_BCC64X}" CACHE FILEPATH "BCC64X assembler driver" FORCE)
set(CMAKE_C_COMPILER   "$ENV{CB_BCC64X}" CACHE FILEPATH "BCC64X C compiler" FORCE)
set(CMAKE_CXX_COMPILER "$ENV{CB_BCC64X}" CACHE FILEPATH "BCC64X C++ compiler" FORCE)

set(CMAKE_USER_MAKE_RULES_OVERRIDE
   "${_BCC64X_RULE_OVERRIDE}"
   CACHE FILEPATH "BCC64X CMake rule override" FORCE)

# Configuration-specific optimization and preprocessor flags are supplied by
# the BuildEngine build-variant contract.  The toolchain does not override them.

set(CMAKE_TRY_COMPILE_CONFIGURATION Release CACHE STRING "" FORCE)

get_property(_BCC64X_TOOLCHAIN_SUMMARY_EMITTED GLOBAL PROPERTY ADECC_BCC64X_TOOLCHAIN_SUMMARY_EMITTED)
if(NOT _BCC64X_TOOLCHAIN_SUMMARY_EMITTED)
   message(STATUS "BCC64X C/C++/ASM toolchain: $ENV{CB_BCC64X}")
   message(STATUS "BCC64X rules override: ${_BCC64X_RULE_OVERRIDE}")
   set_property(GLOBAL PROPERTY ADECC_BCC64X_TOOLCHAIN_SUMMARY_EMITTED TRUE)
endif()
unset(_BCC64X_TOOLCHAIN_SUMMARY_EMITTED)
