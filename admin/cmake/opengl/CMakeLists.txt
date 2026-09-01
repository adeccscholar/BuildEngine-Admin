cmake_minimum_required(VERSION 3.25)
project(AdeccOpenGLDevelopmentPackage NONE)
foreach(_v OPENGL_RUNTIME TDUMP_EXECUTABLE LD_LLD_EXECUTABLE)
  if(NOT DEFINED ${_v} OR "${${_v}}" STREQUAL "")
    message(FATAL_ERROR "${_v} is required")
  endif()
endforeach()
set(_def "${CMAKE_CURRENT_BINARY_DIR}/opengl32.def")
set(_lib "${CMAKE_CURRENT_BINARY_DIR}/opengl32.lib")
add_custom_command(OUTPUT "${_lib}"
  COMMAND "${TDUMP_EXECUTABLE}" -def "${OPENGL_RUNTIME}" "${_def}"
  COMMAND "${LD_LLD_EXECUTABLE}" -m i386pep --out-implib "${_lib}" "${_def}"
  DEPENDS "${OPENGL_RUNTIME}" VERBATIM)
add_custom_target(opengl-import ALL DEPENDS "${_lib}")
