#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <print>

int main() {
   JPH::RegisterDefaultAllocator();
   JPH::Factory::sInstance = new JPH::Factory();
   bool const bPassed = JPH::Factory::sInstance != nullptr;
   if(bPassed) {
      JPH::RegisterTypes();
      JPH::UnregisterTypes();
   }
   delete JPH::Factory::sInstance;
   JPH::Factory::sInstance = nullptr;
   std::println("SMOKE|CHECK|factory|{}|Jolt runtime factory and registration", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Jolt Physics consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
