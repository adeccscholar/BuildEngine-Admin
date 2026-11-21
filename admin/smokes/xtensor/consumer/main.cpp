#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xmath.hpp>

#include <cmath>
#include <print>

int main() {
   xt::xarray<double> const theInput {{1.0, 2.0}, {3.0, 4.0}};
   xt::xarray<double> const theOutput = theInput + 2.0;

   bool const bShape = theOutput.shape().size() == 2 &&
      theOutput.shape()[0] == 2 && theOutput.shape()[1] == 2;
   std::println("SMOKE|CHECK|shape|{}|xtensor broadcasting preserves the matrix shape", bShape ? "PASS" : "FAIL");

   bool const bBroadcast = bShape &&
      std::abs(theOutput(0, 0) - 3.0) < 1.0e-12 &&
      std::abs(theOutput(1, 1) - 6.0) < 1.0e-12;
   std::println("SMOKE|CHECK|broadcast|{}|xtensor broadcasts a scalar to all matrix elements", bBroadcast ? "PASS" : "FAIL");

   bool const bPassed = bShape && bBroadcast;
   std::println("SMOKE|RESULT|{}|xtensor broadcasting", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
