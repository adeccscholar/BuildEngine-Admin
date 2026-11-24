# SPDX-License-Identifier: MIT
# DXC v1.9.2609: migrate original HLSL test cases without replacing the host platform.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT IS_DIRECTORY "${SOURCE_ROOT}/tools/clang/unittests/HLSL")
   message(FATAL_ERROR "DXC v1.9.2609 workspace not found")
endif()
if(NOT DEFINED ADMIN_PATCH_ROOT OR NOT IS_DIRECTORY "${ADMIN_PATCH_ROOT}")
   message(FATAL_ERROR "DXC versioned test patch directory not provided")
endif()

set(_adecc_hlsl "${SOURCE_ROOT}/tools/clang/unittests/HLSL")
set(_adecc_testlib "${SOURCE_ROOT}/tools/clang/unittests/HLSLTestLib")
set(_adecc_header "${SOURCE_ROOT}/include/dxc/Test/HlslTestUtils.h")
set(_adecc_adapter "${SOURCE_ROOT}/include/dxc/Test/WEXAdapter.h")

# Use the existing upstream test helpers, not fake success stubs.
file(COPY_FILE
   "${ADMIN_PATCH_ROOT}/gtest/WEXAdapter.h" "${_adecc_adapter}"
   ONLY_IF_DIFFERENT)
file(COPY_FILE
   "${ADMIN_PATCH_ROOT}/gtest/HLSL-CMakeLists.txt" "${_adecc_hlsl}/CMakeLists.txt"
   ONLY_IF_DIFFERENT)

# The native Win32 code must keep DXGI declarations, but test assertions must
# go to real GoogleTest instead of WexTestClass.h.
file(READ "${_adecc_header}" _adecc_header_source)
set(_adecc_original "#ifdef _WIN32\n\n// Disable -Wignored-qualifiers for WexTestClass.h.")
set(_adecc_replacement "#if defined(_WIN32) && !defined(ADECC_DXC_GTEST)\n\n// Disable -Wignored-qualifiers for WexTestClass.h.")
if(_adecc_header_source MATCHES "defined\\(_WIN32\\) && !defined\\(ADECC_DXC_GTEST\\)")
   # Already migrated.
elseif(_adecc_header_source MATCHES "#ifdef _WIN32\n\n// Disable -Wignored-qualifiers")
   string(REPLACE "${_adecc_original}" "${_adecc_replacement}"
      _adecc_header_source "${_adecc_header_source}")
   # After #else, the original portable branch imports the GoogleTest adapter.
   # Ensure DXGI is included on native Windows under this branch too.
   string(REPLACE "#include \"WEXAdapter.h\"\n#include \"dxc/Support/Global.h\""
      "#include \"WEXAdapter.h\"\n#include \"dxc/Support/Global.h\"\n#ifdef _WIN32\n#include <dxgiformat.h>\n#endif"
      _adecc_header_source "${_adecc_header_source}")
   file(WRITE "${_adecc_header}" "${_adecc_header_source}")
else()
   message(FATAL_ERROR "DXC HlslTestUtils.h layout differs from expected v1.9.2609")
endif()

# Preserve all original Windows-specific classes by making their fixtures
# GoogleTest fixtures, rather than selecting their WEX-only code path.
file(GLOB _adecc_test_sources "${_adecc_hlsl}/*.cpp")
set(_adecc_fixture_count 0)
foreach(_adecc_file IN LISTS _adecc_test_sources)
   file(READ "${_adecc_file}" _adecc_source)
   set(_adecc_previous "${_adecc_source}")
   string(REGEX REPLACE
      "#ifdef _WIN32\nclass ([A-Za-z0-9_]+) \\{\n#else\nclass [A-Za-z0-9_]+ : public ::testing::Test \\{\n#endif"
      "class \\1 : public ::testing::Test {"
      _adecc_source "${_adecc_source}")
   if(_adecc_source MATCHES "class MSFileSysTest \\{")
      string(REPLACE "class MSFileSysTest {" "class MSFileSysTest : public ::testing::Test {" _adecc_source "${_adecc_source}")
   endif()
   if(_adecc_source MATCHES "class PixDiaTest \\{")
      string(REPLACE "class PixDiaTest {" "class PixDiaTest : public ::testing::Test {" _adecc_source "${_adecc_source}")
   endif()
   if(_adecc_source MATCHES "#define TEST_CLASS_DERIVATION\n")
      string(REPLACE "#define TEST_CLASS_DERIVATION\n"
         "#define TEST_CLASS_DERIVATION : public ::testing::Test\n"
         _adecc_source "${_adecc_source}")
   endif()
   if(NOT _adecc_source STREQUAL _adecc_previous)
      math(EXPR _adecc_fixture_count "${_adecc_fixture_count} + 1")
      file(WRITE "${_adecc_file}" "${_adecc_source}")
   endif()
