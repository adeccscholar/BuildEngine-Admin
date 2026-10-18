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
#include <vector>

namespace {

void SystemError(DWORD code) {
   // FormatMessageW is localized. Printing it through the process CRT
   // emitted non-UTF-8 bytes and broke the BuildEngine output decoder.
   // Report the actual Win32 code separately, and keep names ASCII-only.
   wchar_t const* name = L"UNKNOWN_WIN32_ERROR";
   switch(code) {
      case ERROR_BAD_EXE_FORMAT: name = L"ERROR_BAD_EXE_FORMAT"; break;
      case ERROR_MOD_NOT_FOUND: name = L"ERROR_MOD_NOT_FOUND"; break;
      case ERROR_PROC_NOT_FOUND: name = L"ERROR_PROC_NOT_FOUND"; break;
      case ERROR_DLL_INIT_FAILED: name = L"ERROR_DLL_INIT_FAILED"; break;
      case ERROR_FILE_NOT_FOUND: name = L"ERROR_FILE_NOT_FOUND"; break;
      case ERROR_PATH_NOT_FOUND: name = L"ERROR_PATH_NOT_FOUND"; break;
   }
   std::wprintf(L"[LOADER] Windows error: %ls\n", name);
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


bool ProbeOne(std::wstring const& name, std::wstring const& appDirectory,
              std::size_t index, std::size_t total) {
   std::wprintf(L"[LOADER] BEGIN %zu/%zu: %ls\n", index, total, name.c_str());
   if(name.empty() || name == L"." || name == L".." ||
      name.find_first_of(L"\\/:") != std::wstring::npos) {
      std::wprintf(L"[LOADER] RESULT %ls: FAIL (invalid DLL filename)\n", name.c_str());
      return false;
   }

   // Always prefer the exact application-local package DLL. An Embarcadero
   // runtime may already be loaded by the probe itself; in that case use
   // GetModuleFileNameW to inspect and load that exact existing image.
   std::wstring path = appDirectory + name;
   DWORD const attributes = ::GetFileAttributesW(path.c_str());
   bool const local = attributes != INVALID_FILE_ATTRIBUTES &&
      (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0U;
   if(!local) {
      HMODULE const alreadyLoaded = ::GetModuleHandleW(name.c_str());
      if(!alreadyLoaded) {
         std::wprintf(L"[LOADER] RESULT %ls: FAIL (not application-local or already loaded)\n",
            name.c_str());
         return false;
      }
      wchar_t resolved[32768] {};
      DWORD const length = ::GetModuleFileNameW(alreadyLoaded, resolved,
         static_cast<DWORD>(std::size(resolved)));
      if(length == 0U || length >= std::size(resolved)) {
         std::wprintf(L"[LOADER] RESULT %ls: FAIL (cannot resolve already-loaded DLL path)\n",
            name.c_str());
         return false;
      }
      path.assign(resolved, length);
      std::wprintf(L"[LOADER] already-loaded module: %ls\n", path.c_str());
   }
   else {
      std::wprintf(L"[LOADER] application-local module: %ls\n", path.c_str());
   }

   ReportImports(path);
   std::fflush(stdout);
   ::SetLastError(ERROR_SUCCESS);
   HMODULE const loaded = ::LoadLibraryExW(path.c_str(), nullptr,
      LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
   if(!loaded) {
      DWORD const error = ::GetLastError();
      std::wprintf(L"[LOADER] LoadLibraryExW FAILED: Win32=%lu (0x%08lX), DLL=%ls\n",
         static_cast<unsigned long>(error), static_cast<unsigned long>(error), name.c_str());
      SystemError(error);
      std::wprintf(L"[LOADER] RESULT %ls: FAIL\n", name.c_str());
      std::fflush(stdout);
      return false;
   }

   wchar_t actual[32768] {};
   DWORD const count = ::GetModuleFileNameW(loaded, actual,
      static_cast<DWORD>(std::size(actual)));
   if(count != 0U && count < std::size(actual))
      std::wprintf(L"[LOADER] loaded DLL: %ls\n", actual);
   std::wprintf(L"[LOADER] RESULT %ls: PASS\n", name.c_str());
   ::FreeLibrary(loaded);
   std::fflush(stdout);
   return true;
}

} // namespace

int main() {
   // The ordered list is declared in build-libraries.xml, not in this
   // generic probe. The old one-DLL variable remains accepted.
   wchar_t values[4096] {};
   DWORD n = ::GetEnvironmentVariableW(L"ADECC_RUNTIME_PROBE_DLLS",
      values, static_cast<DWORD>(std::size(values)));
   if(n == 0U) {
      n = ::GetEnvironmentVariableW(L"ADECC_RUNTIME_PROBE_DLL",
         values, static_cast<DWORD>(std::size(values)));
   }
   if(n == 0U || n >= std::size(values)) {
      std::wprintf(L"[LOADER] Missing or oversized ADECC_RUNTIME_PROBE_DLLS environment setting\n");
      return 2;
   }

   wchar_t executable[32768] {};
   DWORD const exeLength = ::GetModuleFileNameW(nullptr, executable,
      static_cast<DWORD>(std::size(executable)));
   if(exeLength == 0U || exeLength >= std::size(executable)) {
      std::wprintf(L"[LOADER] GetModuleFileNameW failed\n");
      return 2;
   }
   std::wstring directory(executable, exeLength);
   std::size_t const slash = directory.find_last_of(L"\\/");
   if(slash == std::wstring::npos) return 2;
   directory.erase(slash + 1U);

   std::vector<std::wstring> libraries;
   std::wstring const specification(values, n);
   std::size_t begin = 0U;
   for(;;) {
      std::size_t const separator = specification.find(L';', begin);
      std::wstring const item = specification.substr(begin,
         separator == std::wstring::npos ? separator : separator - begin);
      std::size_t const first = item.find_first_not_of(L" \t\r\n");
      std::size_t const last = item.find_last_not_of(L" \t\r\n");
      if(first == std::wstring::npos || libraries.size() >= 64U) {
         std::wprintf(L"[LOADER] Invalid or oversized ordered DLL list\n");
         return 2;
      }
      libraries.push_back(item.substr(first, last - first + 1U));
      if(separator == std::wstring::npos) break;
      begin = separator + 1U;
   }

   ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
   std::wprintf(L"[LOADER] executable: %ls\n", executable);
   std::wprintf(L"[LOADER] ordered DLL checks: %zu\n", libraries.size());
   std::fflush(stdout);

   std::size_t passed = 0U;
   for(std::size_t i = 0U; i < libraries.size(); ++i) {
      // Deliberately continue after each failure to gather all evidence.
      if(ProbeOne(libraries[i], directory, i + 1U, libraries.size()))
         ++passed;
   }
   std::wprintf(L"[LOADER] SUMMARY: %zu/%zu PASS, %zu FAIL\n",
      passed, libraries.size(), libraries.size() - passed);
   std::fflush(stdout);
   return passed == libraries.size() ? 0 : 1;
}
