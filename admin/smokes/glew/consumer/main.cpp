#include <GL/glew.h>
#include <print>
int main(){ auto p=glewGetErrorString(GLEW_OK); bool ok=p!=nullptr; std::println("SMOKE|CHECK|api|{}|GLEW API linked",ok?"PASS":"FAIL"); std::println("SMOKE|RESULT|{}|GLEW consumer usable",ok?"PASS":"FAIL"); return ok?0:1; }
