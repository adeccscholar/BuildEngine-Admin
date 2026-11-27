# SPDX-License-Identifier: MIT
# Native BCC64X DXC runtime preflight, file diagnostics only.
cmake_minimum_required(VERSION 3.31)
foreach(strRequired IN ITEMS DXC_PACKAGE_ROOT SPIRV_TOOLS_PACKAGE_ROOT BCC64X_TOOLCHAIN NINJA_EXECUTABLE CMAKE_EXECUTABLE PROBE_SOURCE_ROOT PROBE_BUILD_ROOT REPORT_FILE CONFIGURATION)
   if(NOT DEFINED ${strRequired} OR "${${strRequired}}" STREQUAL "")
      message(FATAL_ERROR "DXC runtime preflight: missing ${strRequired}")
   endif()
endforeach()
set(strDxcExe "${DXC_PACKAGE_ROOT}/bin/dxc.exe")
set(strDxcDll "${DXC_PACKAGE_ROOT}/bin/dxcompiler.dll")
get_filename_component(strDxcDll "${strDxcDll}" ABSOLUTE)
get_filename_component(strDxcExe "${strDxcExe}" ABSOLUTE)
get_filename_component(strReportDirectory "${REPORT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${strReportDirectory}")
file(WRITE "${REPORT_FILE}" "DXC runtime preflight\nconfiguration=${CONFIGURATION}\ndxc=${strDxcExe}\ndxcompiler=${strDxcDll}\nSPIRV root=${SPIRV_TOOLS_PACKAGE_ROOT}\nPATH=$ENV{PATH}\n")
file(GLOB vecDxcDlls "${DXC_PACKAGE_ROOT}/bin/*.dll" "${DXC_PACKAGE_ROOT}/bin/win64/${CONFIGURATION}/*.dll")
file(GLOB vecSpirvDlls "${SPIRV_TOOLS_PACKAGE_ROOT}/bin/*.dll" "${SPIRV_TOOLS_PACKAGE_ROOT}/bin/win64/${CONFIGURATION}/*.dll")
foreach(strDll IN LISTS vecDxcDlls vecSpirvDlls)
   file(APPEND "${REPORT_FILE}" "available-dll=${strDll}\n")
endforeach()
if(NOT EXISTS "${strDxcExe}" OR NOT EXISTS "${strDxcDll}")
   file(APPEND "${REPORT_FILE}" "ERROR: dxc.exe or dxcompiler.dll missing\n")
   message(FATAL_ERROR "DXC runtime preflight: package runtime file missing; see ${REPORT_FILE}")
endif()
execute_process(
   COMMAND "${CMAKE_EXECUTABLE}" -S "${PROBE_SOURCE_ROOT}" -B "${PROBE_BUILD_ROOT}" -G Ninja
      "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${BCC64X_TOOLCHAIN}"
      "-DCMAKE_MAKE_PROGRAM:FILEPATH=${NINJA_EXECUTABLE}"
      "-DCMAKE_BUILD_TYPE:STRING=Release"
      "-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF"
   RESULT_VARIABLE iConfigure OUTPUT_VARIABLE strConfigureOut ERROR_VARIABLE strConfigureErr)
file(APPEND "${REPORT_FILE}" "\nPROBE CONFIGURE exit=${iConfigure}\n${strConfigureOut}\n${strConfigureErr}\n")
if(NOT "${iConfigure}" STREQUAL "0")
   message(FATAL_ERROR "DXC runtime preflight: probe configure failed; see ${REPORT_FILE}")
endif()
execute_process(
   COMMAND "${CMAKE_EXECUTABLE}" --build "${PROBE_BUILD_ROOT}"
   RESULT_VARIABLE iBuild OUTPUT_VARIABLE strBuildOut ERROR_VARIABLE strBuildErr)
file(APPEND "${REPORT_FILE}" "\nPROBE BUILD exit=${iBuild}\n${strBuildOut}\n${strBuildErr}\n")
if(NOT "${iBuild}" STREQUAL "0")
   message(FATAL_ERROR "DXC runtime preflight: probe build failed; see ${REPORT_FILE}")
endif()
set(strProbeExe "${PROBE_BUILD_ROOT}/dxc-runtime-probe.exe")
execute_process(
   COMMAND "${strProbeExe}" "${strDxcDll}"
   RESULT_VARIABLE iProbe OUTPUT_VARIABLE strProbeOut ERROR_VARIABLE strProbeErr)
file(APPEND "${REPORT_FILE}" "\nNATIVE LOADER exit=${iProbe}\n${strProbeOut}\n${strProbeErr}\n")
if(NOT "${iProbe}" STREQUAL "0")
   message(FATAL_ERROR "DXC runtime preflight: DLL load failed: ${strProbeErr}; report=${REPORT_FILE}")
endif()
set(strShader "${strReportDirectory}/dxc-runtime-probe.hlsl")
set(strDxil "${strReportDirectory}/dxc-runtime-probe.dxil")
file(WRITE "${strShader}"
   "RWByteAddressBuffer outputBuffer : register(u0);\n"
   "[numthreads(1, 1, 1)] void main() { outputBuffer.Store(0, 1); }\n")
execute_process(
   COMMAND "${strDxcExe}" -T cs_6_0 -E main -Fo "${strDxil}" "${strShader}"
   RESULT_VARIABLE iCompile OUTPUT_VARIABLE strCompileOut ERROR_VARIABLE strCompileErr)
file(APPEND "${REPORT_FILE}" "\nDXC EXECUTION exit=${iCompile}\n${strCompileOut}\n${strCompileErr}\n")
if(NOT "${iCompile}" STREQUAL "0" OR NOT EXISTS "${strDxil}")
   message(FATAL_ERROR "DXC runtime preflight: dxc.exe shader compilation failed (code=${iCompile}): ${strCompileErr}; report=${REPORT_FILE}")
endif()
file(APPEND "${REPORT_FILE}" "\nDXC_RUNTIME|RESULT|PASS\n")
