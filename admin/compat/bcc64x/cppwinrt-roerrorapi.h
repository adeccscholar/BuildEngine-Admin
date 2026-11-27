// SPDX-License-Identifier: MIT
// Native Windows ABI adapter for C++/WinRT's suppress_error_info test.
// No Visual Studio / Windows SDK roerrorapi.h or import library is required.
// The test continues to execute the actual Windows Runtime API.
#pragma once

#include <windows.h>

inline constexpr UINT32 RO_ERROR_REPORTING_USESETERRORINFO = 0x00000004U;
inline constexpr UINT32 RO_ERROR_REPORTING_SUPPRESSSETERRORINFO = 0x00000008U;

inline HRESULT RoSetErrorReportingFlags(UINT32 const uFlags) {
   HMODULE const hRuntime = LoadLibraryW(L"combase.dll");
   if(hRuntime == nullptr)
      return HRESULT_FROM_WIN32(GetLastError());
   using function_ty = HRESULT (WINAPI*)(UINT32);
   function_ty const pFunction = reinterpret_cast<function_ty>(
      GetProcAddress(hRuntime, "RoSetErrorReportingFlags"));
   if(pFunction == nullptr) {
      DWORD const uError = GetLastError();
      FreeLibrary(hRuntime);
      return HRESULT_FROM_WIN32(uError);
   }
   HRESULT const iResult = pFunction(uFlags);
   FreeLibrary(hRuntime);
   return iResult;
}