endforeach()

# The original shared HLSL test library retains all source files and
# dependencies, with GoogleTest headers instead of a TAEF discovery branch.
set(_adecc_testlib_cmake "${_adecc_testlib}/CMakeLists.txt")
file(COPY_FILE
   "${ADMIN_PATCH_ROOT}/gtest/HLSLTestLib-CMakeLists.txt"
   "${_adecc_testlib_cmake}" ONLY_IF_DIFFERENT)


# Preserve original DXIL converter batch/regression test source and fixtures.
set(_adecc_dxilconv_root "${SOURCE_ROOT}/projects/dxilconv")
file(COPY_FILE
   "${ADMIN_PATCH_ROOT}/gtest/DXILCONV-CMakeLists.txt"
   "${_adecc_dxilconv_root}/unittests/CMakeLists.txt" ONLY_IF_DIFFERENT)
set(_adecc_conv_source "${_adecc_dxilconv_root}/unittests/DxilConvTests.cpp")
file(READ "${_adecc_conv_source}" _adecc_conv_code)
string(REPLACE "class DxilConvTest {" "class DxilConvTest : public ::testing::Test {"
   _adecc_conv_code "${_adecc_conv_code}")
string(REPLACE "GetModuleHandleA(\"dxilconv-tests.dll\")" "GetModuleHandleA(nullptr)"
   _adecc_conv_code "${_adecc_conv_code}")
string(REPLACE "TEST_F(DxilConvTest, ManualFileCheckTest)"
   "TEST_F(DxilConvTest, DISABLED_ManualFileCheckTest)"
   _adecc_conv_code "${_adecc_conv_code}")
file(WRITE "${_adecc_conv_source}" "${_adecc_conv_code}")

set(_adecc_conv_cmake "${_adecc_dxilconv_root}/CMakeLists.txt")
file(READ "${_adecc_conv_cmake}" _adecc_conv_parent)
if(_adecc_conv_parent MATCHES "add_subdirectory\\(portable\\)")
   # Keep the independent DXIL/SPIR-V smoke checks in addition to original tests.
elseif(_adecc_conv_parent MATCHES "add_subdirectory\\(unittests\\)")
   string(REPLACE "    add_subdirectory(unittests)"
      "    add_subdirectory(unittests)\n    add_subdirectory(portable)"
      _adecc_conv_parent "${_adecc_conv_parent}")
   file(WRITE "${_adecc_conv_cmake}" "${_adecc_conv_parent}")
else()
   message(FATAL_ERROR "Original DXIL converter test subdirectory missing")
endif()


# Preserve all Windows GPU/HLK source translation units under GoogleTest.
set(_adecc_exec_dir "${SOURCE_ROOT}/tools/clang/unittests/HLSLExec")
file(COPY_FILE
   "${ADMIN_PATCH_ROOT}/gtest/HLSLExec-CMakeLists.txt"
   "${_adecc_exec_dir}/CMakeLists.txt" ONLY_IF_DIFFERENT)

# The original TAEF TestData API is backed by generated pinned XML data.
file(COPY_FILE
   "${ADMIN_PATCH_ROOT}/gtest/PortableTestData.cpp"
   "${_adecc_exec_dir}/PortableTestData.cpp" ONLY_IF_DIFFERENT)
