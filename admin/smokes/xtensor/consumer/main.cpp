#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xmath.hpp>
#include <cmath>
#include <print>
int main() {
   xt::xarray<double> const theInput {{1.0,2.0},{3.0,4.0}};
   xt::xarray<double> const theOutput=theInput+2.0;
   bool const bPassed=theOutput.shape().size()==2 &&
      theOutput.shape()[0]==2 && theOutput.shape()[1]==2 &&
      std::abs(theOutput(1,1)-6.0)<1.0e-12;
   std::println("SMOKE|RESULT|{}|xtensor broadcasting",bPassed?"PASS":"FAIL");
   return bPassed?0:1;
}
