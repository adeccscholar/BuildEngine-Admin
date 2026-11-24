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
