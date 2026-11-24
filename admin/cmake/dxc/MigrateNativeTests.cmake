# SPDX-License-Identifier: MIT
# DXC 1.9.2609: ATL-free original test migration before compilation.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT EXISTS "${SOURCE_ROOT}/tools/clang/unittests")
   message(FATAL_ERROR "DXC native test migration requires pinned source tree")
endif()

file(GLOB_RECURSE _adecc_original_tests LIST_DIRECTORIES false
   "${SOURCE_ROOT}/tools/clang/unittests/*.cpp"
   "${SOURCE_ROOT}/tools/clang/unittests/*.h")
set(_adecc_count 0)
foreach(_adecc_file IN LISTS _adecc_original_tests)
   file(READ "${_adecc_file}" _adecc_before)
   set(_adecc_after "${_adecc_before}")
   string(REGEX REPLACE
      "#[ \t]*include[ \t]*<atl(base|file|coll|comcli|enc)\\.h>"
      "#include \"dxc/Support/Bcc64xNativeAtl.h\""
      _adecc_after "${_adecc_after}")
   string(REGEX REPLACE "#[ \t]*include[ \t]*<Verify\\.h>"
      "#include \"dxc/Test/WEXAdapter.h\"" _adecc_after "${_adecc_after}")
   string(REGEX REPLACE "#[ \t]*include[ \t]*<WexTestClass\\.h>"
      "#include \"dxc/Test/WEXAdapter.h\"" _adecc_after "${_adecc_after}")
   if(NOT _adecc_before STREQUAL _adecc_after)
      file(WRITE "${_adecc_file}" "${_adecc_after}")
      math(EXPR _adecc_count "${_adecc_count}+1")
   endif()
endforeach()

set(_adecc_pix "${SOURCE_ROOT}/tools/clang/unittests/HLSL/PixDiaTest.cpp")
file(READ "${_adecc_pix}" _adecc_pix_original)
string(REGEX REPLACE
   "([A-Za-z_][A-Za-z_0-9]*)->QueryInterface\\(&([A-Za-z_][A-Za-z_0-9]*)\\)"
   "DxcBcc64xQueryInterface(\\1, &\\2)"
   _adecc_pix_new "${_adecc_pix_original}")
if(NOT _adecc_pix_new STREQUAL _adecc_pix_original)
   file(WRITE "${_adecc_pix}" "${_adecc_pix_new}")
endif()

# Preserve the native mutable values in the original frontend regression test.
set(_adecc_lang "${SOURCE_ROOT}/tools/clang/include/clang/Basic/LangOptions.h")
file(READ "${_adecc_lang}" _adecc_lang_text)
set(_adecc_lang_anchor "  bool ObjCARCWeak = false;")
if(_adecc_lang_text MATCHES "bool DelayedTemplateParsing = false;")
elseif(_adecc_lang_text MATCHES "bool ObjCARCWeak = false;")
   string(REPLACE "${_adecc_lang_anchor}"
      "${_adecc_lang_anchor}\n  bool CPlusPlus = true;\n  bool DelayedTemplateParsing = false;"
      _adecc_lang_text "${_adecc_lang_text}")
   file(WRITE "${_adecc_lang}" "${_adecc_lang_text}")
else()
   message(FATAL_ERROR "DXC native frontend LangOptions migration anchor missing")
endif()

# Preserve the test's empty native IStream without an ATL/Shell dependency.
set(_adecc_msfs "${SOURCE_ROOT}/tools/clang/unittests/HLSL/MSFileSysTest.cpp")
file(READ "${_adecc_msfs}" _adecc_msfs_text)
if(_adecc_msfs_text MATCHES "SHCreateMemStream\\(nullptr, 0\\)")
   string(REPLACE
      "*pResult = SHCreateMemStream(nullptr, 0);\n    return (*pResult == nullptr) ? E_OUTOFMEMORY : S_OK;"
      "IStream* pStream = nullptr;\n    HRESULT const hr = ::CreateStreamOnHGlobal(nullptr, TRUE, &pStream);\n    *pResult = pStream;\n    return hr;"
      _adecc_msfs_text "${_adecc_msfs_text}")
   if(_adecc_msfs_text MATCHES "SHCreateMemStream\\(nullptr, 0\\)")
      message(FATAL_ERROR "DXC MSFileSysTest native stream anchor changed")
   endif()
   file(WRITE "${_adecc_msfs}" "${_adecc_msfs_text}")
endif()

# Complete inventory of original native test sources; never skip a target.
set(_adecc_unconverted "")
foreach(_adecc_file IN LISTS _adecc_original_tests)
   file(READ "${_adecc_file}" _adecc_text)
   if(_adecc_text MATCHES "#[ \t]*include[ \t]*[<\"]atl[a-z_0-9]*\\.h[>\"]")
      list(APPEND _adecc_unconverted "${_adecc_file}")
   endif()
