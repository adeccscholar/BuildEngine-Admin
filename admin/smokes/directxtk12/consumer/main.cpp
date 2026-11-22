#include <directxtk12/SpriteBatch.h>
#include <directxtk12/GeometricPrimitive.h>
#include <print>
#include <type_traits>
int main() {
   bool const bOK = std::is_class_v<DirectX::SpriteBatch> && std::is_class_v<DirectX::GeometricPrimitive>;
   std::println("SMOKE|CHECK|directxtk12-api|{}|Native D3D12 toolkit API", bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|DirectXTK12", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
