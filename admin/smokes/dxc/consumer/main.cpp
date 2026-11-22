#include <dxc/dxcapi.h>
#include <print>
#include <type_traits>
int main() {
   bool const bOK = std::is_class_v<IDxcCompiler3> && std::is_class_v<IDxcUtils>;
   std::println("SMOKE|CHECK|dxc-api|{}|Native compiler COM API", bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|DXC API", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
