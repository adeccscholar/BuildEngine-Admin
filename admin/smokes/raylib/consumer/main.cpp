#include <raylib.h>
#include <print>
int main(){ bool ok=!IsWindowReady(); std::println("SMOKE|CHECK|link|{}|raylib API linked",ok?"PASS":"FAIL"); std::println("SMOKE|RESULT|{}|raylib consumer usable",ok?"PASS":"FAIL"); return ok?0:1; }
