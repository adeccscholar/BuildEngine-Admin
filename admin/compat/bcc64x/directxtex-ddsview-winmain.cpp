// BCC64X GUI entry-point adapter for the original DDSView wWinMain.
// Preserve Unicode command-line parsing and the upstream GUI subsystem.
#include <windows.h>
#include <string>

extern int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPSTR, int iShow) {
   wchar_t const* pCommand = GetCommandLineW();
   if (*pCommand == L'"') {
      ++pCommand;
      while (*pCommand && *pCommand != L'"') {
         ++pCommand;
      }
      if (*pCommand) {
         ++pCommand;
      }
   }
   else {
      while (*pCommand && *pCommand != L' ' && *pCommand != L'\t') {
         ++pCommand;
      }
   }
   while (*pCommand == L' ' || *pCommand == L'\t') {
      ++pCommand;
   }
   std::wstring strArguments(pCommand);
   return wWinMain(hInstance, hPrevious, strArguments.data(), iShow);
}
