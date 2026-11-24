# SPDX-License-Identifier: MIT
# Native DXC behavior tests, independent of Microsoft test-runtime adapters.
# Each test executes the actual DXC compiler and checks the observable result.
# This suite is additive: it does not claim equivalence with upstream regressions.

function(adecc_add_dxc_behavior_test name mode entry output)
  add_test(NAME "${name}"
    COMMAND "${CMAKE_COMMAND}"
      "-DDXC_EXE=$<TARGET_FILE:dxc>"
      "-DSHADER=${CMAKE_CURRENT_SOURCE_DIR}/smoke.hlsl"
      "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/${output}"
      "-DMODE=${mode}"
      "-DENTRY=${entry}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/RunShader.cmake")
  set_tests_properties("${name}"
    PROPERTIES LABELS "BCC64X;DXC;behavior" TIMEOUT 60)
endfunction()

adecc_add_dxc_behavior_test(DXC.Behavior.DXIL DXIL main behavioral-dxil.bin)
adecc_add_dxc_behavior_test(DXC.Behavior.SPIRV SPIRV main behavioral-spirv.spv)
adecc_add_dxc_behavior_test(DXC.Behavior.MissingEntry REJECT nonexistent_entry behavioral-invalid.bin)

# The compiler factory is COM-shaped but does not use COM activation.
# Link the real DXC shared library; no ATL, WEX, WRL, te.exe or mocks.
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
  LABELS "BCC64X;DXC;behavior;API" TIMEOUT 120)
