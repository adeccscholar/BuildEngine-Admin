#include <matplot/matplot.h>
#include <array>
#include <print>
int main() {
   std::array<double, 4> const arrX { 0.0, 1.0, 2.0, 3.0 };
   std::array<double, 4> const arrY { 0.0, 1.0, 4.0, 9.0 };
   auto const theFigure = matplot::figure(true);
   matplot::plot(arrX, arrY);
   bool const bPassed = static_cast<bool>(theFigure);
   std::println("SMOKE|CHECK|runtime|{}|Matplot++ figure created", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Matplot++ consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
