cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeAceTao LANGUAGES CXX)

# The ACE/TAO versions share a physical extension package. Resolve every
# build input inside that exact package, never through the machine PATH,
# system registry, other installed ACE/TAO versions, or previous CMake caches.
if(NOT DEFINED ACE_TAO_PACKAGE_ROOT OR NOT IS_DIRECTORY "${ACE_TAO_PACKAGE_ROOT}")
   message(FATAL_ERROR "ACE_TAO_PACKAGE_ROOT must select the installed ACE/TAO package")
endif()
get_filename_component(ACE_TAO_PACKAGE_ROOT "${ACE_TAO_PACKAGE_ROOT}" REALPATH)
set(_ace_tao_include "${ACE_TAO_PACKAGE_ROOT}/include")
set(_ace_tao_lib "${ACE_TAO_PACKAGE_ROOT}/lib/win64/Release")
set(_ace_tao_tool "${ACE_TAO_PACKAGE_ROOT}/tools/bin/win64/Release")
set(_ace_tao_service "${ACE_TAO_PACKAGE_ROOT}/services/bin/win64/Release/tao_cosnaming.exe")
foreach(_ace_tao_required
   "${_ace_tao_include}/ace/ACE.h"
   "${_ace_tao_include}/tao/corba.h"
   "${_ace_tao_include}/orbsvcs/CosNamingC.h"
   "${_ace_tao_service}")
   if(NOT EXISTS "${_ace_tao_required}")
      message(FATAL_ERROR "ACE/TAO package file missing: ${_ace_tao_required}")
   endif()
endforeach()
set(ACE_TAO_INCLUDE_DIR "${_ace_tao_include}")
foreach(_ace_tao_variable
   ACE_LIBRARY TAO_LIBRARY TAO_ANYTYPECODE_LIBRARY
   TAO_PORTABLESERVER_LIBRARY TAO_COSNAMING_LIBRARY TAO_IDL_EXECUTABLE)
   unset(${_ace_tao_variable} CACHE)
endforeach()
find_library(ACE_LIBRARY NAMES ACE_br ACE
   PATHS "${_ace_tao_lib}" NO_DEFAULT_PATH REQUIRED)
find_library(TAO_LIBRARY NAMES TAO_br TAO
   PATHS "${_ace_tao_lib}" NO_DEFAULT_PATH REQUIRED)
find_library(TAO_ANYTYPECODE_LIBRARY NAMES TAO_AnyTypeCode_br TAO_AnyTypeCode
   PATHS "${_ace_tao_lib}" NO_DEFAULT_PATH REQUIRED)
find_library(TAO_PORTABLESERVER_LIBRARY NAMES TAO_PortableServer_br TAO_PortableServer
   PATHS "${_ace_tao_lib}" NO_DEFAULT_PATH REQUIRED)
find_library(TAO_COSNAMING_LIBRARY NAMES TAO_CosNaming_br TAO_CosNaming
   PATHS "${_ace_tao_lib}" NO_DEFAULT_PATH REQUIRED)
find_program(TAO_IDL_EXECUTABLE NAMES tao_idl.exe
   PATHS "${_ace_tao_tool}" NO_DEFAULT_PATH REQUIRED)
message(STATUS "ACE/TAO package: ${ACE_TAO_PACKAGE_ROOT}")
message(STATUS "ACE/TAO IDL tool: ${TAO_IDL_EXECUTABLE}")
message(STATUS "ACE/TAO naming service: ${_ace_tao_service}")
foreach(_ace_tao_library
   ACE_LIBRARY TAO_LIBRARY TAO_ANYTYPECODE_LIBRARY
   TAO_PORTABLESERVER_LIBRARY TAO_COSNAMING_LIBRARY)
   message(STATUS "ACE/TAO import: ${${_ace_tao_library}}")
endforeach()

set(TAO_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${TAO_GENERATED_DIR}")
execute_process(
   COMMAND "${TAO_IDL_EXECUTABLE}" -o "${TAO_GENERATED_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Smoke.idl"
   RESULT_VARIABLE TAO_IDL_RESULT
   OUTPUT_VARIABLE TAO_IDL_STDOUT
   ERROR_VARIABLE TAO_IDL_STDERR
)
if(NOT TAO_IDL_RESULT EQUAL 0)
   message(FATAL_ERROR "tao_idl failed with ${TAO_IDL_RESULT}\n${TAO_IDL_STDOUT}\n${TAO_IDL_STDERR}")
endif()
if(NOT EXISTS "${TAO_GENERATED_DIR}/SmokeC.h" OR NOT EXISTS "${TAO_GENERATED_DIR}/SmokeC.cpp")
   message(FATAL_ERROR "tao_idl did not generate SmokeC.h/SmokeC.cpp")
endif()

add_executable(ace-tao-consumer
   main.cpp
   "${TAO_GENERATED_DIR}/SmokeC.cpp"
)
set_property(TARGET ace-tao-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(ace-tao-consumer PRIVATE cxx_std_23)

# ACE's Embarcadero configuration requires the compiler's multi-threaded
# runtime mode.  Do not emulate this by defining __MT__ manually: -tWM makes
# the compiler select the matching RTL and defines the compiler contract.
target_compile_options(ace-tao-consumer PRIVATE -tWM)
target_link_options(ace-tao-consumer PRIVATE -tWM)

target_include_directories(ace-tao-consumer PRIVATE
   "${ACE_TAO_INCLUDE_DIR}"
   "${TAO_GENERATED_DIR}"
)
target_link_libraries(ace-tao-consumer PRIVATE
   "${ACE_LIBRARY}"
   "${TAO_LIBRARY}"
   "${TAO_ANYTYPECODE_LIBRARY}"
   "${TAO_PORTABLESERVER_LIBRARY}"
   "${TAO_COSNAMING_LIBRARY}"
)
