#include <DirectXTex.h>
#include <objbase.h>
#include <format>
#include <print>

/**
 \brief Validates native DirectXTex mipmap generation against its installed DLL.
 \details Initializes Windows COM for WIC-backed filtering, creates a complete
          8x8 RGBA mip chain and prints the exact HRESULT on failure.
 \returns Zero when all four mip levels were generated.
 \throw None. HRESULT errors are reported in the smoke protocol.
 */
int main() {
   HRESULT const hCom = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
   if(FAILED(hCom)) {
      std::println("SMOKE|CHECK|com-initialization|FAIL|HRESULT={:#010x}",
         static_cast<unsigned int>(hCom));
      std::println("SMOKE|RESULT|FAIL|DirectXTex COM initialization");
      return 1;
   }

   struct ComScope {
      ~ComScope() noexcept { ::CoUninitialize(); }
   } const theComScope;

   DirectX::ScratchImage theBase;
   HRESULT const hInitialize = theBase.Initialize2D(
      DXGI_FORMAT_R8G8B8A8_UNORM, 8u, 8u, 1u, 1u);
   if(FAILED(hInitialize)) {
      std::println("SMOKE|CHECK|scratch-allocation|FAIL|HRESULT={:#010x}",
         static_cast<unsigned int>(hInitialize));
      std::println("SMOKE|RESULT|FAIL|DirectXTex scratch allocation");
      return 1;
   }

   DirectX::ScratchImage theMipChain;
   HRESULT const hMipmaps = DirectX::GenerateMipMaps(
      *theBase.GetImage(0u, 0u, 0u), DirectX::TEX_FILTER_DEFAULT, 0u, theMipChain);
   bool const bOK = SUCCEEDED(hMipmaps) && theMipChain.GetImageCount() == 4u;
   std::println("SMOKE|CHECK|mipmap-generation|{}|HRESULT={:#010x}, mip-levels={}, expected=4",
      bOK ? "PASS" : "FAIL", static_cast<unsigned int>(hMipmaps),
      theMipChain.GetImageCount());
   std::println("SMOKE|RESULT|{}|DirectXTex 2026.5.8", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
