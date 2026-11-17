#include <box2d/box2d.h>
#include <print>
int main() {
   b2Version const theVersion = b2GetVersion();
   b2WorldDef const theDefinition = b2DefaultWorldDef();
   b2WorldId const theWorld = b2CreateWorld(&theDefinition);
   bool const bPass = theVersion.major == 3 && b2World_IsValid(theWorld);
   if(b2World_IsValid(theWorld)) b2DestroyWorld(theWorld);
   std::println("SMOKE|CHECK|runtime|{}|Box2D world and version", bPass ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Box2D consumer", bPass ? "PASS" : "FAIL");
   return bPass ? 0 : 1;
}
