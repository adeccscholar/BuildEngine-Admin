// SPDX-License-Identifier: MIT
#include <omp.h>
#include <atomic>
#include <print>
#include <vector>

namespace {
bool Check(char const* name, bool result, char const* detail) {
   std::println("SMOKE|CHECK|{}|{}|{}", name, result ? "PASS" : "FAIL", detail);
   return result;
}
}

int main() {
   bool success = true;
   int threads = 0;
   int sum = 0;
   constexpr int count = 4096;
   std::vector<std::atomic<int>> visits(count);

   omp_set_dynamic(0);
   omp_set_num_threads(2);
#pragma omp parallel
   {
#pragma omp single
      threads = omp_get_num_threads();
   }
   success = Check("parallel-region", threads >= 2,
      "OpenMP creates at least two participating threads") && success;

#pragma omp parallel for reduction(+:sum) schedule(static)
   for(int i = 0; i < count; ++i) {
      visits[static_cast<std::size_t>(i)].fetch_add(1, std::memory_order_relaxed);
      sum += i;
   }
   bool exactlyOnce = true;
   for(auto const& value : visits)
      exactlyOnce = exactlyOnce && value.load(std::memory_order_relaxed) == 1;
   success = Check("parallel-for", exactlyOnce,
      "all loop iterations execute exactly once") && success;
   success = Check("reduction", sum == count * (count - 1) / 2,
      "parallel reduction matches exact arithmetic result") && success;

   std::println("SMOKE|RESULT|{}|BCC64X native OpenMP probe",
      success ? "PASS" : "FAIL");
   return success ? 0 : 1;
}
