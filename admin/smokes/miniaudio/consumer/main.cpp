#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <cstdio>

int main() {
   unsigned int const uMajor=MA_VERSION_MAJOR;
   unsigned int const uMinor=MA_VERSION_MINOR;
   if(uMajor!=0U || uMinor<11U) {
      std::puts("SMOKE|RESULT|FAIL");
      return 1;
   }
   // A build-only test of the fade API without opening any audio device.
   using fade_fn_ty=decltype(&ma_sound_set_fade_in_milliseconds);
   fade_fn_ty const theFade=&ma_sound_set_fade_in_milliseconds;
   if(theFade==nullptr) return 2;
   std::puts("SMOKE|CHECK|miniaudio fade API available");
   std::puts("SMOKE|RESULT|PASS");
   return 0;
}
