# SPDX-License-Identifier: MIT
# BCC64X-only adaptation of Chromium's pinned GN Visual Studio discovery.
# This source preflight must fail on an unexpected upstream GN contract.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT IS_DIRECTORY "${SOURCE_ROOT}/build/config/win")
   message(FATAL_ERROR "PDFium Chromium GN source root is missing")
endif()
set(_gn_file "${SOURCE_ROOT}/build/config/win/visual_studio_version.gni")
if(NOT EXISTS "${_gn_file}")
   message(FATAL_ERROR "Pinned Chromium visual_studio_version.gni is missing: ${_gn_file}")
endif()
file(READ "${_gn_file}" _text)
# The upstream win_toolchain_data.gni imports setup_toolchain.py even for a
# custom toolchain. Under native BCC64X this would implicitly require MSVC.
# Provide inert GN environment scopes only for our explicitly selected toolchain.
set(_data_file "${SOURCE_ROOT}/build/toolchain/win/win_toolchain_data.gni")
if(NOT EXISTS "${_data_file}")
   message(FATAL_ERROR "Pinned Chromium win_toolchain_data.gni is missing: ${_data_file}")
endif()
file(READ "${_data_file}" _data)
set(_data_marker "# BCC64X uses a native toolchain; Chromium MSVC environment discovery is irrelevant.")
string(FIND "${_data}" "${_data_marker}" _data_existing)
if(_data_existing EQUAL -1)
   # Only replace the top-level GN declare_args block containing MSVC setup.
   # Locate its matching brace structurally rather than relying on whitespace
   # around the following current_cpu branch.
   string(FIND "${_data}" "declare_args() {" _data_begin)
   string(FIND "${_data}" "setup_toolchain.py" _setup_pos)
   if(_data_begin EQUAL -1 OR _setup_pos EQUAL -1)
      message(FATAL_ERROR "Pinned Chromium GN toolchain declaration not recognized")
   endif()
   string(LENGTH "${_data}" _data_size)
   math(EXPR _scan "${_data_begin} + 14")
   set(_depth 1)
   set(_data_end -1)
   while(_scan LESS _data_size)
      string(SUBSTRING "${_data}" ${_scan} 1 _character)
      if(_character STREQUAL "{")
         math(EXPR _depth "${_depth} + 1")
      elseif(_character STREQUAL "}")
         math(EXPR _depth "${_depth} - 1")
         if(_depth EQUAL 0)
            set(_data_end ${_scan})
            break()
         endif()
      endif()
      math(EXPR _scan "${_scan} + 1")
   endwhile()
   if(_data_end EQUAL -1 OR _setup_pos GREATER _data_end)
      message(FATAL_ERROR "Pinned Chromium GN toolchain declaration is incomplete")
   endif()
   string(SUBSTRING "${_data}" 0 ${_data_begin} _data_prefix)
   math(EXPR _after "${_data_end} + 1")
   string(SUBSTRING "${_data}" ${_after} -1 _data_suffix)
   set(_data_guard [=[
if (defined(custom_toolchain) && custom_toolchain == "//build/toolchain/bcc64x:bcc64x") {
  # BCC64X uses a native toolchain; Chromium MSVC environment discovery is irrelevant.
  win_toolchain_data_x86 = {
    vc_bin_dir = ""
    include_flags_I = ""
    include_flags_I_list = []
    include_flags_imsvc = ""
    include_flags_imsvc_list = []
    paths = ""
    libpath_flags = ""
    libpath_flags_list = []
    libpath_lldlink_flags = ""
    libpath_lldlink_flags_list = []
    msvc_dir = ""
    windows_kits_dir = ""
  }
  win_toolchain_data_x64 = win_toolchain_data_x86
  win_toolchain_data_arm64 = win_toolchain_data_x86
} else {
]=])
   string(SUBSTRING "${_data}" ${_data_begin} ${_after} _unused)
   math(EXPR _length "${_after} - ${_data_begin}")
   string(SUBSTRING "${_data}" ${_data_begin} ${_length} _declarations)
   set(_data "${_data_prefix}${_data_guard}${_declarations}\n}\n${_data_suffix}")
   file(WRITE "${_data_file}" "${_data}")
else()
   string(FIND "${_data}" "win_toolchain_data_x64 = win_toolchain_data_x86" _guard_complete)
   if(_guard_complete EQUAL -1)
      message(FATAL_ERROR "Pinned Chromium GN BCC64X toolchain-data guard diverged")
   endif()
endif()

set(_marker "# BuildEngine: BCC64X uses its registered compiler, not Visual Studio.")
set(_original "if (visual_studio_path == \"\") {")
set(_replacement
   "if (defined(custom_toolchain) && custom_toolchain == \"//build/toolchain/bcc64x:bcc64x\") {\n  ${_marker}\n  visual_studio_runtime_dirs = \"\"\n} else if (visual_studio_path == \"\") {")
string(FIND "${_text}" "${_marker}" _existing)
if(NOT _existing EQUAL -1)
   string(FIND "${_text}" "${_replacement}" _complete)
   if(_complete EQUAL -1)
      message(FATAL_ERROR "PDFium GN BCC64X VS-discovery guard has diverged")
   endif()
   return()
endif()
string(FIND "${_text}" "${_original}" _first)
if(_first EQUAL -1)
   message(FATAL_ERROR "PDFium GN VS-discovery anchor missing; pinned Chromium source has changed")
endif()
string(SUBSTRING "${_text}" 0 ${_first} _prefix)
math(EXPR _next "${_first} + 1")
string(SUBSTRING "${_text}" ${_next} -1 _suffix)
string(FIND "${_suffix}" "${_original}" _duplicate)
if(NOT _duplicate EQUAL -1)
   message(FATAL_ERROR "PDFium GN VS-discovery anchor is ambiguous")
endif()
string(REPLACE "${_original}" "${_replacement}" _patched "${_text}")
if(_patched STREQUAL _text)
   message(FATAL_ERROR "PDFium GN VS-discovery patch had no effect")
endif()
file(WRITE "${_gn_file}" "${_patched}")
message(STATUS "PDFium: native BCC64X GN toolchain bypasses Chromium Visual Studio discovery")
