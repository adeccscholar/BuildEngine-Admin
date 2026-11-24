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
