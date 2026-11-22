#include <DirectXTex.h>
#include <print>

int main() {
   DirectX::ScratchImage theBase;
   HRESULT const hInitialize = theBase.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 8u, 8u, 1u, 1u);
   if(FAILED(hInitialize)) {
      std::println("SMOKE|RESULT|FAIL|DirectXTex scratch allocation");
      return 1;
   }
   DirectX::ScratchImage theMipChain;
   HRESULT const hMipmaps = DirectX::GenerateMipMaps(
      *theBase.GetImage(0u, 0u, 0u), DirectX::TEX_FILTER_DEFAULT, 0u, theMipChain);
   bool const bOK = SUCCEEDED(hMipmaps) && theMipChain.GetImageCount() >= 4u;
   std::println("SMOKE|CHECK|mipmap-generation|{}|Native texture mipmaps", bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|DirectXTex 2026.5.8", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
