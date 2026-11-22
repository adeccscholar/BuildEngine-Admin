# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
#
# DirectX-Headers upstream D3DX12 state-object helpers require <wrl/client.h>.
# Unlike the Embarcadero Windows SDK/RTL paths, WRL is supplied by the
# independently registered Windows SDK WinRT include directory.
# This is strictly a test-target include contract, not another compiler SDK.

function(adecc_directx_headers_winrt_include)
   if(NOT DEFINED ENV{CB_WINDOWS_RC} OR "$ENV{CB_WINDOWS_RC}" STREQUAL "")
      message(FATAL_ERROR
         "DirectX-Headers upstream tests require the registered Windows SDK RC tool")
   endif()

   get_filename_component(_adecc_rc_bin "$ENV{CB_WINDOWS_RC}" DIRECTORY)
   get_filename_component(_adecc_rc_version_dir "${_adecc_rc_bin}" DIRECTORY)
   get_filename_component(_adecc_rc_version "${_adecc_rc_version_dir}" NAME)
   get_filename_component(_adecc_rc_bin_root "${_adecc_rc_version_dir}" DIRECTORY)
   get_filename_component(_adecc_sdk_root "${_adecc_rc_bin_root}" DIRECTORY)
   set(_adecc_sdk_winrt "${_adecc_sdk_root}/Include/${_adecc_rc_version}/winrt")

   if(NOT EXISTS "${_adecc_sdk_winrt}/wrl/client.h")
      message(FATAL_ERROR
         "Registered Windows SDK lacks the WRL header required by upstream "
         "DirectX-Headers tests: ${_adecc_sdk_winrt}/wrl/client.h")
   endif()

   foreach(_adecc_target IN LISTS ARGN)
      if(NOT TARGET "${_adecc_target}")
         message(FATAL_ERROR "DirectX-Headers upstream test target missing: ${_adecc_target}")
      endif()
      target_include_directories("${_adecc_target}" SYSTEM PRIVATE "${_adecc_sdk_winrt}")
   endforeach()
endfunction()
