// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Run as a separate process: it must NOT import the DLL being probed.
// The DLL name is explicitly supplied by the smoke XML environment.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <string>

namespace {

void SystemError(DWORD code) {
   wchar_t buffer[1024] {};
   DWORD const count = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, code, 0, buffer, static_cast<DWORD>(std::size(buffer)), nullptr);
   if(count != 0U) std::wprintf(L"[LOADER] Windows message: %ls\n", buffer);
}

struct MappedFile {
   HANDLE file { INVALID_HANDLE_VALUE };
   HANDLE mapping { nullptr };
   BYTE const* data { nullptr };
   std::size_t length { 0U };

   explicit MappedFile(std::wstring const& name) {
      file = ::CreateFileW(name.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
      if(file == INVALID_HANDLE_VALUE) return;
      LARGE_INTEGER size {};
      if(!::GetFileSizeEx(file, &size) || size.QuadPart <= 0) return;
      length = static_cast<std::size_t>(size.QuadPart);
      mapping = ::CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
      if(mapping) data = static_cast<BYTE const*>(::MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0));
   }
   ~MappedFile() {
      if(data) ::UnmapViewOfFile(data);
      if(mapping) ::CloseHandle(mapping);
      if(file != INVALID_HANDLE_VALUE) ::CloseHandle(file);
   }
   MappedFile(MappedFile const&) = delete;
   MappedFile& operator=(MappedFile const&) = delete;

   template<typename T>
   T const* At(std::size_t pos) const {
      if(!data || pos > length || sizeof(T) > length - pos) return nullptr;
      return reinterpret_cast<T const*>(data + pos);
   }
};

void ReportImports(std::wstring const& path) {
   MappedFile const f { path };
   auto const* dos = f.At<IMAGE_DOS_HEADER>(0);
   if(!dos || dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0) {
      std::wprintf(L"[LOADER] PE: invalid DOS header or inaccessible file\n");
      return;
   }
   auto const* nt = f.At<IMAGE_NT_HEADERS64>(static_cast<std::size_t>(dos->e_lfanew));
   if(!nt || nt->Signature != IMAGE_NT_SIGNATURE) {
      std::wprintf(L"[LOADER] PE: invalid NT header\n");
      return;
   }
   std::wprintf(L"[LOADER] PE: Machine=0x%04X OptionalMagic=0x%04X Sections=%u\n",
      nt->FileHeader.Machine, nt->OptionalHeader.Magic,
      static_cast<unsigned>(nt->FileHeader.NumberOfSections));
   if(nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
      nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      std::wprintf(L"[LOADER] PE: ERROR - image is not AMD64 PE32+\n");
      return;
   }
   std::size_t const sectOffset = static_cast<std::size_t>(dos->e_lfanew) +
      sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + nt->FileHeader.SizeOfOptionalHeader;
   auto rva = [&](DWORD address, std::size_t need) -> std::size_t {
      for(unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
         auto const* section = f.At<IMAGE_SECTION_HEADER>(
            sectOffset + i * sizeof(IMAGE_SECTION_HEADER));
         if(!section) break;
         if(address < section->VirtualAddress) continue;
         std::size_t const displacement = address - section->VirtualAddress;
         if(displacement > section->SizeOfRawData ||
            need > section->SizeOfRawData - displacement) continue;
         std::size_t const pos = section->PointerToRawData + displacement;
         if(pos <= f.length && need <= f.length - pos) return pos;
      }
      return f.length;
   };
   if(nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_IMPORT) return;
   IMAGE_DATA_DIRECTORY const dir =
      nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
   if(dir.VirtualAddress == 0U || dir.Size == 0U) {
      std::wprintf(L"[LOADER] PE: no static import directory\n");
      return;
   }
   for(std::size_t i = 0; i < dir.Size / sizeof(IMAGE_IMPORT_DESCRIPTOR) && i < 1024U; ++i) {
      std::size_t const offset = rva(dir.VirtualAddress +
         static_cast<DWORD>(i * sizeof(IMAGE_IMPORT_DESCRIPTOR)), sizeof(IMAGE_IMPORT_DESCRIPTOR));
      auto const* descriptor = f.At<IMAGE_IMPORT_DESCRIPTOR>(offset);
      if(!descriptor) { std::wprintf(L"[LOADER] PE: malformed import descriptors\n"); break; }
      if(descriptor->Name == 0U) break;
      std::size_t const namePos = rva(descriptor->Name, 1U);
      if(namePos >= f.length) {
         std::wprintf(L"[LOADER] PE: malformed import name RVA\n"); continue;
      }
      char const* name = reinterpret_cast<char const*>(f.data + namePos);
      std::size_t const available = f.length - namePos;
      void const* zero = std::memchr(name, 0, available);
      if(!zero) { std::wprintf(L"[LOADER] PE: unterminated import name\n"); continue; }
      std::wprintf(L"[LOADER] imports: %hs\n", name);
   }
}

} // namespace

int main() {
   wchar_t dllName[512] {};
   DWORD const dllLen = ::GetEnvironmentVariableW(
      L"ADECC_RUNTIME_PROBE_DLL", dllName, static_cast<DWORD>(std::size(dllName)));
   if(dllLen == 0U || dllLen >= std::size(dllName)) {
      std::wprintf(L"[LOADER] ADECC_RUNTIME_PROBE_DLL is missing or too long\n");
      return 2;
   }
   if(std::wcschr(dllName, L'\\') || std::wcschr(dllName, L'/') ||
      std::wcschr(dllName, L':')) {
      std::wprintf(L"[LOADER] probe DLL must be a filename, not a path\n");
      return 2;
   }
   wchar_t exePath[32768] {};
   DWORD const exeLen = ::GetModuleFileNameW(
      nullptr, exePath, static_cast<DWORD>(std::size(exePath)));
   if(exeLen == 0U || exeLen >= std::size(exePath)) {
      std::wprintf(L"[LOADER] GetModuleFileNameW failed\n");
      return 2;
   }
   std::wstring root(exePath);
   std::size_t const cut = root.find_last_of(L"\\/");
   if(cut == std::wstring::npos) return 2;
   root.erase(cut + 1U);
   root += dllName;
   std::wprintf(L"[LOADER] executable: %ls\n", exePath);
   std::wprintf(L"[LOADER] expected application-local DLL: %ls\n", root.c_str());
   std::fflush(stdout);
   ReportImports(root);
   ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
   HMODULE const mod = ::LoadLibraryExW(root.c_str(), nullptr,
      LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
   if(!mod) {
      DWORD const error = ::GetLastError();
      std::wprintf(L"[LOADER] LoadLibraryExW FAILED: Win32=%lu (0x%08lX)\n",
         static_cast<unsigned long>(error), static_cast<unsigned long>(error));
      SystemError(error);
      std::wprintf(L"[LOADER] The error may originate in a transitive import; inspect the names above and Windows loader tracing.\n");
      return 1;
   }
   wchar_t loaded[32768] {};
   DWORD const got = ::GetModuleFileNameW(mod, loaded, static_cast<DWORD>(std::size(loaded)));
   if(got > 0 && got < std::size(loaded))
      std::wprintf(L"[LOADER] loaded DLL: %ls\n", loaded);
   std::wprintf(L"[LOADER] LoadLibraryExW PASS\n");
   ::FreeLibrary(mod);
   return 0;
}
