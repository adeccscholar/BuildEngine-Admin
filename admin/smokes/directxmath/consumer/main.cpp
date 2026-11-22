#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <cmath>
#include <print>

int main() {
   DirectX::XMVECTOR const thePosition = DirectX::XMVectorSet(1.f, 2.f, 3.f, 1.f);
   DirectX::XMMATRIX const theTranslation = DirectX::XMMatrixTranslation(2.f, -1.f, 4.f);
   DirectX::XMVECTOR const theTransformed = DirectX::XMVector3TransformCoord(thePosition, theTranslation);
   DirectX::XMFLOAT3 theResult {};
   DirectX::XMStoreFloat3(&theResult, theTransformed);
   DirectX::BoundingSphere const theSphere {{3.f, 1.f, 7.f}, 2.f};
   bool const bVector = std::fabs(theResult.x - 3.f) < 0.0001f &&
      std::fabs(theResult.y - 1.f) < 0.0001f && std::fabs(theResult.z - 7.f) < 0.0001f;
   bool const bCollision = theSphere.Contains(DirectX::XMVectorSet(3.f, 1.f, 7.f, 1.f)) != DirectX::DISJOINT;
   bool const bOK = bVector && bCollision;
   std::println("SMOKE|CHECK|simd-transform|{}|Native vector/matrix transform", bVector ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|collision|{}|Bounding sphere containment", bCollision ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|DirectXMath and DirectXCollision", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
