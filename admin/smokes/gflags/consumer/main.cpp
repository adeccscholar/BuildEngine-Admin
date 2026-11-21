#include <gflags/gflags.h>
#include <print>

DEFINE_int32(buildengine_test_value, 17, "Native gflags consumer smoke");

int main(int iArgc, char** pszArgv) {
   gflags::ParseCommandLineFlags(&iArgc, &pszArgv, true);
   bool const bPassed = FLAGS_buildengine_test_value == 17;
   std::println("SMOKE|CHECK|flag-value|{}|Native shared gflags returns the declared default", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|gflags shared native consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
