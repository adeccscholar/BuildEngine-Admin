// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: LicenseRef-PolyForm-Noncommercial-1.0.0
//
// BCC64X console entry-point adapter for the OpenAL Soft openal-info utility.
// Upstream win_main_utf8.h supplies wmain(int, wchar_t**) and converts the
// Unicode arguments into UTF-8 for the original utility implementation.
// The adapter preserves that upstream path without modifying audio features.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

int wmain(int argc, wchar_t** argv);

int main(void) {
   int iArgc = 0;
   wchar_t** pszArgv = CommandLineToArgvW(GetCommandLineW(), &iArgc);
   if(pszArgv == NULL) {
      return 1;
   }

   int const iResult = wmain(iArgc, pszArgv);
   LocalFree(pszArgv);
   return iResult;
}
