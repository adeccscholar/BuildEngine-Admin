#include <Imath/ImathVec.h>
#include <cmath>
#include <print>

int main() {
   Imath::V3f const aVector {3.0F, 4.0F, 0.0F};
   bool const bPassed = std::abs(aVector.length() - 5.0F) < 0.0001F;
   std::println("SMOKE|CHECK|vector-length|{}|Native Imath three-dimensional vector length", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Imath shared native consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
