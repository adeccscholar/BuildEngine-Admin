# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
#
# Native Windows/BCC64X build adapter for TECkit 2.5.13.
# Upstream Makefile.am files remain the source inventory reference; this file
# only expresses that inventory with CMake/Ninja so no Autotools runtime is
# needed on Windows.
#
# Dependency inputs are producer artifacts from the completed dependency Build
# states.  They deliberately do not consume InstallRoot: the library FSM gates
# Build on dependency > Build, not on dependency Install.

cmake_minimum_required(VERSION 3.25)

project(BuildEngineTECkit VERSION 2.5.13 LANGUAGES CXX)

include(GNUInstallDirs)

foreach(_required
      TECKIT_SOURCE_DIR
      TECKIT_ZLIB_INCLUDE_DIR
      TECKIT_ZLIB_GENERATED_INCLUDE_DIR
      TECKIT_ZLIB_LIBRARY
      TECKIT_ZLIB_RUNTIME
      TECKIT_EXPAT_INCLUDE_DIR
      TECKIT_EXPAT_LIBRARY
      TECKIT_EXPAT_RUNTIME)
   if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
      message(FATAL_ERROR "${_required} is not set")
   endif()
endforeach()

get_filename_component(TECKIT_SOURCE_DIR "${TECKIT_SOURCE_DIR}" ABSOLUTE)
if(NOT EXISTS "${TECKIT_SOURCE_DIR}/source/Engine.cpp")
   message(FATAL_ERROR "TECkit source tree is invalid: ${TECKIT_SOURCE_DIR}")
endif()

foreach(_required_file
      "${TECKIT_ZLIB_LIBRARY}"
      "${TECKIT_ZLIB_RUNTIME}"
      "${TECKIT_EXPAT_LIBRARY}"
      "${TECKIT_EXPAT_RUNTIME}")
   if(NOT EXISTS "${_required_file}")
      message(FATAL_ERROR "TECkit dependency Build artifact is missing: ${_required_file}")
   endif()
endforeach()

if(NOT EXISTS "${TECKIT_ZLIB_INCLUDE_DIR}/zlib.h")
   message(FATAL_ERROR "TECkit ZLib source headers are missing: ${TECKIT_ZLIB_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${TECKIT_ZLIB_GENERATED_INCLUDE_DIR}/zconf.h")
   message(FATAL_ERROR "TECkit ZLib generated headers are missing: ${TECKIT_ZLIB_GENERATED_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${TECKIT_EXPAT_INCLUDE_DIR}/expat.h")
   message(FATAL_ERROR "TECkit Expat headers are missing: ${TECKIT_EXPAT_INCLUDE_DIR}")
endif()

add_library(BuildEngineZLib SHARED IMPORTED GLOBAL)
set_target_properties(BuildEngineZLib PROPERTIES
   IMPORTED_IMPLIB "${TECKIT_ZLIB_LIBRARY}"
   IMPORTED_LOCATION "${TECKIT_ZLIB_RUNTIME}"
   INTERFACE_INCLUDE_DIRECTORIES "${TECKIT_ZLIB_GENERATED_INCLUDE_DIR};${TECKIT_ZLIB_INCLUDE_DIR}"
   INTERFACE_COMPILE_DEFINITIONS "ZLIB_DLL")

add_library(BuildEngineExpat SHARED IMPORTED GLOBAL)
set_target_properties(BuildEngineExpat PROPERTIES
   IMPORTED_IMPLIB "${TECKIT_EXPAT_LIBRARY}"
   IMPORTED_LOCATION "${TECKIT_EXPAT_RUNTIME}"
   INTERFACE_INCLUDE_DIRECTORIES "${TECKIT_EXPAT_INCLUDE_DIR}")

set(_TECKIT_PUBLIC_DIR "${TECKIT_SOURCE_DIR}/source/Public-headers")
set(_TECKIT_SOURCE_DIR "${TECKIT_SOURCE_DIR}/source")
set(_TECKIT_SFCONV_DIR "${TECKIT_SOURCE_DIR}/SFconv")

add_library(TECkit SHARED
   "${_TECKIT_SOURCE_DIR}/Engine.cpp")

target_compile_features(TECkit PRIVATE cxx_std_11)
target_compile_definitions(TECkit PRIVATE WIN32)
target_compile_definitions(TECkit INTERFACE "EXPORTED=")
target_include_directories(TECkit
   PUBLIC
      "$<BUILD_INTERFACE:${_TECKIT_PUBLIC_DIR}>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/TECkit>"
   PRIVATE
      "${_TECKIT_SOURCE_DIR}")
target_link_libraries(TECkit PRIVATE BuildEngineZLib)

add_library(TECkit_Compiler SHARED
   "${_TECKIT_SOURCE_DIR}/Compiler.cpp"
   "${_TECKIT_SOURCE_DIR}/UnicodeNames.cpp")

target_compile_features(TECkit_Compiler PRIVATE cxx_std_11)
target_compile_definitions(TECkit_Compiler PRIVATE WIN32)
target_compile_definitions(TECkit_Compiler INTERFACE "EXPORTED=")
target_include_directories(TECkit_Compiler
   PUBLIC
      "$<BUILD_INTERFACE:${_TECKIT_PUBLIC_DIR}>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/TECkit>"
   PRIVATE
      "${_TECKIT_SOURCE_DIR}")
target_link_libraries(TECkit_Compiler PRIVATE BuildEngineZLib)

foreach(_target TECkit TECkit_Compiler)
   set_target_properties(${_target} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endforeach()

add_executable(teckit_compile
   "${_TECKIT_SOURCE_DIR}/Sample-tools/TECkit_Compile.cpp")
target_compile_features(teckit_compile PRIVATE cxx_std_11)
target_link_libraries(teckit_compile PRIVATE TECkit_Compiler)

add_executable(txtconv
   "${_TECKIT_SOURCE_DIR}/Sample-tools/TxtConv.cpp")
target_compile_features(txtconv PRIVATE cxx_std_11)
target_compile_definitions(txtconv PRIVATE WIN32)
target_link_libraries(txtconv PRIVATE TECkit)

add_executable(sfconv
   "${_TECKIT_SFCONV_DIR}/SFconv.cpp"
   "${_TECKIT_SFCONV_DIR}/UtfCodec.cpp")
target_compile_features(sfconv PRIVATE cxx_std_11)
target_compile_definitions(sfconv PRIVATE
   WIN32
   HAVE_LIBEXPAT=1
   "platformUTF16=kForm_UTF16LE")
target_include_directories(sfconv PRIVATE
   "${_TECKIT_SFCONV_DIR}")
target_link_libraries(sfconv PRIVATE
   TECkit
   BuildEngineExpat)

foreach(_target teckit_compile txtconv sfconv)
   set_target_properties(${_target} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endforeach()

# The unchanged upstream Perl regression suite writes its temporary products
# into the working directory supplied by BuildEngine.
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/test")

install(TARGETS
   TECkit
   TECkit_Compiler
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(TARGETS
   teckit_compile
   txtconv
   sfconv
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES
   "${_TECKIT_PUBLIC_DIR}/TECkit_Common.h"
   "${_TECKIT_PUBLIC_DIR}/TECkit_Engine.h"
   "${_TECKIT_PUBLIC_DIR}/TECkit_Compiler.h"
   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/TECkit")