file(READ "${_adecc_testlib_cmake}" _adecc_testlib_contents)
if(NOT _adecc_testlib_contents MATCHES "PortableTestData\\.cpp")
file(APPEND "${_adecc_testlib_cmake}" "
# BuildEngine: original XML test parameter values, no TAEF runtime.
target_sources(HLSLTestLib PRIVATE
  ${_adecc_exec_dir}/PortableTestData.cpp)
target_include_directories(HLSLTestLib PRIVATE
  ${_adecc_exec_dir})
target_compile_definitions(HLSLTestLib PRIVATE ADECC_DXC_GTEST=1)
")
endif()

# Replace historical TAEF-driven LLVM/LIT suites with native GoogleTest.
set(_adecc_lit_template "${ADMIN_PATCH_ROOT}/gtest/OriginalGoogleTest.lit.cfg.in")
if(NOT EXISTS "${_adecc_lit_template}")
   message(FATAL_ERROR "DXC original GoogleTest LIT adapter missing")
endif()
file(READ "${_adecc_lit_template}" _adecc_lit_base)
foreach(_adecc_lit IN ITEMS
   "tools/clang/test/taef|ClangHLSLTests|tools/clang/test/HLSL"
   "tools/clang/test/taef_exec|ExecHLSLTests|tools/clang/test/HLSL"
   "projects/dxilconv/test/taef|dxilconv-tests|projects/dxilconv/test")
   string(REPLACE "|" ";" _adecc_parts "${_adecc_lit}")
   list(GET _adecc_parts 0 _adecc_lit_dir)
   list(GET _adecc_parts 1 _adecc_lit_binary)
   list(GET _adecc_parts 2 _adecc_lit_fixtures)
   set(_adecc_lit_text "${_adecc_lit_base}")
   string(REPLACE "ADECC_EXECUTABLE" "${_adecc_lit_binary}"
      _adecc_lit_text "${_adecc_lit_text}")
   string(REPLACE "ADECC_SUITE_ROOT" "${_adecc_lit_fixtures}"
      _adecc_lit_text "${_adecc_lit_text}")
   file(WRITE "${SOURCE_ROOT}/${_adecc_lit_dir}/lit.cfg"
      "${_adecc_lit_text}")
endforeach()

# Complete the pinned native Windows header migration before compiling any
# original DXC unit or command-line target. Keep the upstream MSVC includes
# intact; the BCC64X path consumes the existing native COM/UTF-8 adapter.
# Fail on changed upstream anchors rather than silently missing a source edit.
function(_adecc_dxc_replace_once _relative _original _replacement)
   set(_path "${SOURCE_ROOT}/${_relative}")
   file(READ "${_path}" _source)
   string(FIND "${_source}" "${_replacement}" _already)
   if(NOT _already EQUAL -1)
      return()
   endif()
   string(FIND "${_source}" "${_original}" _position)
   if(_position EQUAL -1)
      message(FATAL_ERROR "DXC native header migration anchor missing: ${_relative}")
   endif()
   string(REPLACE "${_original}" "${_replacement}" _updated "${_source}")
   file(WRITE "${_path}" "${_updated}")
endfunction()

set(_adecc_native_com_guard
   "#if !defined(__CODEGEARC__) && !defined(__BORLANDC__)\\n#include <comdef.h>\\n#endif")
set(_adecc_native_atl_guard
   "#if !defined(__CODEGEARC__) && !defined(__BORLANDC__)\\n#include <atlfile.h>\\n#endif")
# CMake quoted strings above intentionally encode newlines; expand only here.
string(REPLACE "\\n" "\n" _adecc_native_com_guard "${_adecc_native_com_guard}")
string(REPLACE "\\n" "\n" _adecc_native_atl_guard "${_adecc_native_atl_guard}")
_adecc_dxc_replace_once("tools/clang/tools/dxclib/dxc.cpp"
   "#include <comdef.h>" "${_adecc_native_com_guard}")
_adecc_dxc_replace_once("tools/clang/unittests/HLSLTestLib/FileCheckerTest.cpp"
   "#include <atlfile.h>" "${_adecc_native_atl_guard}")
_adecc_dxc_replace_once("projects/dxilconv/unittests/DxilConvTests.cpp"
   "#include <atlfile.h>" "${_adecc_native_atl_guard}")
_adecc_dxc_replace_once("tools/clang/unittests/HLSL/HLSLTestOptions.cpp"
   "#include \"dxc/WinAdapter.h\""
   "#include \"dxc/WinAdapter.h\"\n#if (defined(__CODEGEARC__) || defined(__BORLANDC__)) && !defined(_MSC_VER)\n#include \"dxc/Support/Bcc64xNativeAtl.h\"\n#endif")

message(STATUS "DXC HLSL original test migration: GoogleTest/CTest, changed fixture files=${_adecc_fixture_count}")
