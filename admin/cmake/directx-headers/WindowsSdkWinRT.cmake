# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
#
# BCC64X cannot consume the MSVC-gated WRL implementation from the Windows
# SDK. DirectX-Headers v1.619.1 carries Microsoft's MIT-licensed ComPtr
# implementation in include/wsl/wrladapter.h and a <wrl/client.h> forwarder
# in include/wsl/stubs/wrl/client.h.
#
# Only that single forwarder is staged in an isolated include root. Do NOT
# add include/wsl/stubs wholesale on native Windows: the other WSL stubs
# substitute Win32 types with different ABIs. winadapter.h itself includes
# the native <unknwn.h>; the Windows SDK continues to supply COM interfaces.
#
# The function name remains compatible with the registered build contract.

function(adecc_directx_headers_winrt_include)
   if(NOT WIN32)
      message(FATAL_ERROR "This BuildEngine WRL adapter is for native Windows only")
   endif()

   set(_adecc_upstream_root "${CMAKE_SOURCE_DIR}/include")
   set(_adecc_forwarder
      "${_adecc_upstream_root}/wsl/stubs/wrl/client.h")
   set(_adecc_adapter
      "${_adecc_upstream_root}/wsl/wrladapter.h")
   set(_adecc_winadapter
      "${_adecc_upstream_root}/wsl/winadapter.h")

   foreach(_adecc_source IN ITEMS
      "${_adecc_forwarder}" "${_adecc_adapter}" "${_adecc_winadapter}")
      if(NOT EXISTS "${_adecc_source}")
         message(FATAL_ERROR
            "Pinned Microsoft DirectX-Headers WRL source is missing: ${_adecc_source}")
      endif()
   endforeach()

   set(_adecc_override_root
      "${CMAKE_CURRENT_BINARY_DIR}/adecc-microsoft-wrl-compat")
   file(MAKE_DIRECTORY "${_adecc_override_root}/wrl")
   configure_file(
      "${_adecc_forwarder}"
      "${_adecc_override_root}/wrl/client.h"
      COPYONLY)

   # Upstream installs include/wsl already; install the missing wrl/client.h\n   # forwarder into the versioned package for independent consumers too.\n   get_property(_adecc_install_registered GLOBAL PROPERTY\n      ADECC_DIRECTX_HEADERS_WRL_INSTALL_REGISTERED)\n   if(NOT _adecc_install_registered)\n      install(FILES "${_adecc_forwarder}" DESTINATION include/wrl)\n      set_property(GLOBAL PROPERTY\n         ADECC_DIRECTX_HEADERS_WRL_INSTALL_REGISTERED TRUE)\n   endif()\n\n   foreach(_adecc_target IN LISTS ARGN)
      if(NOT TARGET "${_adecc_target}")
         message(FATAL_ERROR
            "DirectX-Headers upstream test target missing: ${_adecc_target}")
      endif()
      target_include_directories("${_adecc_target}" BEFORE PRIVATE
         "${_adecc_override_root}"
         "${_adecc_upstream_root}")
      message(STATUS
         "DirectX-Headers ${_adecc_target}: pinned MIT WRL adapter; "
         "native Windows COM types; include root=${_adecc_override_root}")
   endforeach()
endfunction()
