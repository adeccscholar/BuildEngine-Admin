#include <DirectXMesh.h>
#include <DirectXMath.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <print>

int main() {
   std::array<DirectX::XMFLOAT3, 3> const arrPositions {{
      {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}
   }};
   std::array<std::uint32_t, 3> const arrIndices {0u, 1u, 2u};
   std::array<DirectX::XMFLOAT3, 3> arrNormals {};
   HRESULT const hResult = DirectX::ComputeNormals(arrIndices.data(), 1u,
      arrPositions.data(), arrPositions.size(), DirectX::CNORM_DEFAULT, arrNormals.data());
   bool const bNormal = SUCCEEDED(hResult) && std::fabs(arrNormals[0].z) > 0.99f;
   std::println("SMOKE|CHECK|mesh-normals|{}|Native triangle normal computation",
      bNormal ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|DirectXMesh 2026.5.8", bNormal ? "PASS" : "FAIL");
   return bNormal ? 0 : 1;
}