endforeach()
if(_adecc_unconverted)
   string(JOIN "\n  " _adecc_report ${_adecc_unconverted})
   message(FATAL_ERROR "DXC ATL-free test migration incomplete:\n  ${_adecc_report}")
endif()
message(STATUS "DXC ATL-free source preflight passed; files normalized=${_adecc_count}")

# Version-pinned source-level removal of MSVC-only declarations.
# These original test sources do not use _com_error, _bstr_t or _variant_t.
function(_adecc_replace_test_source _relative _before _after)
   set(_path "${SOURCE_ROOT}/${_relative}")
   file(READ "${_path}" _contents)
   if(_contents MATCHES "ADECC_NATIVE_SOURCE_MARKER_DO_NOT_MATCH")
      message(FATAL_ERROR "Unexpected DXC native source marker")
   endif()
   string(FIND "${_contents}" "${_before}" _position)
   if(_position EQUAL -1)
      string(FIND "${_contents}" "${_after}" _already)
      if(_already EQUAL -1)
         message(FATAL_ERROR "DXC source migration anchor missing: ${_relative}")
      endif()
      return()
   endif()
   string(REPLACE "${_before}" "${_after}" _updated "${_contents}")
   file(WRITE "${_path}" "${_updated}")
endfunction()

# Neither source uses COM compiler-support classes from comdef.h.
_adecc_replace_test_source("tools/clang/unittests/HLSLHost/HLSLHost.cpp"
   "#include <comdef.h>" "// Native Windows COM ABI: no MSVC comdef.h dependency.")
_adecc_replace_test_source("tools/clang/unittests/dxc_batch/dxc_batch.cpp"
   "#include <comdef.h>" "// Native DXC COM API: no MSVC comdef.h dependency.")

# Preserve QueryInterface's real two-parameter ABI. Smart-pointer overloads
# operate on the wrapper, while raw interface pointers require an explicit IID.
_adecc_replace_test_source("tools/clang/unittests/HLSL/CompilerTest.cpp"
   "pCompiler->QueryInterface(&pCompilerVersion)"
   "pCompiler.QueryInterface(&pCompilerVersion)")
_adecc_replace_test_source("tools/clang/unittests/HLSL/CompilerTest.cpp"
   "pCompiler->QueryInterface(&pCompilerVersion2)"
   "pCompiler.QueryInterface(&pCompilerVersion2)")
_adecc_replace_test_source("tools/clang/unittests/HLSL/RewriterTest.cpp"
   "(*pRewriter)->QueryInterface(&pLangExtensions)"
   "(*pRewriter)->QueryInterface(__uuidof(IDxcLangExtensions), reinterpret_cast<void**>(&pLangExtensions))")

# Explicitly compare native COM identity pointers, not wrapper conversions.
_adecc_replace_test_source("tools/clang/unittests/HLSL/CompilerTest.cpp"
   "VERIFY_ARE_EQUAL(pPdbUtils2_Again, pPdbUtils2);"
   "EXPECT_EQ(pPdbUtils2_Again.Get(), pPdbUtils2.Get());")
_adecc_replace_test_source("tools/clang/unittests/HLSL/CompilerTest.cpp"
   "VERIFY_ARE_EQUAL(pPdbUtils_Again, pPdbUtils);"
   "EXPECT_EQ(pPdbUtils_Again.Get(), pPdbUtils.Get());")

# The upstream hash depended on a private MSVC STL symbol.
_adecc_replace_test_source("tools/clang/unittests/HLSLExec/ShaderOpTest.h"
   "#include <unordered_set>"
   "#include <unordered_set>\n#include <string_view>")
_adecc_replace_test_source("tools/clang/unittests/HLSLExec/ShaderOpTest.h"
   "std::_Hash_array_representation((const unsigned char *)a,\n                                             strlen(a))"
   "std::hash<std::string_view>{}(a)")

# The released D3D12_FEATURE enum has a restricted range. Future preview
# feature values 77 and 78 are valid runtime integral casts, not constexpr.
_adecc_replace_test_source("tools/clang/unittests/HLSLExec/HlslExecTestUtils.h"
   "constexpr D3D12_FEATURE D3D12_FEATURE_LINEAR_ALGEBRA_SUPPORT"
   "inline D3D12_FEATURE const D3D12_FEATURE_LINEAR_ALGEBRA_SUPPORT")
_adecc_replace_test_source("tools/clang/unittests/HLSLExec/HlslExecTestUtils.h"
   "constexpr D3D12_FEATURE D3D12_FEATURE_LINEAR_ALGEBRA_MATRIX_OPERATION_SUPPORT"
   "inline D3D12_FEATURE const D3D12_FEATURE_LINEAR_ALGEBRA_MATRIX_OPERATION_SUPPORT")
