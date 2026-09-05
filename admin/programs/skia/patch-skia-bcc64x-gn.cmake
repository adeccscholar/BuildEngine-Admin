# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
#
# Exact-revision GN adaptation for Skia
# 2eed75b956045eb8603d3690a1e84bc582a2135d.
#
# This file contains only toolchain/dialect adaptations required for the
# direct BCC64X target. The selected Skia feature profile remains in
# build-libraries.xml (args.gn) and is deliberately not hidden here.

cmake_minimum_required(VERSION 3.25)

set(_ADECC_SKIA_REVISION "2eed75b956045eb8603d3690a1e84bc582a2135d")
set(_ADECC_MARKER "adecc-bcc64x-gn-overlay-v8-shared")

if(NOT DEFINED SKIA_ROOT OR SKIA_ROOT STREQUAL "")
   message(FATAL_ERROR "SKIA_ROOT is required")
endif()
if(NOT DEFINED EXPECTED_REVISION OR NOT EXPECTED_REVISION STREQUAL _ADECC_SKIA_REVISION)
   message(FATAL_ERROR "This GN adapter is bound to Skia ${_ADECC_SKIA_REVISION}; got '${EXPECTED_REVISION}'")
endif()

function(adecc_require_file theRelativePath)
   if(NOT EXISTS "${SKIA_ROOT}/${theRelativePath}")
      message(FATAL_ERROR "Skia checkout is missing ${theRelativePath}")
   endif()
endfunction()

function(adecc_replace_once theRelativePath theLabel theOld theNew)
   set(thePath "${SKIA_ROOT}/${theRelativePath}")
   file(READ "${thePath}" theText)

   # Idempotent reruns are allowed. The replacement itself is sufficiently
   # specific for this exact pinned Skia revision.
   string(FIND "${theText}" "${theNew}" theNewPos)
   if(NOT theNewPos EQUAL -1)
      return()
   endif()

   string(FIND "${theText}" "${theOld}" thePos)
   if(thePos EQUAL -1)
      message(FATAL_ERROR "${theLabel}: expected source fragment not found in ${theRelativePath}")
   endif()

   string(LENGTH "${theOld}" theOldLength)
   string(SUBSTRING "${theText}" 0 ${thePos} theBefore)
   math(EXPR theAfterPos "${thePos} + ${theOldLength}")
   string(SUBSTRING "${theText}" ${theAfterPos} -1 theAfter)
   file(WRITE "${thePath}" "${theBefore}${theNew}${theAfter}")
endfunction()

foreach(theFile IN ITEMS
   "gn/BUILDCONFIG.gn"
   "gn/skia/BUILD.gn"
   "gn/toolchain/BUILD.gn"
   "BUILD.gn"
   "modules/skcms/BUILD.gn"
   "src/utils/win/SkDWriteFontFileStream.h"
   "third_party/third_party.gni")
   adecc_require_file("${theFile}")
endforeach()

# -----------------------------------------------------------------------------
# gn/BUILDCONFIG.gn: select Skia's GCC-like target toolchain for BCC64X while
# keeping the native Windows/MSVC host toolchain for Skia's host tools.
# -----------------------------------------------------------------------------
adecc_replace_once(
   "gn/BUILDCONFIG.gn"
   "declare skia_use_bcc64x"
   [=[# Default configs
]=]
   [=[# adecc-bcc64x-gn-overlay-v8-shared
declare_args() {
  skia_use_bcc64x = false
}

# Default configs
]=])

