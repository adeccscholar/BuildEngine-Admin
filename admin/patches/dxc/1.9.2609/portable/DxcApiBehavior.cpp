// SPDX-License-Identifier: MIT
// Exercise the actual native DXC COM-shaped API without ATL, WEX, WRL or TAEF.
// DxcCreateInstance is a DXC factory: it does not require CoInitialize.
#include <dxc/dxcapi.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string_view>

namespace {
struct ReleaseInterface {
   template<class T>
   void operator()(T* pInterface) const noexcept {
      if(pInterface != nullptr) {
         pInterface->Release();
      }
   }
};

template<class T>
using ComHandle = std::unique_ptr<T, ReleaseInterface>;

bool CheckCompilation(IDxcCompiler3& aCompiler, bool bValid) {
   constexpr std::string_view svShader =
      "float4 main() : SV_Target { return float4(0.125, 0.25, 0.5, 1.0); }";
   std::array<LPCWSTR, 4> const arrArgs = {
      L"-T", L"ps_6_0", L"-E", bValid ? L"main" : L"missing_entry"
   };
   DxcBuffer const aSource {
      svShader.data(), svShader.size(), DXC_CP_UTF8
   };
   IDxcResult* pResult = nullptr;
   HRESULT const hrCompile = aCompiler.Compile(
      &aSource, arrArgs.data(), static_cast<UINT32>(arrArgs.size()),
      nullptr, __uuidof(IDxcResult),
      reinterpret_cast<void**>(&pResult));
   ComHandle<IDxcResult> theResult(pResult);
   if(FAILED(hrCompile) || theResult == nullptr) {
      std::fprintf(stderr, "DXC API invocation failed: 0x%08lx\n",
                   static_cast<unsigned long>(hrCompile));
      return false;
   }

   HRESULT hrStatus = E_FAIL;
   if(FAILED(theResult->GetStatus(&hrStatus))) {
      std::fputs("DXC result status unavailable\n", stderr);
      return false;
   }
   if(!bValid) {
      if(SUCCEEDED(hrStatus) || !theResult->HasOutput(DXC_OUT_ERRORS)) {
         std::fputs("DXC accepted invalid entry or omitted diagnostics\n", stderr);
         return false;
      }
      IDxcBlobUtf8* pErrors = nullptr;
      HRESULT const hrErrors = theResult->GetOutput(
         DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8),
         reinterpret_cast<void**>(&pErrors), nullptr);
      ComHandle<IDxcBlobUtf8> theErrors(pErrors);
      if(FAILED(hrErrors) || theErrors == nullptr ||
         theErrors->GetStringLength() == 0) {
         std::fputs("DXC returned an empty error diagnostic\n", stderr);
         return false;
      }
      return true;
   }
   if(FAILED(hrStatus) || !theResult->HasOutput(DXC_OUT_OBJECT)) {
      std::fputs("DXC failed to produce shader object\n", stderr);
      return false;
   }

   IDxcBlob* pObject = nullptr;
   HRESULT const hrObject = theResult->GetOutput(
      DXC_OUT_OBJECT, __uuidof(IDxcBlob),
      reinterpret_cast<void**>(&pObject), nullptr);
   ComHandle<IDxcBlob> theObject(pObject);
   if(FAILED(hrObject) || theObject == nullptr || theObject->GetBufferSize() < 4) {
      std::fputs("DXC returned no shader container\n", stderr);
      return false;
   }
   auto const* pBytes = static_cast<std::uint8_t const*>(theObject->GetBufferPointer());
   return pBytes[0] == 0x44 && pBytes[1] == 0x58 &&
          pBytes[2] == 0x42 && pBytes[3] == 0x43;
}
}

int main() {
   IDxcCompiler3* pCompiler = nullptr;
   HRESULT const hr = DxcCreateInstance(
      CLSID_DxcCompiler, __uuidof(IDxcCompiler3),
      reinterpret_cast<void**>(&pCompiler));
   ComHandle<IDxcCompiler3> theCompiler(pCompiler);
   if(FAILED(hr) || theCompiler == nullptr) {
      std::fprintf(stderr, "DXC API factory failed: 0x%08lx\n",
                   static_cast<unsigned long>(hr));
      return 1;
   }
   if(!CheckCompilation(*theCompiler, true)) {
      return 2;
   }
   if(!CheckCompilation(*theCompiler, false)) {
      return 3;
   }
   std::puts("DXC native API: successful shader and invalid-entry rejection PASS");
   return 0;
}
