# SPDX-License-Identifier: MIT
# Independent native DXC C++23 and HLSL behavior suite.
# No legacy TAEF/WEX/ATL test registration or test translation.
# All tests exercise production dxcompiler or the production dxc executable.

function(adecc_add_dxc_shader_test name mode shader profile entry)
  add_test(NAME "DXC.Behavior.${name}"
    COMMAND "${CMAKE_COMMAND}"
      "-DDXC_EXE=$<TARGET_FILE:dxc>"
      "-DSHADER=${CMAKE_CURRENT_SOURCE_DIR}/${shader}"
      "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
      "-DMODE=${mode}"
      "-DPROFILE=${profile}"
      "-DENTRY=${entry}"
      "-DINCLUDE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/RunShader.cmake")
  set_tests_properties("DXC.Behavior.${name}"
    PROPERTIES LABELS "BCC64X;DXC;native;HLSL" TIMEOUT 60)
endfunction()

adecc_add_dxc_shader_test(PixelDXIL DXIL smoke.hlsl ps_6_0 main)
adecc_add_dxc_shader_test(PixelSPIRV SPIRV smoke.hlsl ps_6_0 main)
adecc_add_dxc_shader_test(ComputeDXIL DXIL compute.hlsl cs_6_0 main)
adecc_add_dxc_shader_test(ComputeSPIRV SPIRV compute.hlsl cs_6_0 main)
adecc_add_dxc_shader_test(IncludeDXIL DXIL include-test.hlsl ps_6_0 main)
adecc_add_dxc_shader_test(MissingEntry REJECT smoke.hlsl ps_6_0 nonexistent_entry)
adecc_add_dxc_shader_test(InvalidSyntax REJECT invalid-syntax.hlsl ps_6_0 main)

# Exercise the true native DXC COM-shaped ABI. No COM activation is needed.
add_executable(adecc-dxc-api-behavior DxcApiBehavior.cpp)
set_target_properties(adecc-dxc-api-behavior PROPERTIES
  CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF
  CXX_SCAN_FOR_MODULES OFF)
target_include_directories(adecc-dxc-api-behavior PRIVATE
  "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(adecc-dxc-api-behavior PRIVATE dxcompiler)
add_dependencies(adecc-dxc-api-behavior dxcompiler)
add_test(NAME DXC.Behavior.NativeApi
  COMMAND "$<TARGET_FILE:adecc-dxc-api-behavior>")
set_tests_properties(DXC.Behavior.NativeApi PROPERTIES
  LABELS "BCC64X;DXC;native;API" TIMEOUT 120)
