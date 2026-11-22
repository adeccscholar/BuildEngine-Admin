// Native Windows wide-command-line adapter for the original DirectXTex tools.
// The upstream wmain implementation, Unicode arguments and all tool features
// remain unchanged. The BCC64X console startup links against main instead.
#include <windows.h>
#include <shellapi.h>

extern int __cdecl wmain(int, wchar_t*[]);

int main() {
   int iArgc = 0;
   wchar_t** pArgs = CommandLineToArgvW(GetCommandLineW(), &iArgc);
   if (!pArgs) {
      return 1;
   }
   int const iResult = wmain(iArgc, pArgs);
   LocalFree(pArgs);
   return iResult;
}
