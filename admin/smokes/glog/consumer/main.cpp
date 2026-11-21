#include <glog/logging.h>
#include <print>

int main(int iArgc, char** pszArgv) {
   bool const bArguments = iArgc > 0 && pszArgv != nullptr && pszArgv[0] != nullptr;
   std::println("SMOKE|CHECK|program-arguments|{}|Logging process has a valid program name", bArguments ? "PASS" : "FAIL");
   if(!bArguments) {
      std::println("SMOKE|RESULT|FAIL|No program name available");
      return 1;
   }
   google::InitGoogleLogging(pszArgv[0]);
   LOG(INFO) << "BuildEngine native glog smoke";
   google::ShutdownGoogleLogging();
   std::println("SMOKE|CHECK|logging|PASS|Native glog initialized and shut down");
   std::println("SMOKE|RESULT|PASS|glog shared native consumer");
   return 0;
}
