#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <print>

int main() {
   unsigned int const uMajor=MA_VERSION_MAJOR;
   unsigned int const uMinor=MA_VERSION_MINOR;
   bool const bVersion = uMajor==0U && uMinor>=11U;
   std::println("SMOKE|CHECK|version|{}|miniaudio version 0.11 or later", bVersion ? "PASS" : "FAIL");
   // A build-only test of the fade API without opening any audio device.
   using fade_fn_ty=decltype(&ma_sound_set_fade_in_milliseconds);
   fade_fn_ty const theFade=&ma_sound_set_fade_in_milliseconds;
   bool const bFade = theFade!=nullptr;
   std::println("SMOKE|CHECK|fade-api|{}|miniaudio fade API available", bFade ? "PASS" : "FAIL");
   bool const bPassed = bVersion && bFade;
   std::println("SMOKE|RESULT|{}|miniaudio consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
