// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: LicenseRef-PolyForm-Noncommercial-1.0.0
//
// BCC64X Clang console entry point for upstream libdeflate test programs.
// Upstream Windows code declares wmain(int, wchar_t**); do not modify it.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

int wmain(int argc, wchar_t** argv);

int main(void) {
   int argc = 0;
   wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
   if(argv == NULL) {
      return 1;
   }

   int const result = wmain(argc, argv);
   LocalFree(argv);
   return result;
}