adecc_replace_once(
   "gn/BUILDCONFIG.gn"
   "BCC64X target toolchain selection"
   [=[if (is_win) {
  # Windows tool chain
  set_default_toolchain("//gn/toolchain:msvc")
  default_toolchain_name = "msvc"
  host_toolchain = "msvc_host"
} else if (is_wasm) {
]=]
   [=[if (is_win && skia_use_bcc64x) {
  # adecc: BCC64X is the Windows target compiler. Host tools remain on
  # Skia's native Windows host toolchain.
  set_default_toolchain("//gn/toolchain:gcc_like")
  default_toolchain_name = "gcc_like"
  host_toolchain = "msvc_host"
} else if (is_win) {
  # Windows tool chain
  set_default_toolchain("//gn/toolchain:msvc")
  default_toolchain_name = "msvc"
  host_toolchain = "msvc_host"
} else if (is_wasm) {
]=])

# -----------------------------------------------------------------------------
# gn/skia/BUILD.gn: compiler-dialect adaptations. Windows source selection is
# preserved; only cl.exe/clang-cl-specific flags are bypassed for BCC64X.
# -----------------------------------------------------------------------------
adecc_replace_once(
   "gn/skia/BUILD.gn"
   "target toolchain discriminator"
   [=[import("../skia.gni")
]=]
   [=[import("../skia.gni")

# adecc-bcc64x-gn-overlay-v8-shared
is_bcc64x_toolchain = skia_use_bcc64x && current_toolchain == default_toolchain
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "default warning dialect"
   [=[  if (is_win && !is_clang) {
    cflags += [
]=]
   [=[  if (is_win && !is_clang && !is_bcc64x_toolchain) {
    cflags += [
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "default Windows compiler flags"
   [=[  if (is_win) {
    if (is_clang && current_cpu == "arm64") {
]=]
   [=[  if (is_win && !is_bcc64x_toolchain) {
    if (is_clang && current_cpu == "arm64") {
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "no exceptions dialect"
   [=[config("no_exceptions") {
  # Exceptions are disabled by default on Windows.  (Use /EHsc to enable them.)
  if (!is_win) {
]=]
   [=[config("no_exceptions") {
  # Exceptions are disabled by default under cl.exe; BCC64X uses the
  # Clang/GCC-style target driver and therefore needs the explicit flag.
  if (!is_win || is_bcc64x_toolchain) {
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "werror Windows dialect"
   [=[    if (is_win) {
      cflags += [ "/WX" ]
]=]
   [=[    if (is_win && !(is_bcc64x_toolchain && is_debug)) {
      cflags += [ "/WX" ]
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "Debug warnings Windows dialect"
   [=[  }

  if (is_win) {
    cflags += [
      "/W3",  # Turn on lots of warnings.
]=]
   [=[  }

  if (is_win && !(is_bcc64x_toolchain && is_debug)) {
    cflags += [
      "/W3",  # Turn on lots of warnings.
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "debug symbols Windows dialect"
   [=[  } else if (is_win) {
    cflags = [ "/Z7" ]
    if (is_clang) {
]=]
   [=[  } else if (is_win && is_bcc64x_toolchain && is_debug) {
    # adecc: BCC64X uses the Clang-style driver; do not pass cl.exe /Z7 or
    # /DEBUG:* switches through the gcc_like target toolchain.
    cflags = [ "-g" ]
  } else if (is_win) {
    cflags = [ "/Z7" ]
    if (is_clang) {
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "RTTI dialect"
   [=[    if (is_win) {
      cflags_cc = [ "/GR-" ]
    } else {
]=]
   [=[    if (is_win && !is_bcc64x_toolchain) {
      cflags_cc = [ "/GR-" ]
    } else {
]=])

adecc_replace_once(
   "gn/skia/BUILD.gn"
   "optimization dialect"
   [=[config("optimize") {
  ldflags = []
  if (is_win) {
]=]
   [=[config("optimize") {
  ldflags = []
  if (is_win && !is_bcc64x_toolchain) {
]=])

# -----------------------------------------------------------------------------
# CPU feature flags. BCC64X accepts the Clang/GCC -m... dialect, not MSVC
# /arch:... nor clang-cl /clang: forwarding syntax.
# -----------------------------------------------------------------------------
adecc_replace_once(
   "BUILD.gn"
   "ml3 SIMD dialect"
   [=[opts("ml3") {
  enabled = is_x86
  sources = skia_opts.ml3_sources
  if (is_win) {
    cflags = [ "/arch:AVX2" ]
]=]
   [=[opts("ml3") {
  enabled = is_x86
  sources = skia_opts.ml3_sources
  if (is_win && !skia_use_bcc64x) {
    cflags = [ "/arch:AVX2" ]
]=])

adecc_replace_once(
   "BUILD.gn"
   "ml4 SIMD dialect"
   [=[opts("ml4") {
  enabled = is_x86
  sources = skia_opts.ml4_sources
  if (is_win) {
    cflags = [ "/arch:AVX512" ]
]=]
   [=[opts("ml4") {
  enabled = is_x86
  sources = skia_opts.ml4_sources
  if (is_win && !skia_use_bcc64x) {
    cflags = [ "/arch:AVX512" ]
]=])

adecc_replace_once(
   "modules/skcms/BUILD.gn"
   "skcms HSW SIMD dialect"
   [=[arch("skcms_TransformHsw") {
  enabled = current_cpu == "x64" && target_os != "android"
  sources = skcms_TransformHsw
  if (is_win) {
    if (is_clang) {
]=]
   [=[arch("skcms_TransformHsw") {
  enabled = current_cpu == "x64" && target_os != "android"
  sources = skcms_TransformHsw
  if (is_win) {
    if (skia_use_bcc64x) {
      cflags = [
        "-mavx2",
        "-mf16c",
        "-ffp-contract=off",
      ]
    } else if (is_clang) {
]=])

adecc_replace_once(
   "modules/skcms/BUILD.gn"
   "skcms SKX SIMD dialect"
   [=[arch("skcms_TransformSkx") {
  enabled = current_cpu == "x64" && target_os != "android"
  sources = skcms_TransformSkx
  if (is_win) {
    if (is_clang) {
]=]
   [=[arch("skcms_TransformSkx") {
  enabled = current_cpu == "x64" && target_os != "android"
  sources = skcms_TransformSkx
  if (is_win) {
    if (skia_use_bcc64x) {
      cflags = [
        "-mavx512f",
        "-mavx512dq",
        "-mavx512cd",
        "-mavx512bw",
        "-mavx512vl",
        "-ffp-contract=off",
      ]
    } else if (is_clang) {
]=])

# DirectWrite's Interlocked* API operates on LONG storage.
adecc_replace_once(
   "src/utils/win/SkDWriteFontFileStream.h"
   "DirectWrite refcount storage"
   [=[    ULONG fRefCount;
]=]
   [=[    LONG fRefCount;
]=])

# Third-party warning suppression must use the target driver's dialect.
adecc_replace_once(
   "third_party/third_party.gni"
   "third-party warning suppression dialect"
   [=[      if (is_win) {
        cflags += [ "/w" ]
      } else {
        cflags += [ "-w" ]
      }
]=]
   [=[      if (is_win && skia_use_bcc64x && current_toolchain == default_toolchain) {
        cflags += [ "-w" ]
      } else if (is_win) {
        cflags += [ "/w" ]
      } else {
        cflags += [ "-w" ]
      }
]=])

# -----------------------------------------------------------------------------
# Shared-library rule for the BCC64X Windows target. The normal gcc_like
# solink rule remains untouched for every other target/host configuration.
# -----------------------------------------------------------------------------
adecc_replace_once(
   "gn/toolchain/BUILD.gn"
   "BCC64X Windows shared-library rule"
   [=[    tool("solink") {
      soname = "{{target_output_name}}{{output_extension}}"

      rpath = "-Wl,-soname,$soname"
      if (current_os == "mac" || current_os == "ios" || current_os == "tvos") {
        rpath = "-Wl,-install_name,@rpath/$soname"
      }

      rspfile = "{{output}}.rsp"
      rspfile_content = "{{inputs}}"

      # --start-group/--end-group let us link multiple .a {{inputs}}
      # without worrying about their relative order on the link line.
      #
      # This is mostly important for traditional linkers like GNU ld and Gold.
      # The Mac/iOS linker neither needs nor accepts these flags.
      # LLD doesn't need these flags, but accepts and ignores them.
      _start_group = "-Wl,--start-group"
      _end_group = "-Wl,--end-group"
      if (current_os == "mac" || current_os == "ios" || current_os == "tvos") {
        _start_group = ""
        _end_group = ""
      }

      _emcc_flags = ""
      if (current_os == "wasm") {
        _emcc_flags = "-sSIDE_MODULE=1"
      }

      command = "$link -shared {{ldflags}} $_start_group @$rspfile {{frameworks}} {{solibs}} $_end_group {{libs}} $rpath $_emcc_flags -o {{output}}"
      outputs = [ "{{root_out_dir}}/$soname" ]
      output_prefix = "lib"
      if (current_os == "mac" || current_os == "ios" || current_os == "tvos") {
        default_output_extension = ".dylib"
      } else if (current_os == "wasm" && !is_canvaskit) {
        default_output_extension = ".wasm.so"
      } else {
        default_output_extension = ".so"
      }
      description = "link {{output}}"
      if (0 <= link_pool_depth) {
        pool = ":link_pool($default_toolchain)"
      }
    }
]=]
   [=[    tool("solink") {
      if (current_os == "win" && skia_use_bcc64x &&
          current_toolchain == default_toolchain) {
        # adecc-bcc64x-gn-overlay-v8-shared
        # BCC64X Win64 Modern DLL plus COFF import library.
        dllname = "{{root_out_dir}}/{{target_output_name}}{{output_extension}}"
        libname = "{{root_out_dir}}/{{target_output_name}}.lib"
        rspfile = "${dllname}.rsp"
        command = "$link --rsp-quoting=windows -tD -o$dllname @$rspfile -Wl,--out-implib,$libname"
        rspfile_content = "{{inputs}} {{libs}} {{solibs}} {{ldflags}}"
        outputs = [
          dllname,
          libname,
        ]
        default_output_extension = ".dll"
        default_output_dir = "{{root_out_dir}}"
        link_output = libname
        depend_output = libname
        runtime_outputs = [ dllname ]
        restat = true
        description = "link {{output}}"
        if (0 <= link_pool_depth) {
          pool = ":link_pool($default_toolchain)"
        }
      } else {
        soname = "{{target_output_name}}{{output_extension}}"

        rpath = "-Wl,-soname,$soname"
        if (current_os == "mac" || current_os == "ios" || current_os == "tvos") {
          rpath = "-Wl,-install_name,@rpath/$soname"
        }

        rspfile = "{{output}}.rsp"
        rspfile_content = "{{inputs}}"

        # --start-group/--end-group let us link multiple .a {{inputs}}
        # without worrying about their relative order on the link line.
        #
        # This is mostly important for traditional linkers like GNU ld and Gold.
        # The Mac/iOS linker neither needs nor accepts these flags.
        # LLD doesn't need these flags, but accepts and ignores them.
        _start_group = "-Wl,--start-group"
        _end_group = "-Wl,--end-group"
        if (current_os == "mac" || current_os == "ios" || current_os == "tvos") {
          _start_group = ""
          _end_group = ""
        }

        _emcc_flags = ""
        if (current_os == "wasm") {
          _emcc_flags = "-sSIDE_MODULE=1"
        }

        command = "$link -shared {{ldflags}} $_start_group @$rspfile {{frameworks}} {{solibs}} $_end_group {{libs}} $rpath $_emcc_flags -o {{output}}"
        outputs = [ "{{root_out_dir}}/$soname" ]
        output_prefix = "lib"
        if (current_os == "mac" || current_os == "ios" || current_os == "tvos") {
          default_output_extension = ".dylib"
        } else if (current_os == "wasm" && !is_canvaskit) {
          default_output_extension = ".wasm.so"
        } else {
          default_output_extension = ".so"
        }
        description = "link {{output}}"
        if (0 <= link_pool_depth) {
          pool = ":link_pool($default_toolchain)"
        }
      }
    }
]=])

message(STATUS "Skia BCC64X GN adapter applied for ${_ADECC_SKIA_REVISION}")
