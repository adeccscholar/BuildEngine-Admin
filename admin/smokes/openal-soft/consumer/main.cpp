#include <AL/alc.h>
#include <print>

int main() {
   ALCenum const iEnum = alcGetEnumValue(nullptr, "ALC_MAJOR_VERSION");
   bool const bPassed = iEnum == ALC_MAJOR_VERSION;
   std::println("SMOKE|CHECK|alc-enum|{}|OpenAL C API works without a physical device",
      bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|OpenAL library API without physical device",
      bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
   }
