#pragma once
// DirectX-Headers v1.619.1: diagnostics only; no COM calls or test assertions changed.
#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <string>

namespace adecc::dxcore_diagnostics {

inline void PrintModule(char const* const szLabel, HMODULE const hModule) {
   if(hModule == nullptr) {
      std::fprintf(stderr, "[DXCORE-ABI] %s: module not loaded (GetLastError=%lu)\n",
         szLabel, static_cast<unsigned long>(::GetLastError()));
      std::fflush(stderr);
      return;
   }
   wchar_t arrPath[32768] {};
   DWORD const uLength = ::GetModuleFileNameW(hModule, arrPath, 32768);
   if(uLength == 0U || uLength >= 32768U) {
      std::fprintf(stderr, "[DXCORE-ABI] %s: GetModuleFileNameW failed, module=%p, error=%lu\n",
         szLabel, static_cast<void*>(hModule), static_cast<unsigned long>(::GetLastError()));
      std::fflush(stderr);
      return;
   }
   int const iLength = ::WideCharToMultiByte(CP_UTF8, 0, arrPath,
      static_cast<int>(uLength), nullptr, 0, nullptr, nullptr);
   if(iLength <= 0) return;
   std::string strPath(static_cast<std::size_t>(iLength), '\0');
   ::WideCharToMultiByte(CP_UTF8, 0, arrPath, static_cast<int>(uLength),
      strPath.data(), iLength, nullptr, nullptr);
   std::fprintf(stderr, "[DXCORE-ABI] %s: module=%p, path=%s\n",
      szLabel, static_cast<void*>(hModule), strPath.c_str());
   std::fflush(stderr);
}

inline bool Readable(void const* const pLocation, std::size_t const uBytes) {
   MEMORY_BASIC_INFORMATION theInfo {};
   if(!pLocation || ::VirtualQuery(pLocation, &theInfo, sizeof(theInfo)) != sizeof(theInfo))
      return false;
   if(theInfo.State != MEM_COMMIT ||
      (theInfo.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0U)
      return false;
   std::uintptr_t const uStart = reinterpret_cast<std::uintptr_t>(pLocation);
   std::uintptr_t const uRegionStart = reinterpret_cast<std::uintptr_t>(theInfo.BaseAddress);
   if(uStart < uRegionStart || uStart - uRegionStart >= theInfo.RegionSize)
      return false;
   return uBytes <= theInfo.RegionSize - (uStart - uRegionStart);
}

inline void PrintStage(char const* const szStage, void const* const pFactory) {
   std::fprintf(stderr, "[DXCORE-ABI] stage=%s, pid=%lu, thread=%lu, ptr_size=%zu, factory=%p\n",
      szStage, static_cast<unsigned long>(::GetCurrentProcessId()),
      static_cast<unsigned long>(::GetCurrentThreadId()), sizeof(void*), pFactory);
   PrintModule("dxcore.dll", ::GetModuleHandleW(L"dxcore.dll"));
   PrintModule("d3d12.dll", ::GetModuleHandleW(L"d3d12.dll"));
   if(!Readable(pFactory, sizeof(void*))) {
      std::fprintf(stderr, "[DXCORE-ABI] factory pointer is not readable\n");
      std::fflush(stderr);
      return;
   }
   void const* const* const pVtable =
      *reinterpret_cast<void const* const* const*>(pFactory);
   std::fprintf(stderr, "[DXCORE-ABI] factory vtable=%p\n",
      static_cast<void const*>(pVtable));
   if(!Readable(pVtable, 4U * sizeof(void*))) {
      std::fprintf(stderr, "[DXCORE-ABI] vtable[0..3] is not readable\n");
      std::fflush(stderr);
      return;
   }
   for(unsigned uSlot = 0U; uSlot < 4U; ++uSlot) {
      void const* const pEntry = pVtable[uSlot];
      MEMORY_BASIC_INFORMATION theInfo {};
      std::size_t const uQueried = ::VirtualQuery(pEntry, &theInfo, sizeof(theInfo));
      std::fprintf(stderr,
         "[DXCORE-ABI] vtable[%u]=%p, allocation=%p, protect=0x%lX, state=0x%lX\n",
         uSlot, pEntry, uQueried ? theInfo.AllocationBase : nullptr,
         uQueried ? static_cast<unsigned long>(theInfo.Protect) : 0UL,
         uQueried ? static_cast<unsigned long>(theInfo.State) : 0UL);
      if(uSlot == 3U && uQueried) {
         HMODULE hCodeModule {};
         if(::GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(pEntry), &hCodeModule))
            PrintModule("CreateAdapterList vtable slot 3", hCodeModule);
      }
   }
   std::fflush(stderr);
}

} // namespace adecc::dxcore_diagnostics
