cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeAceTao LANGUAGES CXX)

find_path(ACE_TAO_INCLUDE_DIR NAMES ace/ACE.h REQUIRED)
if(NOT EXISTS "${ACE_TAO_INCLUDE_DIR}/tao/corba.h")
   message(FATAL_ERROR "TAO public headers are missing below ${ACE_TAO_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${ACE_TAO_INCLUDE_DIR}/orbsvcs/CosNamingC.h")
   message(FATAL_ERROR "TAO CosNaming public headers are missing below ${ACE_TAO_INCLUDE_DIR}")
endif()

find_library(ACE_LIBRARY NAMES ACE_br ACE REQUIRED)
find_library(TAO_LIBRARY NAMES TAO_br TAO REQUIRED)
find_library(TAO_PORTABLESERVER_LIBRARY NAMES TAO_PortableServer_br TAO_PortableServer REQUIRED)
find_library(TAO_COSNAMING_LIBRARY NAMES TAO_CosNaming_br TAO_CosNaming REQUIRED)
find_program(TAO_IDL_EXECUTABLE NAMES tao_idl.exe tao_idl REQUIRED)

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
   "${TAO_PORTABLESERVER_LIBRARY}"
   "${TAO_COSNAMING_LIBRARY}"
)
