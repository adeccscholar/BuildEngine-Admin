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

# DXC's fixed LangOptions profile declares most flags immutable. ARC migration,
# however, must mutate these two per-invocation flags. Shadow only those two
# fixed defaults with per-instance state on BCC64X; do not globally switch
# to the unrelated variable LangOptions configuration.
_adecc_dxc_replace_once("tools/clang/include/clang/Basic/LangOptions.h"
   "class LangOptions : public LangOptionsBase {\npublic:"
   "class LangOptions : public LangOptionsBase {\npublic:\n#if defined(__CODEGEARC__) || defined(__BORLANDC__)\n  bool ObjCAutoRefCount = false;\n  bool ObjCARCWeak = false;\n#endif")

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
