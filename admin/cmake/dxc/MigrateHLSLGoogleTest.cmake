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

# Port the shared test utility target without a TAEF discovery side effect.
set(_adecc_testlib_cmake "${_adecc_testlib}/CMakeLists.txt")
file(READ "${_adecc_testlib_cmake}" _adecc_testlib_contents)
if(_adecc_testlib_contents MATCHES "find_package\\(TAEF")
   string(REPLACE "if(WIN32)\nfind_package(TAEF REQUIRED)\ninclude_directories(${TAEF_INCLUDE_DIRS})\nelse(WIN32)"
      "if(TRUE)" _adecc_testlib_contents "${_adecc_testlib_contents}")
   string(REPLACE "endif(WIN32)" "endif()" _adecc_testlib_contents "${_adecc_testlib_contents}")
   if(_adecc_testlib_contents MATCHES "find_package\\(TAEF")
      message(FATAL_ERROR "Not all original HLSLTestLib TAEF references migrated")
   endif()
   file(WRITE "${_adecc_testlib_cmake}" "${_adecc_testlib_contents}")
endif()

message(STATUS "DXC HLSL original test migration: GoogleTest/CTest, changed fixture files=${_adecc_fixture_count}")
