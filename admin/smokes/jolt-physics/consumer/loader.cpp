// SPDX-License-Identifier: MIT
// Jolt runtime preflight. This executable deliberately does not link Jolt.
// It can report DLL loader errors that happen before the actual consumer main.
#include <windows.h>
#include <winnt.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace {
   void PrintWindowsError(char const* szOperation, DWORD const uCode) {
      LPSTR szMessage = nullptr;
      DWORD const uLength = FormatMessageA(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
         FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, uCode, 0,
         reinterpret_cast<LPSTR>(&szMessage), 0, nullptr);
      std::fprintf(stderr, "SMOKE|INFO|loader|%s: Win32=%lu (0x%08lX): %.*s\n",
         szOperation, static_cast<unsigned long>(uCode),
         static_cast<unsigned long>(uCode),
         static_cast<int>(uLength), szMessage ? szMessage : "");
      if(szMessage) LocalFree(szMessage);
   }

   // Translate a PE import-table RVA into its location in the raw file.
   BYTE const* RvaToFile(BYTE const* pData, std::size_t const uSize,
      IMAGE_NT_HEADERS64 const* pNt, DWORD const uRva, std::size_t const uNeed) {
      auto const* pSections = IMAGE_FIRST_SECTION(pNt);
      for(WORD i = 0; i < pNt->FileHeader.NumberOfSections; ++i) {
         DWORD const uStart = pSections[i].VirtualAddress;
         DWORD const uSpan = pSections[i].Misc.VirtualSize > pSections[i].SizeOfRawData
            ? pSections[i].Misc.VirtualSize : pSections[i].SizeOfRawData;
         if(uRva < uStart || static_cast<std::uint64_t>(uRva) + uNeed >
            static_cast<std::uint64_t>(uStart) + uSpan)
            continue;
         std::uint64_t const uOffset =
            static_cast<std::uint64_t>(pSections[i].PointerToRawData) +
            (uRva - uStart);
         if(uOffset + uNeed <= uSize)
            return pData + uOffset;
      }
      return nullptr;
   }

   void ShowImports(std::filesystem::path const& aPath) {
      FILE* pFile = nullptr;
      if(_wfopen_s(&pFile, aPath.c_str(), L"rb") != 0 || !pFile) {
         std::fprintf(stderr, "SMOKE|INFO|imports|Cannot inspect Jolt DLL\n");
         return;
      }
      if(_fseeki64(pFile, 0, SEEK_END) != 0) {
         std::fclose(pFile);
         return;
      }
      auto const iEnd = _ftelli64(pFile);
      if(iEnd <= 0 || iEnd > 128 * 1024 * 1024) {
         std::fclose(pFile);
         return;
      }
      std::vector<BYTE> vecData(static_cast<std::size_t>(iEnd));
      std::rewind(pFile);
      bool const bRead = std::fread(vecData.data(), 1, vecData.size(), pFile) ==
         vecData.size();
      std::fclose(pFile);
      if(!bRead || vecData.size() < sizeof(IMAGE_DOS_HEADER))
         return;
      auto const* pBase = vecData.data();
      auto const* pDos = reinterpret_cast<IMAGE_DOS_HEADER const*>(pBase);
      if(pDos->e_magic != IMAGE_DOS_SIGNATURE || pDos->e_lfanew < 0 ||
         static_cast<std::uint64_t>(pDos->e_lfanew) +
            sizeof(IMAGE_NT_HEADERS64) > vecData.size())
         return;
      auto const* pNt = reinterpret_cast<IMAGE_NT_HEADERS64 const*>(
         pBase + pDos->e_lfanew);
      if(pNt->Signature != IMAGE_NT_SIGNATURE ||
         pNt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
         pNt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_IMPORT)
         return;
      DWORD const uRva = pNt->OptionalHeader
         .DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
      if(!uRva) return;
      for(unsigned i = 0; i < 128; ++i) {
         auto const* pImport = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR const*>(
            RvaToFile(pBase, vecData.size(), pNt,
               uRva + i * sizeof(IMAGE_IMPORT_DESCRIPTOR),
               sizeof(IMAGE_IMPORT_DESCRIPTOR)));
         if(!pImport || !pImport->Name) break;
         char const* szName = reinterpret_cast<char const*>(
            RvaToFile(pBase, vecData.size(), pNt, pImport->Name, 1));
         if(!szName) continue;
         std::size_t const uMax = vecData.size() -
            static_cast<std::size_t>(reinterpret_cast<BYTE const*>(szName) - pBase);
         std::size_t const uLength = strnlen_s(szName, uMax);
         if(uLength == uMax || uLength >= 260) continue;
         std::fprintf(stderr, "SMOKE|INFO|import|%.*s\n",
            static_cast<int>(uLength), szName);
         // Diagnose each direct import only after the primary LoadLibrary failed.
         if(HMODULE const hModule = LoadLibraryA(szName)) {
            std::array<char, 32768> arrResolved {};
            DWORD const uChars = GetModuleFileNameA(hModule,
               arrResolved.data(), static_cast<DWORD>(arrResolved.size()));
            std::fprintf(stderr, "SMOKE|INFO|import-resolved|%.*s => %.*s\n",
               static_cast<int>(uLength), szName, static_cast<int>(uChars),
               arrResolved.data());
            FreeLibrary(hModule);
         }
         else {
            PrintWindowsError(szName, GetLastError());
         }
      }
   }
}

