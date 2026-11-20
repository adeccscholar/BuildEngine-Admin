#include <hnswlib/hnswlib.h>

#include <array>
#include <cstdio>
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
      if(theMatches.size()!=1U || theMatches.top().second!=1U) return 2;
      std::puts("SMOKE|CHECK|hnswlib nearest neighbor");
      std::puts("SMOKE|RESULT|PASS");
      return 0;
   }
   catch(std::exception const& theError) {
      std::fprintf(stderr,"SMOKE|RESULT|FAIL %s\n",theError.what());
      return 1;
   }
}
