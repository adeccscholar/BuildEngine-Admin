#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

#include <print>

int main() {
   // Verify that the client and DLL agree on JPH_VERSION_ID before
   // RegisterTypes(), which aborts on an ABI feature mismatch.
   bool const bAbiCompatible = JPH::VerifyJoltVersionID();
   std::println("SMOKE|CHECK|abi-version|{}|Jolt client and DLL feature versions match", bAbiCompatible ? "PASS" : "FAIL");
   if(!bAbiCompatible) {
      std::println("SMOKE|RESULT|FAIL|Jolt consumer ABI mismatch");
      return 1;
   }

   JPH::RegisterDefaultAllocator();
   JPH::Factory::sInstance = new JPH::Factory();
   bool const bFactory = JPH::Factory::sInstance != nullptr;
   if(bFactory) {
      JPH::RegisterTypes();
      JPH::UnregisterTypes();
   }
   delete JPH::Factory::sInstance;
   JPH::Factory::sInstance = nullptr;
   std::println("SMOKE|CHECK|factory|{}|Jolt runtime factory and registration", bFactory ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Jolt Physics consumer", bFactory ? "PASS" : "FAIL");
   return bFactory ? 0 : 1;
}
