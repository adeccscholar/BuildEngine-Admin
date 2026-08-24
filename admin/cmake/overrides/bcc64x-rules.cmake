# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# RAD Studio 13 Florence / BCC64X rule overrides for stock Kitware CMake.
#
# This file is loaded through CMAKE_USER_MAKE_RULES_OVERRIDE. CMake loads it
# after its builtin compiler/platform modules, but before compiler checks are
# used. Therefore these settings also apply to try_compile().

get_property(_BCC64X_RULES_SUMMARY_EMITTED GLOBAL PROPERTY ADECC_BCC64X_RULES_SUMMARY_EMITTED)
if(NOT _BCC64X_RULES_SUMMARY_EMITTED)
   message(STATUS "Applying BCC64X make-rules override: ${CMAKE_CURRENT_LIST_FILE}")
   set_property(GLOBAL PROPERTY ADECC_BCC64X_RULES_SUMMARY_EMITTED TRUE)
endif()
unset(_BCC64X_RULES_SUMMARY_EMITTED)


# Stock CMake's Windows-Embarcadero platform module still initializes linker
# flags for the legacy ILINK32/BCC32 toolchain:
#   -lS:<stack reserve> -lSc:<stack commit>
#   -lH:<heap reserve>  -lHc:<heap commit>
# BCC64X forwards -l... to LLD as library names, so these legacy switches are
# invalid. Reset the linker flag initializers before CMake caches them.
foreach(_bcc64x_kind EXE SHARED MODULE)
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_INIT "")
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_DEBUG_INIT "")
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_RELEASE_INIT "")
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_RELWITHDEBINFO_INIT "")
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_MINSIZEREL_INIT "")
endforeach()
unset(_bcc64x_kind)

# Clear cache variables too. This is important for nested try_compile projects
# and makes the override deterministic even if CMake has already initialized
# the variables from its stock Embarcadero platform module.
foreach(_bcc64x_kind EXE SHARED MODULE)
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS "" CACHE STRING "BCC64X linker flags" FORCE)
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_DEBUG "" CACHE STRING "BCC64X Debug linker flags" FORCE)
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_RELEASE "" CACHE STRING "BCC64X Release linker flags" FORCE)
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "BCC64X RelWithDebInfo linker flags" FORCE)
   set(CMAKE_${_bcc64x_kind}_LINKER_FLAGS_MINSIZEREL "" CACHE STRING "BCC64X MinSizeRel linker flags" FORCE)
endforeach()
unset(_bcc64x_kind)

# Modern Win64 uses the BCC64X compiler driver and LLD. Do not inherit legacy
# Borland/Embarcadero default system libraries such as import32.lib.
set(CMAKE_C_STANDARD_LIBRARIES_INIT "")
set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "")
set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "BCC64X C standard libraries" FORCE)
set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "BCC64X C++ standard libraries" FORCE)

# Output formats for Win64 Modern / COFF64.
set(CMAKE_EXECUTABLE_SUFFIX ".exe")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
set(CMAKE_IMPORT_LIBRARY_SUFFIX ".lib")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll" ".a")

# BCC64X Win64 Modern uses the MinGW/LLVM library search convention for bare
# logical library names.  Stock CMake's Windows-Embarcadero platform emits
# e.g. kernel32.lib, while the Modern toolchain ships the Windows import
# libraries primarily as libkernel32.a, libuser32.a, ... .
#
# Embarcadero documents -lname as the supported auto-link search form.  It
# searches the Modern import/static library naming variants, including
# lib<name>.a and <name>.lib.  This affects only logical names passed to
# target_link_libraries(); explicit/absolute library files remain explicit.
set(CMAKE_LINK_LIBRARY_FLAG "-l")
set(CMAKE_LINK_LIBRARY_SUFFIX "")

