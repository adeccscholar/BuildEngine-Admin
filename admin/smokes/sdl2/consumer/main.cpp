#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <print>
int main(){ SDL_version v{}; SDL_GetVersion(&v); int rc=SDL_Init(0); bool ok=rc==0 && v.major==2; if(rc==0) SDL_Quit(); std::println("SMOKE|CHECK|runtime|{}|SDL2 init/version",ok?"PASS":"FAIL"); std::println("SMOKE|RESULT|{}|SDL2 consumer usable",ok?"PASS":"FAIL"); return ok?0:1; }
