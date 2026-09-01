#include <GL/gl.h>
#include <print>
int main(){ auto p=&glGetError; bool ok=p!=nullptr; std::println("SMOKE|CHECK|link|{}|glGetError linked",ok?"PASS":"FAIL"); std::println("SMOKE|RESULT|{}|OpenGL consumer usable",ok?"PASS":"FAIL"); return ok?0:1; }
