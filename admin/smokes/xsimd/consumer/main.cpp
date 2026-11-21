#include <xsimd/xsimd.hpp>
#include <cmath>
#include <print>
int main() {
   xsimd::batch<float> const theValues { 2.0f };
   float const flSum=xsimd::reduce_add(theValues);
   bool const bPassed=std::abs(flSum-2.0f*xsimd::batch<float>::size)<0.01f;
   std::println("SMOKE|RESULT|{}|xsimd SIMD reduction",bPassed?"PASS":"FAIL");
   return bPassed?0:1;
}
