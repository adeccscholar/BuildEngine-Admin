# SPDX-License-Identifier: MIT
# Versioned BCC64X DXC source preflight, without legacy test migration.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT EXISTS "${SOURCE_ROOT}/projects/dxilconv/CMakeLists.txt")
   message(FATAL_ERROR "DXC source root is missing")
endif()
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

# Restore the original PCH accessor instead of adding a second class method.
# Only this accessor is enabled; unsupported module and PCH writer methods
# remain inside the upstream disabled region.
_adecc_dxc_replace_once("tools/clang/include/clang/Frontend/CompilerInstance.h"
   "  std::shared_ptr<PCHContainerOperations> getPCHContainerOperations() const {"
   "#endif // ADECC: restore original accessor\n  std::shared_ptr<PCHContainerOperations> getPCHContainerOperations() const {")
_adecc_dxc_replace_once("tools/clang/include/clang/Frontend/CompilerInstance.h"
   "  /// Return the appropriate PCHContainerWriter depending on the"
   "#if 0 // ADECC: keep original PCH writer disabled\n  /// Return the appropriate PCHContainerWriter depending on the")

# IN collides with a Windows SDK annotation macro.
_adecc_dxc_replace_once("tools/llvm-diff/DiffConsumer.cpp"
   "unsigned IN = 0;" "unsigned iNextNumber = 0;")
_adecc_dxc_replace_once("tools/llvm-diff/DiffConsumer.cpp"
   "IN++" "iNextNumber++")

# The BCC64X standard library accepts wchar_t const* for wide paths.

# GoogleTest TEST_F methods derive from their test fixture.

# DXC already discovers the DIA SDK for dxcompiler; attach it to dxclib too.
_adecc_dxc_replace_once("tools/clang/tools/dxclib/CMakeLists.txt"
   "add_clang_library(dxclib\n  dxc.cpp\n  )"
   "add_clang_library(dxclib\n  dxc.cpp\n  )\n\nif (WIN32 AND NOT MSVC)\n  find_path(ADECC_DXC_DIA_INCLUDE_DIR dia2.h\n    HINTS\n      \"$ENV{VSINSTALLDIR}/DIA SDK/include\"\n      \"$ENV{MSVC_DIA_SDK_DIR}/include\"\n      \"$ENV{DIA_SDK_DIR}/include\"\n      \"C:/Program Files/Microsoft Visual Studio/2022/Professional/DIA SDK/include\"\n      \"C:/Program Files/Microsoft Visual Studio/2022/Enterprise/DIA SDK/include\"\n      \"C:/Program Files/Microsoft Visual Studio/2022/Community/DIA SDK/include\")\n  if (NOT ADECC_DXC_DIA_INCLUDE_DIR)\n    message(FATAL_ERROR \"DXC dxclib requires DIA SDK dia2.h\")\n  endif()\n  target_include_directories(dxclib PRIVATE \"${ADECC_DXC_DIA_INCLUDE_DIR}\")\nendif()")

# DXC's fixed LangOptions profile declares most flags immutable. ARC migration,
# however, must mutate these two per-invocation flags. Shadow only those two
# fixed defaults with per-instance state on BCC64X; do not globally switch
# to the unrelated variable LangOptions configuration.
_adecc_dxc_replace_once("tools/clang/include/clang/Basic/LangOptions.h"
   "class LangOptions : public LangOptionsBase {\npublic:"
   "class LangOptions : public LangOptionsBase {\npublic:\n#if defined(__CODEGEARC__) || defined(__BORLANDC__)\n  bool ObjCAutoRefCount = false;\n  bool ObjCARCWeak = false;\n#endif")

