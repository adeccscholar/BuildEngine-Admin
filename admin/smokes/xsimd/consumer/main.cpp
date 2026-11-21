#include <xsimd/xsimd.hpp>
#include <cmath>
#include <print>

int main() {
   xsimd::batch<float> const theValues { 2.0f };
   float const flSum = xsimd::reduce_add(theValues);
   float const flExpected = 2.0f * static_cast<float>(xsimd::batch<float>::size);

   bool const bFinite = std::isfinite(flSum);
   std::println("SMOKE|CHECK|finite|{}|SIMD reduction produces a finite result", bFinite ? "PASS" : "FAIL");

   bool const bReduction = bFinite && std::abs(flSum - flExpected) < 0.01f;
   std::println("SMOKE|CHECK|reduction|{}|SIMD reduction matches the lane count", bReduction ? "PASS" : "FAIL");

   bool const bPassed = bFinite && bReduction;
   std::println("SMOKE|RESULT|{}|xsimd SIMD reduction", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
