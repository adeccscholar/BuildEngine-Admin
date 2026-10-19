// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Run as a separate process: it must NOT import the DLL being probed.
// The DLL name is explicitly supplied by the smoke XML environment.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstddef>
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


void DiagnoseImage(std::wstring const& path) {
   // A PE32+ AMD64 signature is not sufficient to prove Windows can map an
   // image. Inspect its section table and ask the kernel to create an
   // image section without resolving imports or calling DllMain.
   MappedFile const file { path };
   auto const* dos = file.At<IMAGE_DOS_HEADER>(0U);
   if(!dos || dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0) {
      std::wprintf(L"[LOADER] IMAGE: cannot inspect DOS header\n");
      return;
   }
   std::size_t const ntOffset = static_cast<std::size_t>(dos->e_lfanew);
   auto const* nt = file.At<IMAGE_NT_HEADERS64>(ntOffset);
   if(!nt || nt->Signature != IMAGE_NT_SIGNATURE ||
      nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      std::wprintf(L"[LOADER] IMAGE: cannot inspect PE32+ optional header\n");
      return;
   }

   WORD const characteristics = nt->FileHeader.Characteristics;
   auto const& opt = nt->OptionalHeader;
   std::wprintf(L"[LOADER] IMAGE: FileBytes=%llu DLL=%u Characteristics=0x%04X\n",
      static_cast<unsigned long long>(file.length),
      static_cast<unsigned>((characteristics & IMAGE_FILE_DLL) != 0U),
      static_cast<unsigned>(characteristics));
   std::wprintf(L"[LOADER] IMAGE: SizeOfImage=0x%08lX SizeOfHeaders=0x%08lX EntryRVA=0x%08lX\n",
      static_cast<unsigned long>(opt.SizeOfImage),
      static_cast<unsigned long>(opt.SizeOfHeaders),
      static_cast<unsigned long>(opt.AddressOfEntryPoint));
   std::wprintf(L"[LOADER] IMAGE: ImageBase=0x%llX SectionAlignment=0x%08lX FileAlignment=0x%08lX DllCharacteristics=0x%04X\n",
      static_cast<unsigned long long>(opt.ImageBase),
      static_cast<unsigned long>(opt.SectionAlignment),
      static_cast<unsigned long>(opt.FileAlignment),
      static_cast<unsigned>(opt.DllCharacteristics));

   if(opt.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_BASERELOC) {
      auto const& reloc = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
      std::wprintf(L"[LOADER] IMAGE: BaseRelocRVA=0x%08lX BaseRelocSize=0x%08lX\n",
         static_cast<unsigned long>(reloc.VirtualAddress),
         static_cast<unsigned long>(reloc.Size));
   }
   if(opt.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_IMPORT) {
      auto const& imports = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
      std::wprintf(L"[LOADER] IMAGE: ImportRVA=0x%08lX ImportSize=0x%08lX\n",
         static_cast<unsigned long>(imports.VirtualAddress),
         static_cast<unsigned long>(imports.Size));
   }
   if(opt.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_EXCEPTION) {
      auto const& unwind = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
      std::wprintf(L"[LOADER] IMAGE: ExceptionRVA=0x%08lX ExceptionSize=0x%08lX\n",
         static_cast<unsigned long>(unwind.VirtualAddress),
         static_cast<unsigned long>(unwind.Size));
   }

   std::size_t const sectionOffset = ntOffset + sizeof(DWORD) +
      sizeof(IMAGE_FILE_HEADER) + nt->FileHeader.SizeOfOptionalHeader;
   unsigned invalidSections = 0U;
   for(unsigned i = 0U; i < nt->FileHeader.NumberOfSections; ++i) {
      auto const* section = file.At<IMAGE_SECTION_HEADER>(
         sectionOffset + i * sizeof(IMAGE_SECTION_HEADER));
      if(!section) {
         std::wprintf(L"[LOADER] IMAGE: section table truncated at %u\n", i);
         ++invalidSections;
         break;
      }
      char sectionName[9] {};
      for(std::size_t j = 0U; j < sizeof(section->Name); ++j) {
         unsigned char const c = section->Name[j];
         sectionName[j] = (c == 0U || (c >= 32U && c < 127U))
            ? static_cast<char>(c) : '?';
      }
      std::size_t const raw = section->PointerToRawData;
      bool const rawValid = (section->SizeOfRawData == 0U) ||
         (raw <= file.length && section->SizeOfRawData <= file.length - raw);
      bool const rvaValid = section->VirtualAddress <= opt.SizeOfImage &&
         section->Misc.VirtualSize <= opt.SizeOfImage - section->VirtualAddress;
      if(!rawValid || !rvaValid) ++invalidSections;
      std::wprintf(L"[LOADER] IMAGE: section[%u] %hs RVA=0x%08lX VirtualSize=0x%08lX Raw=0x%08lX RawSize=0x%08lX rawValid=%u rvaValid=%u\n",
         i, sectionName, static_cast<unsigned long>(section->VirtualAddress),
         static_cast<unsigned long>(section->Misc.VirtualSize),
         static_cast<unsigned long>(section->PointerToRawData),
         static_cast<unsigned long>(section->SizeOfRawData),
         static_cast<unsigned>(rawValid), static_cast<unsigned>(rvaValid));
   }
   std::wprintf(L"[LOADER] IMAGE: suspicious-section-count=%u\n", invalidSections);


   // DllCharacteristics=0x4000 marks Control Flow Guard support. An invalid
   // load-config directory or invalid TLS virtual addresses are especially
   // relevant when SEC_IMAGE mapping succeeds but normal LoadLibrary fails.
   // Read RVA-backed structures from file offsets; never treat an RVA as an
   // ordinary pointer into the raw-mapped file.
   auto rawOffset = [&](DWORD rva, std::size_t bytes) -> std::size_t {
      for(unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
         auto const* section = file.At<IMAGE_SECTION_HEADER>(
            sectionOffset + i * sizeof(IMAGE_SECTION_HEADER));
         if(!section || rva < section->VirtualAddress) continue;
         std::size_t const displacement = rva - section->VirtualAddress;
         if(displacement > section->SizeOfRawData ||
            bytes > section->SizeOfRawData - displacement) continue;
         std::size_t const offset = section->PointerToRawData + displacement;
         if(offset <= file.length && bytes <= file.length - offset) return offset;
      }
      return file.length;
   };
   if(opt.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG) {
      auto const& load = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
      bool const guardCf = (opt.DllCharacteristics &
         IMAGE_DLLCHARACTERISTICS_GUARD_CF) != 0U;
      std::wprintf(L"[LOADER] IMAGE: CFG-flag=%u LoadConfigRVA=0x%08lX LoadConfigSize=0x%08lX\n",
         static_cast<unsigned>(guardCf),
         static_cast<unsigned long>(load.VirtualAddress),
         static_cast<unsigned long>(load.Size));
      if(load.VirtualAddress != 0U && load.Size >= sizeof(DWORD)) {
         std::size_t const at = rawOffset(load.VirtualAddress, sizeof(DWORD));
         auto const* declaredSize = file.At<DWORD>(at);
         if(declaredSize)
            std::wprintf(L"[LOADER] IMAGE: LoadConfigDeclaredSize=0x%08lX\n",
               static_cast<unsigned long>(*declaredSize));
         constexpr std::size_t guardOffset =
            offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, GuardFlags);
         if(load.Size >= guardOffset + sizeof(DWORD)) {
            std::size_t const guardAt = rawOffset(
               load.VirtualAddress + static_cast<DWORD>(guardOffset), sizeof(DWORD));
            auto const* const flags = file.At<DWORD>(guardAt);
            if(flags)
               std::wprintf(L"[LOADER] IMAGE: LoadConfig.GuardFlags=0x%08lX\n",
                  static_cast<unsigned long>(*flags));
            else
               std::wprintf(L"[LOADER] IMAGE: LoadConfig.GuardFlags RVA INVALID\n");
         }
      }
   }
   if(opt.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_TLS) {
      auto const& tls = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
      std::wprintf(L"[LOADER] IMAGE: TLS-RVA=0x%08lX TLS-Size=0x%08lX\n",
         static_cast<unsigned long>(tls.VirtualAddress),
         static_cast<unsigned long>(tls.Size));
      if(tls.VirtualAddress != 0U) {
         std::size_t const at = rawOffset(tls.VirtualAddress,
            sizeof(IMAGE_TLS_DIRECTORY64));
         auto const* t = file.At<IMAGE_TLS_DIRECTORY64>(at);
         if(!t) {
            std::wprintf(L"[LOADER] IMAGE: TLS directory truncated or not in raw section\n");
         }
         else {
            auto inImage = [&](ULONGLONG va) -> unsigned {
               return static_cast<unsigned>(va >= opt.ImageBase &&
                  va - opt.ImageBase < opt.SizeOfImage);
            };
            std::wprintf(L"[LOADER] IMAGE: TLS RawStart=0x%llX RawEnd=0x%llX IndexVA=0x%llX CallbackVA=0x%llX\n",
               static_cast<unsigned long long>(t->StartAddressOfRawData),
               static_cast<unsigned long long>(t->EndAddressOfRawData),
               static_cast<unsigned long long>(t->AddressOfIndex),
               static_cast<unsigned long long>(t->AddressOfCallBacks));
            std::wprintf(L"[LOADER] IMAGE: TLS index-in-image=%u callbacks-in-image=%u raw-start-in-image=%u raw-end-in-image=%u ZeroFill=%lu\n",
               inImage(t->AddressOfIndex),
               static_cast<unsigned>(t->AddressOfCallBacks == 0U ||
                  inImage(t->AddressOfCallBacks)),
               inImage(t->StartAddressOfRawData),
               inImage(t->EndAddressOfRawData),
               static_cast<unsigned long>(t->SizeOfZeroFill));
         }
      }
   }

   HANDLE const rawFile = ::CreateFileW(path.c_str(), GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
   if(rawFile == INVALID_HANDLE_VALUE) {
      DWORD const error = ::GetLastError();
      std::wprintf(L"[LOADER] IMAGE CreateFileW FAILED: Win32=%lu\n",
         static_cast<unsigned long>(error));
      SystemError(error);
      return;
   }
   HANDLE const imageMapping = ::CreateFileMappingW(rawFile, nullptr,
      PAGE_READONLY | SEC_IMAGE_NO_EXECUTE, 0U, 0U, nullptr);
   if(!imageMapping) {
      DWORD const error = ::GetLastError();
      std::wprintf(L"[LOADER] IMAGE CreateFileMappingW(SEC_IMAGE_NO_EXECUTE) FAILED: Win32=%lu (0x%08lX)\n",
         static_cast<unsigned long>(error), static_cast<unsigned long>(error));
      SystemError(error);
      ::CloseHandle(rawFile);
      return;
   }
   void const* const image = ::MapViewOfFile(imageMapping, FILE_MAP_READ,
      0U, 0U, 0U);
   if(image) {
      std::wprintf(L"[LOADER] IMAGE SEC_IMAGE_NO_EXECUTE mapping PASS (imports and DllMain not executed)\n");
      ::UnmapViewOfFile(image);
   }
   else {
      DWORD const error = ::GetLastError();
      std::wprintf(L"[LOADER] IMAGE MapViewOfFile FAILED: Win32=%lu (0x%08lX)\n",
         static_cast<unsigned long>(error), static_cast<unsigned long>(error));
      SystemError(error);
   }
   ::CloseHandle(imageMapping);
   ::CloseHandle(rawFile);
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
      for(wchar_t const* szDependency : {L"libc++-370.dll", L"libunwind-370.dll"}) {
         HMODULE const hExisting = ::GetModuleHandleW(szDependency);
         ::SetLastError(ERROR_SUCCESS);
         HMODULE const hModule = hExisting ? hExisting :
            ::LoadLibraryExW(szDependency, nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
         DWORD const uDependencyError = hModule ? ERROR_SUCCESS : ::GetLastError();
         wchar_t szResolved[32768] {};
         if(hModule)
            ::GetModuleFileNameW(hModule, szResolved, static_cast<DWORD>(std::size(szResolved)));
         std::wprintf(L"[LOADER] RUNTIME %ls: %ls already-loaded=%u Win32=%lu path=%ls\n",
            szDependency, hModule ? L"PASS" : L"FAIL",
            static_cast<unsigned>(hExisting != nullptr),
            static_cast<unsigned long>(uDependencyError),
            hModule ? szResolved : L"<unresolved>");
         if(hModule && !hExisting) ::FreeLibrary(hModule);
      }
      if(error == ERROR_BAD_EXE_FORMAT) {
         DiagnoseImage(path);

         // Diagnostic only: map the target DLL without resolving imports or
         // invoking its entry point. Never call exports from this handle.
         // This does not identify the failing transitive dependency.
         ::SetLastError(ERROR_SUCCESS);
         HMODULE const imageOnly = ::LoadLibraryExW(
            path.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
         if(imageOnly) {
            std::wprintf(L"[LOADER] DONT_RESOLVE_DLL_REFERENCES: PASS (target mapped without import resolution or DllMain)\n");
            ::FreeLibrary(imageOnly);
         }
         else {
            DWORD const mapError = ::GetLastError();
            std::wprintf(L"[LOADER] DONT_RESOLVE_DLL_REFERENCES: FAIL Win32=%lu (0x%08lX)\n",
               static_cast<unsigned long>(mapError),
               static_cast<unsigned long>(mapError));
            SystemError(mapError);
         }
      }
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
