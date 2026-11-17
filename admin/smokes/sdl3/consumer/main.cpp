#include <SDL3/SDL.h>
#include <print>
int main() {
   int const iVersion = SDL_GetVersion();
   bool const bValid = SDL_VERSIONNUM_MAJOR(iVersion) == 3;
   bool const bInitialized = SDL_Init(0);
   if(bInitialized) SDL_Quit();
   bool const bPass = bValid && bInitialized;
   std::println("SMOKE|CHECK|runtime|{}|SDL3 version/init", bPass ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|SDL3 consumer", bPass ? "PASS" : "FAIL");
   return bPass ? 0 : 1;
}
