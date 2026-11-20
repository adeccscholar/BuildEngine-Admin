#include <hnswlib/hnswlib.h>

#include <array>
#include <print>
#include <exception>

int main() {
   try {
      hnswlib::L2Space theSpace{3U};
      hnswlib::HierarchicalNSW<float> theIndex{&theSpace,16U};
      std::array<float,3> const arrFirst{0.0f,0.0f,0.0f};
      std::array<float,3> const arrSecond{5.0f,5.0f,5.0f};
      std::array<float,3> const arrQuery{0.1f,0.1f,0.1f};
      theIndex.addPoint(arrFirst.data(),1U);
      theIndex.addPoint(arrSecond.data(),2U);
      auto theMatches=theIndex.searchKnn(arrQuery.data(),1U);
      bool const bPassed = theMatches.size()==1U && theMatches.top().second==1U;
      std::println("SMOKE|CHECK|nearest-neighbor|{}|hnswlib returns the expected nearest label", bPassed ? "PASS" : "FAIL");
      std::println("SMOKE|RESULT|{}|hnswlib consumer", bPassed ? "PASS" : "FAIL");
      return bPassed ? 0 : 2;
   }
   catch(std::exception const& theError) {
      std::println("SMOKE|CHECK|nearest-neighbor|FAIL|hnswlib threw an exception");
      std::println("SMOKE|RESULT|FAIL|hnswlib exception: {}", theError.what());
      return 1;
   }
}
