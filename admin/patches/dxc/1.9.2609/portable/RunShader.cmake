# SPDX-License-Identifier: MIT
# Exercise the actual DXC compiler and validate successful AND rejected input.
# No WEX, ATL, TAEF, fake COM or test-framework adapter is involved.
foreach(_required DXC_EXE SHADER OUTPUT MODE)
  if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
    message(FATAL_ERROR "Missing DXC behavioral test argument: ${_required}")
  endif()
endforeach()
if(NOT EXISTS "${DXC_EXE}" OR NOT EXISTS "${SHADER}")
  message(FATAL_ERROR "DXC compiler or shader fixture is missing")
endif()

if(NOT DEFINED PROFILE)
  set(PROFILE ps_6_0)
endif()
if(NOT DEFINED ENTRY)
  set(ENTRY main)
endif()
file(REMOVE "${OUTPUT}")
set(_args -T "${PROFILE}" -E "${ENTRY}")
if(DEFINED INCLUDE_DIR AND NOT "${INCLUDE_DIR}" STREQUAL "")
  if(NOT IS_DIRECTORY "${INCLUDE_DIR}")
    message(FATAL_ERROR "DXC include directory not found: ${INCLUDE_DIR}")
  endif()
  list(APPEND _args -I "${INCLUDE_DIR}")
endif()
if(MODE STREQUAL "SPIRV")
  list(APPEND _args -spirv)
elseif(NOT MODE STREQUAL "DXIL" AND NOT MODE STREQUAL "REJECT")
  message(FATAL_ERROR "Unsupported behavioral test mode: ${MODE}")
endif()

execute_process(
  COMMAND "${DXC_EXE}" ${_args} -Fo "${OUTPUT}" "${SHADER}"
  RESULT_VARIABLE _result
  OUTPUT_VARIABLE _stdout
  ERROR_VARIABLE _stderr
  TIMEOUT 45)

# Loader failures are infrastructure errors, not shader compilation results.
# In particular, ERROR_MOD_NOT_FOUND (0x8007007E) must never count as
# successful rejection of an intentionally invalid shader.
if("${_stdout}\n${_stderr}" MATCHES "dxcompiler[.]dll failed to load" OR
   "${_result}" STREQUAL "-2147024770")
  message(FATAL_ERROR
    "DXC runtime failed to load dxcompiler.dll or a dependent DLL: ${_result}\n${_stdout}\n${_stderr}")
endif()

if(MODE STREQUAL "REJECT")
  if("${_result}" STREQUAL "0")
    message(FATAL_ERROR "DXC accepted an intentionally invalid shader: ${SHADER}")
  endif()
  if(EXISTS "${OUTPUT}")
    message(FATAL_ERROR "DXC left an output artifact for rejected shader: ${OUTPUT}")
  endif()
  if("${_stdout}${_stderr}" STREQUAL "")
    message(FATAL_ERROR "DXC rejected input without reporting a diagnostic")
  endif()
  message(STATUS "DXC correctly rejected invalid input: ${SHADER}")
  return()
endif()

if(NOT "${_result}" STREQUAL "0")
  message(FATAL_ERROR "DXC ${MODE} compilation failed (${_result}): ${_stdout}\n${_stderr}")
endif()
if(NOT EXISTS "${OUTPUT}")
  message(FATAL_ERROR "DXC ${MODE} compilation produced no output")
endif()
file(SIZE "${OUTPUT}" _size)
if(_size LESS 16)
  message(FATAL_ERROR "DXC ${MODE} output unexpectedly small: ${_size}")
endif()
file(READ "${OUTPUT}" _magic LIMIT 4 HEX)
string(TOLOWER "${_magic}" _magic)
if(MODE STREQUAL "SPIRV" AND NOT _magic STREQUAL "03022307")
  message(FATAL_ERROR "Invalid SPIR-V magic: ${_magic}")
elseif(MODE STREQUAL "DXIL" AND NOT _magic STREQUAL "44584243")
  message(FATAL_ERROR "Invalid DXIL container magic: ${_magic}")
endif()
message(STATUS "DXC ${MODE} behavioral test passed (${_size} bytes): ${SHADER}")
