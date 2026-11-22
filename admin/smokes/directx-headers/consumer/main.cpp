#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <directx/dxcore.h>
#include <print>
#include <type_traits>

int main() {
   bool const bAPI = std::is_class_v<ID3D12Device> &&
      std::is_class_v<ID3D12GraphicsCommandList>;
   bool const bDefinitions = sizeof(D3D12_RESOURCE_DESC) > 0 &&
      sizeof(D3D12_HEAP_PROPERTIES) > 0;
   std::println("SMOKE|CHECK|directx-api|{}|Native BCC64X D3D12 interfaces",
      bAPI ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|directx-structures|{}|Native D3D12 declarations",
      bDefinitions ? "PASS" : "FAIL");
   bool const bOK = bAPI && bDefinitions;
   std::println("SMOKE|RESULT|{}|DirectX-Headers 1.619.1 consumer",
      bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