int main() {
   std::array<wchar_t, 32768> arrDirectory {};
   DWORD const uLength = GetEnvironmentVariableW(L"JOLT_SMOKE_DLL_DIR",
      arrDirectory.data(), static_cast<DWORD>(arrDirectory.size()));
   if(uLength == 0 || uLength >= arrDirectory.size()) {
      std::fputs("SMOKE|CHECK|jolt-loader|FAIL|JOLT_SMOKE_DLL_DIR missing\n", stderr);
      std::fputs("SMOKE|RESULT|FAIL|Jolt runtime environment\n", stderr);
      return 1;
   }
   std::filesystem::path const aDll =
      std::filesystem::path(arrDirectory.data()) / L"libJolt.dll";
   std::fprintf(stderr, "SMOKE|INFO|jolt-dll|%ls\n", aDll.c_str());
   if(!std::filesystem::exists(aDll)) {
      std::fputs("SMOKE|CHECK|jolt-loader|FAIL|Expected DLL not found\n", stderr);
      std::fputs("SMOKE|RESULT|FAIL|Jolt DLL missing\n", stderr);
      return 1;
   }
   HMODULE const hJolt = LoadLibraryW(aDll.c_str());
   if(!hJolt) {
      PrintWindowsError("LoadLibraryW(libJolt.dll)", GetLastError());
      ShowImports(aDll);
      std::fputs("SMOKE|CHECK|jolt-loader|FAIL|DLL loading or initialization\n", stderr);
      std::fputs("SMOKE|RESULT|FAIL|Jolt DLL loader\n", stderr);
      return 1;
   }
   std::puts("SMOKE|CHECK|jolt-loader|PASS|Installed Jolt DLL initialized");
   FreeLibrary(hJolt);
   std::array<wchar_t, 32768> arrExecutable {};
   DWORD const uExeLength = GetModuleFileNameW(nullptr, arrExecutable.data(),
      static_cast<DWORD>(arrExecutable.size()));
   if(uExeLength == 0 || uExeLength >= arrExecutable.size()) {
      PrintWindowsError("GetModuleFileNameW", GetLastError());
      return 1;
   }
   std::filesystem::path aConsumer(arrExecutable.data());
   aConsumer.replace_filename(L"jolt-consumer-real.exe");
   std::wstring strCommand = L"\"" + aConsumer.wstring() + L"\"";
   std::vector<wchar_t> vecCommand(strCommand.begin(), strCommand.end());
   vecCommand.push_back(L'\0');
   STARTUPINFOW aStartup {};
   aStartup.cb = sizeof(aStartup);
   PROCESS_INFORMATION aProcess {};
   if(!CreateProcessW(aConsumer.c_str(), vecCommand.data(), nullptr, nullptr,
      TRUE, 0, nullptr, nullptr, &aStartup, &aProcess)) {
      PrintWindowsError("CreateProcessW(jolt-consumer-real.exe)", GetLastError());
      std::fputs("SMOKE|RESULT|FAIL|Jolt consumer process startup\n", stderr);
      return 1;
   }
   WaitForSingleObject(aProcess.hProcess, INFINITE);
   DWORD uExit = 1;
   GetExitCodeProcess(aProcess.hProcess, &uExit);
   CloseHandle(aProcess.hThread);
   CloseHandle(aProcess.hProcess);
   if(uExit != 0)
      std::fprintf(stderr, "SMOKE|INFO|child-exit|0x%08lX\n",
         static_cast<unsigned long>(uExit));
   return static_cast<int>(uExit);
}
