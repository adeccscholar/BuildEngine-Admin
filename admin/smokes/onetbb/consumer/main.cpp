#include <oneapi/tbb/parallel_reduce.h>
#include <oneapi/tbb/blocked_range.h>

#include <cstdint>
#include <print>

int main() {
   constexpr std::int64_t iBegin = 1;
   constexpr std::int64_t iEnd = 10001;
   constexpr std::int64_t iExpected = (iEnd - 1) * iEnd / 2;

   std::int64_t const iSum = oneapi::tbb::parallel_reduce(
      oneapi::tbb::blocked_range<std::int64_t>(iBegin, iEnd),
      std::int64_t{0},
      [](auto const& range, std::int64_t value) {
         for (std::int64_t i = range.begin(); i != range.end(); ++i)
            value += i;
         return value;
      },
      [](std::int64_t left, std::int64_t right) {
         return left + right;
      });

   bool const bSum = iSum == iExpected;
   std::println("SMOKE|CHECK|parallel_reduce|{}|sum={} expected={}",
                bSum ? "PASS" : "FAIL", iSum, iExpected);
   std::println("SMOKE|RESULT|{}|oneTBB consumer usable", bSum ? "PASS" : "FAIL");
   return bSum ? 0 : 1;
}
