# Portable CTest coverage for the DXC and dxilconv toolchain.
# Microsoft-specific DxilConvTests.cpp has not yet been completely ported.
# Do not report these tests as equivalent to all historical TAEF test cases.
if(NOT TARGET dxc)
  message(FATAL_ERROR "DXC portable tests require the original dxc target")
endif()

add_test(NAME DXC.Portable.DXIL
  COMMAND "${CMAKE_COMMAND}"
    "-DDXC_EXE=$<TARGET_FILE:dxc>"
    "-DSHADER=${CMAKE_CURRENT_SOURCE_DIR}/smoke.hlsl"
    "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/portable-dxil.bin"
    "-DMODE=DXIL"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/RunShader.cmake")

add_test(NAME DXC.Portable.SPIRV
  COMMAND "${CMAKE_COMMAND}"
    "-DDXC_EXE=$<TARGET_FILE:dxc>"
    "-DSHADER=${CMAKE_CURRENT_SOURCE_DIR}/smoke.hlsl"
    "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/portable-spirv.spv"
    "-DMODE=SPIRV"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/RunShader.cmake")

set_tests_properties(DXC.Portable.DXIL DXC.Portable.SPIRV
  PROPERTIES LABELS "BCC64X;DXC;portable" TIMEOUT 60)