# Native CMake Windows executable semantics for BCC64X.
# add_executable(target WIN32 ...) / WIN32_EXECUTABLE=ON maps to -tW
# (Windows GUI subsystem, WinMain entry point, no console window).
# A normal add_executable(target ...) maps to -tC (console, main entry point).
# Keep this explicit because the stock Windows-Embarcadero platform module is
# otherwise targeted at several generations of Embarcadero toolchains.
foreach(_bcc64x_lang C CXX)
   set(CMAKE_${_bcc64x_lang}_CREATE_WIN32_EXE "-tW")
   set(CMAKE_${_bcc64x_lang}_CREATE_CONSOLE_EXE "-tC")
endforeach()
unset(_bcc64x_lang)

# Configuration-specific compiler flags belong to the BuildEngine variant
# contract.  Clear legacy Embarcadero initializers here without replacing them
# with hidden Release/Debug policy.
set(CMAKE_C_FLAGS_DEBUG_INIT "")
set(CMAKE_C_FLAGS_RELEASE_INIT "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "")

# Executables: BCC64X/Clang uses -o, not the legacy Borland -e<TARGET> syntax.
set(CMAKE_C_LINK_EXECUTABLE
   "<CMAKE_C_COMPILER> --rsp-quoting=windows -o<TARGET> <LINK_FLAGS> <FLAGS> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE
   "<CMAKE_CXX_COMPILER> --rsp-quoting=windows -o<TARGET> <LINK_FLAGS> <FLAGS> <OBJECTS> <LINK_LIBRARIES>")

# DLLs: let LLD create the COFF import library in the same link invocation.
# Embarcadero documents -Wl,--out-implib,<file.lib> for BCC64X.
set(CMAKE_C_CREATE_SHARED_LIBRARY
   "<CMAKE_C_COMPILER> --rsp-quoting=windows -tD -o<TARGET> <LINK_FLAGS> <FLAGS> <OBJECTS> <LINK_LIBRARIES> -Wl,--out-implib,<TARGET_IMPLIB>")
set(CMAKE_CXX_CREATE_SHARED_LIBRARY
   "<CMAKE_CXX_COMPILER> --rsp-quoting=windows -tD -o<TARGET> <LINK_FLAGS> <FLAGS> <OBJECTS> <LINK_LIBRARIES> -Wl,--out-implib,<TARGET_IMPLIB>")

# MODULE libraries do not require an import library.
set(CMAKE_C_CREATE_SHARED_MODULE
   "<CMAKE_C_COMPILER> --rsp-quoting=windows -tD -o<TARGET> <LINK_FLAGS> <FLAGS> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_CXX_CREATE_SHARED_MODULE
   "<CMAKE_CXX_COMPILER> --rsp-quoting=windows -tD -o<TARGET> <LINK_FLAGS> <FLAGS> <OBJECTS> <LINK_LIBRARIES>")

# Windows resource compiler.
#
# CMake uses the Windows SDK rc.exe directly (absolute path from the toolchain).
# Keep CMake's stock Microsoft RC command line; no CGRC/BRCC32 translation rule
# is required here.

# Static COFF archives. llvm-ar is part of the LLVM toolchain used by BCC64X.
if(DEFINED ENV{CB_LLVM_AR} AND NOT "$ENV{CB_LLVM_AR}" STREQUAL "")
   set(CMAKE_AR "$ENV{CB_LLVM_AR}" CACHE FILEPATH "BCC64X llvm-ar" FORCE)
endif()
if(DEFINED ENV{CB_LLVM_RANLIB} AND NOT "$ENV{CB_LLVM_RANLIB}" STREQUAL "")
   set(CMAKE_RANLIB "$ENV{CB_LLVM_RANLIB}" CACHE FILEPATH "BCC64X llvm-ranlib" FORCE)
endif()

if(CMAKE_AR)
   set(CMAKE_C_CREATE_STATIC_LIBRARY
      "<CMAKE_AR> qc <TARGET> <LINK_FLAGS> <OBJECTS>"
      "<CMAKE_AR> s <TARGET>")
   set(CMAKE_CXX_CREATE_STATIC_LIBRARY
      "<CMAKE_AR> qc <TARGET> <LINK_FLAGS> <OBJECTS>"
      "<CMAKE_AR> s <TARGET>")
endif()
