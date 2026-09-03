#include <SOIL2/SOIL2.h>

#include <print>

int main() {
   unsigned long const uRuntimeVersion = SOIL_version();
   bool const bVersion = uRuntimeVersion == SOIL_COMPILED_VERSION;

   std::println("SMOKE|CHECK|version|{}|SOIL2 header/runtime version {}",
                bVersion ? "PASS" : "FAIL", uRuntimeVersion);
   std::println("SMOKE|RESULT|{}|SOIL2 consumer usable", bVersion ? "PASS" : "FAIL");
   return bVersion ? 0 : 1;
}
