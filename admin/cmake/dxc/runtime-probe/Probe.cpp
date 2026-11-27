#include <windows.h>
#include <cstdio>

int main(int iArgc, char** pszArgv) {
   if(iArgc != 2) {
      std::fprintf(stderr, "DXC_RUNTIME|ERROR|expected dll path\n");
      return 2;
   }
   char const* szDll = pszArgv[1];
   if(GetFileAttributesA(szDll) == INVALID_FILE_ATTRIBUTES) {
      std::fprintf(stderr, "DXC_RUNTIME|ERROR|missing DLL %s, error %lu\n", szDll, (unsigned long)GetLastError());
      return 3;
   }
   SetLastError(ERROR_SUCCESS);
   HMODULE hModule = LoadLibraryExA(szDll, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
   if(hModule == nullptr) {
      DWORD dwError = GetLastError();
      char szMessage[2048]{};
      FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         nullptr, dwError, 0, szMessage, sizeof(szMessage), nullptr);
      std::fprintf(stderr, "DXC_RUNTIME|ERROR|LoadLibraryExA error=%lu (0x%08lX) message=%s\n",
         (unsigned long)dwError, (unsigned long)dwError, szMessage);
      return 4;
   }
   char szPath[32768]{};
   GetModuleFileNameA(hModule, szPath, sizeof(szPath));
   std::printf("DXC_RUNTIME|LOADED|%s\n", szPath);
   FARPROC pfnCreate = GetProcAddress(hModule, "DxcCreateInstance");
   if(pfnCreate == nullptr) {
      std::fprintf(stderr, "DXC_RUNTIME|ERROR|DxcCreateInstance export missing, error=%lu\n", (unsigned long)GetLastError());
      FreeLibrary(hModule);
      return 5;
   }
   std::puts("DXC_RUNTIME|RESULT|PASS");
   FreeLibrary(hModule);
   return 0;
}
