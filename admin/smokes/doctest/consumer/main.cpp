#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <print>

TEST_CASE("managed doctest arithmetic") {
   CHECK(6 * 7 == 42);
}

int main(int iArgc, char** pszArgv) {
   doctest::Context theContext;
   theContext.applyCommandLine(iArgc, pszArgv);
   int const iResult = theContext.run();

   bool const bPassed = iResult == 0 && !theContext.shouldExit();
   std::println("SMOKE|CHECK|doctest-suite|{}|managed doctest test suite completed successfully", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|doctest managed test execution", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