# The target compiler may report MSVC simulation. Select BCC64X by the
# explicit BuildEngine toolchain contract, never by NOT MSVC.
_adecc_dxc_replace_once("tools/clang/tools/dxclib/CMakeLists.txt"
   "if (WIN32 AND NOT MSVC)\n  find_path(ADECC_DXC_DIA_INCLUDE_DIR"
   "if (ADECC_BCC64X_NATIVE_X64)\n  find_path(ADECC_DXC_DIA_INCLUDE_DIR")

# Confirm the DIA include contract at CMake configure time, not after compiling
# thousands of LLVM translation units. This postcondition is deliberately
# independent of the historical WIN32/NOT MSVC compiler classification.
set(_adecc_dxclib_cmake "${SOURCE_ROOT}/tools/clang/tools/dxclib/CMakeLists.txt")
file(READ "${_adecc_dxclib_cmake}" _adecc_dxclib_contents)
if(NOT _adecc_dxclib_contents MATCHES "ADECC_DXC_DIA_TARGET_CHECK")
   file(APPEND "${_adecc_dxclib_cmake}" [=[

# ADECC_DXC_DIA_TARGET_CHECK: required Windows DIA SDK contract for dxclib.
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(NOT ADECC_DXC_DIA_INCLUDE_DIR OR
     NOT EXISTS "${ADECC_DXC_DIA_INCLUDE_DIR}/dia2.h")
    unset(ADECC_DXC_DIA_INCLUDE_DIR CACHE)
    find_path(ADECC_DXC_DIA_INCLUDE_DIR NAMES dia2.h
      HINTS
        "$ENV{VSINSTALLDIR}/DIA SDK/include"
        "$ENV{MSVC_DIA_SDK_DIR}/include"
        "$ENV{DIA_SDK_DIR}/include"
        "C:/Program Files/Microsoft Visual Studio/2022/Professional/DIA SDK/include"
        "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/DIA SDK/include"
        "C:/Program Files/Microsoft Visual Studio/2022/Community/DIA SDK/include"
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/DIA SDK/include")
  endif()
  if(NOT ADECC_DXC_DIA_INCLUDE_DIR OR
     NOT EXISTS "${ADECC_DXC_DIA_INCLUDE_DIR}/dia2.h")
    message(FATAL_ERROR
      "DXC dxclib requires the genuine Microsoft DIA SDK dia2.h. "
      "Set ADECC_DXC_DIA_INCLUDE_DIR to a verified DIA SDK include directory.")
  endif()
  target_include_directories(dxclib PRIVATE "${ADECC_DXC_DIA_INCLUDE_DIR}")
  get_target_property(_adecc_dia_target_includes dxclib INCLUDE_DIRECTORIES)
  if(NOT _adecc_dia_target_includes)
    message(FATAL_ERROR "DXC dxclib DIA include contract was not attached")
  endif()
  message(STATUS "DXC dxclib DIA SDK: ${ADECC_DXC_DIA_INCLUDE_DIR}")
endif()
]=])
endif()


# Keep standalone C++23 API/compiler regression tests in the build regardless
# of upstream HLSL_INCLUDE_TESTS. The historical Windows test targets do not
# become part of the new native behavior suite.
set(_adecc_dxc_parent "${SOURCE_ROOT}/projects/dxilconv/CMakeLists.txt")
file(READ "${_adecc_dxc_parent}" _adecc_parent)
if(_adecc_parent MATCHES "add_subdirectory\\(portable\\)")
   # Already attached.
elseif(_adecc_parent MATCHES "add_subdirectory\\(tools\\)")
   string(REPLACE "  add_subdirectory(tools)"
      "  add_subdirectory(tools)\\n  add_subdirectory(portable)"
      _adecc_parent "${_adecc_parent}")
   string(REPLACE "\\n" "\n" _adecc_parent "${_adecc_parent}")
   file(WRITE "${_adecc_dxc_parent}" "${_adecc_parent}")
else()
   message(FATAL_ERROR "DXC parent CMake target insertion anchor missing")
endif()
message(STATUS "DXC native behavior CMake integration prepared; no WEX/TAEF translation")
