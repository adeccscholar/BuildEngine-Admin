// BCC64X console startup bridge for the original DXC dxbc2dxil wmain.
// Keep Unicode argument handling and upstream command-line behavior intact.
#include <windows.h>
#include <shellapi.h>

extern int __cdecl wmain(int, wchar_t*[]);

int main() {
   int iArgc = 0;
   wchar_t** pArgs = CommandLineToArgvW(GetCommandLineW(), &iArgc);
   if(!pArgs)
      return 1;
   int const iResult = wmain(iArgc,pArgs);
   LocalFree(pArgs);
   return iResult;
}
