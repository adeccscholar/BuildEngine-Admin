// SPDX-License-Identifier: MIT
#include <oneapi/tbb/parallel_reduce.h>
#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/task_group.h>
#include <oneapi/tbb/flow_graph.h>
#include <oneapi/tbb/global_control.h>

#include <atomic>
#include <cstdint>
#include <exception>
#include <print>
#include <stdexcept>

namespace {
bool Check(char const* const szName, bool const bResult, char const* const szDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", szName, bResult ? "PASS" : "FAIL", szDetail);
   return bResult;
}
}

int main() {
   bool bSuccess = true;
   try {
      oneapi::tbb::global_control const theLimit {
         oneapi::tbb::global_control::max_allowed_parallelism, 4
      };

      constexpr std::int64_t iEnd = 10001;
      std::int64_t const iSum = oneapi::tbb::parallel_reduce(
         oneapi::tbb::blocked_range<std::int64_t> { 1, iEnd },
         std::int64_t { 0 },
         [](auto const& theRange, std::int64_t iValue) {
            for(std::int64_t i = theRange.begin(); i != theRange.end(); ++i)
               iValue += i;
            return iValue;
         },
         [](std::int64_t const iLeft, std::int64_t const iRight) {
            return iLeft + iRight;
         }
      );
      bSuccess = Check("parallel-reduce", iSum == (iEnd - 1) * iEnd / 2, "parallel sum matches arithmetic series") && bSuccess;

      std::atomic<int> theCount { 0 };
      oneapi::tbb::task_group theGroup;
      for(int i = 0; i < 64; ++i)
         theGroup.run([&theCount] { theCount.fetch_add(1, std::memory_order_relaxed); });
      theGroup.wait();
      bSuccess = Check("task-group", theCount.load() == 64, "all scheduled tasks completed") && bSuccess;

      oneapi::tbb::flow::graph theGraph;
      std::atomic<int> theGraphSum { 0 };
      oneapi::tbb::flow::function_node<int, oneapi::tbb::flow::continue_msg> theNode {
         theGraph, oneapi::tbb::flow::unlimited,
         [&theGraphSum](int const iValue) {
            theGraphSum.fetch_add(iValue, std::memory_order_relaxed);
            return oneapi::tbb::flow::continue_msg {};
         }
      };
      for(int i = 1; i <= 100; ++i)
         theNode.try_put(i);
      theGraph.wait_for_all();
      bSuccess = Check("flow-graph", theGraphSum.load() == 5050, "flow graph messages processed") && bSuccess;

      oneapi::tbb::task_group theFailureGroup;
      theFailureGroup.run([] { throw std::runtime_error { "expected task failure" }; });
      bool bCaught = false;
      try { theFailureGroup.wait(); }
      catch(std::runtime_error const&) { bCaught = true; }
      bSuccess = Check("exception-propagation", bCaught, "worker exception reaches task_group wait") && bSuccess;
   }
   catch(std::exception const& theException) {
      std::println("SMOKE|ERROR|{}", theException.what());
      bSuccess = false;
   }
   std::println("SMOKE|RESULT|{}|oneTBB consumer", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
