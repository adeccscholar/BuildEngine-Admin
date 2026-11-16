#include "dng_host.h"

#include <print>

int main() {
   dng_host theHost;
   bool const bResult = theHost.PerformAreaTaskThreads() > 0;
   std::println("SMOKE|RESULT|{}|DNG SDK DLL consumer", bResult ? "PASS" : "FAIL");
   return bResult ? 0 : 1;
}
