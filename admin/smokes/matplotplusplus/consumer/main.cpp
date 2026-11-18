#include <matplot/matplot.h>

#include <algorithm>
#include <array>
#include <print>

int main() {
   std::array<double, 4> const arrExpected { 0.0, 1.0, 2.0, 3.0 };
   auto const vecValues = matplot::linspace(0.0, 3.0, 4);
   bool const bPassed = vecValues.size() == arrExpected.size() &&
      std::ranges::equal(vecValues, arrExpected);
   std::println("SMOKE|CHECK|runtime|{}|Matplot++ linspace operation", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Matplot++ consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
