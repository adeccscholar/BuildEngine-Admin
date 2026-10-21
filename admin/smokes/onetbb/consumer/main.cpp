// SPDX-License-Identifier: MIT
#include <oneapi/tbb/parallel_reduce.h>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/parallel_scan.h>
#include <oneapi/tbb/parallel_sort.h>
#include <oneapi/tbb/parallel_pipeline.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/spin_mutex.h>
#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/task_group.h>
#include <oneapi/tbb/flow_graph.h>
#include <oneapi/tbb/global_control.h>

#include <atomic>
#include <array>
#include <algorithm>
#include <numeric>
#include <vector>
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

      constexpr int iElements = 2048;
      std::vector<std::atomic<int>> vecVisits(static_cast<std::size_t>(iElements));
      oneapi::tbb::parallel_for(0, iElements, [&vecVisits](int const i) {
         vecVisits[static_cast<std::size_t>(i)].fetch_add(1, std::memory_order_relaxed);
      });
      bool const bExactlyOnce = std::all_of(vecVisits.begin(), vecVisits.end(),
         [](std::atomic<int> const& theValue) { return theValue.load() == 1; });
      bSuccess = Check("parallel-for", bExactlyOnce, "each input visited exactly once") && bSuccess;

      std::vector<int> vecSorted(static_cast<std::size_t>(iElements));
      std::iota(vecSorted.begin(), vecSorted.end(), 0);
      std::reverse(vecSorted.begin(), vecSorted.end());
      oneapi::tbb::parallel_sort(vecSorted.begin(), vecSorted.end());
      bSuccess = Check("parallel-sort", std::is_sorted(vecSorted.begin(), vecSorted.end()) &&
         vecSorted.front() == 0 && vecSorted.back() == iElements - 1,
         "sort maintains complete ordered domain") && bSuccess;

      std::vector<int> vecPrefix(static_cast<std::size_t>(iElements), 0);
      oneapi::tbb::parallel_scan(oneapi::tbb::blocked_range<int> { 0, iElements },
         std::int64_t { 0 },
         [&vecPrefix](oneapi::tbb::blocked_range<int> const& theRange, std::int64_t iAccumulated,
                      bool const bFinal) {
            for(int i = theRange.begin(); i != theRange.end(); ++i) {
               iAccumulated += i + 1;
               if(bFinal) vecPrefix[static_cast<std::size_t>(i)] = static_cast<int>(iAccumulated);
            }
            return iAccumulated;
         },
         std::plus<std::int64_t> {});
      bool bPrefix = true;
      for(int i = 0; i < iElements; ++i)
         bPrefix = bPrefix && vecPrefix[static_cast<std::size_t>(i)] == (i + 1) * (i + 2) / 2;
      bSuccess = Check("parallel-scan", bPrefix, "all inclusive prefix sums match") && bSuccess;

      oneapi::tbb::task_arena theArena { 2 };
      std::atomic<int> iArenaCount { 0 };
      theArena.execute([&] {
         oneapi::tbb::parallel_for(0, 128, [&iArenaCount](int) {
            iArenaCount.fetch_add(1, std::memory_order_relaxed);
         });
      });
      bSuccess = Check("task-arena", iArenaCount.load() == 128,
         "parallel work executes in dedicated arena") && bSuccess;

      oneapi::tbb::concurrent_vector<int> vecConcurrent;
      oneapi::tbb::parallel_for(0, iElements, [&vecConcurrent](int const i) {
         vecConcurrent.push_back(i);
      });
      std::vector<int> vecValues(vecConcurrent.begin(), vecConcurrent.end());
      std::sort(vecValues.begin(), vecValues.end());
      bSuccess = Check("concurrent-vector", vecValues.size() == static_cast<std::size_t>(iElements) &&
         std::equal(vecValues.begin(), vecValues.end(), vecSorted.begin()),
         "concurrent appends preserve all unique elements") && bSuccess;

      oneapi::tbb::concurrent_queue<int> theQueue;
      oneapi::tbb::parallel_for(0, iElements, [&theQueue](int const i) { theQueue.push(i); });
      std::vector<int> vecDequeued;
      vecDequeued.reserve(iElements);
      int iDequeued {};
      while(theQueue.try_pop(iDequeued)) vecDequeued.push_back(iDequeued);
      std::sort(vecDequeued.begin(), vecDequeued.end());
      bSuccess = Check("concurrent-queue", vecDequeued == vecSorted,
         "queue drains exactly the submitted values") && bSuccess;

      oneapi::tbb::concurrent_hash_map<int, int> theMap;
      oneapi::tbb::parallel_for(0, iElements, [&theMap](int const i) {
         oneapi::tbb::concurrent_hash_map<int, int>::accessor theAccessor;
         theMap.insert(theAccessor, i);
         theAccessor->second = i * 2;
      });
      bool bMap = theMap.size() == static_cast<std::size_t>(iElements);
      for(int i = 0; i < iElements && bMap; ++i) {
         oneapi::tbb::concurrent_hash_map<int, int>::const_accessor theAccessor;
         bMap = theMap.find(theAccessor, i) && theAccessor->second == i * 2;
      }
      bSuccess = Check("concurrent-hash-map", bMap, "concurrent inserts and reads consistent") && bSuccess;

      oneapi::tbb::spin_mutex theMutex;
      int iGuarded {};
      oneapi::tbb::parallel_for(0, iElements, [&](int) {
         oneapi::tbb::spin_mutex::scoped_lock theLock { theMutex };
         ++iGuarded;
      });
      bSuccess = Check("spin-mutex", iGuarded == iElements,
         "all synchronized increments retained") && bSuccess;

      std::atomic<int> iPipelineSource { 0 };
      std::atomic<std::int64_t> iPipelineSum { 0 };
      oneapi::tbb::parallel_pipeline(4,
         oneapi::tbb::make_filter<void, int>(
            oneapi::tbb::filter_mode::serial_in_order,
            [&iPipelineSource](oneapi::tbb::flow_control& theFlow) {
               int const i = iPipelineSource.fetch_add(1);
               if(i >= iElements) { theFlow.stop(); return 0; }
               return i;
            }) &
         oneapi::tbb::make_filter<int, int>(
            oneapi::tbb::filter_mode::parallel,
            [](int const i) { return i * 2; }) &
         oneapi::tbb::make_filter<int, void>(
            oneapi::tbb::filter_mode::serial_in_order,
            [&iPipelineSum](int const i) { iPipelineSum.fetch_add(i); }));
      bSuccess = Check("parallel-pipeline",
         iPipelineSum.load() == static_cast<std::int64_t>(iElements) * (iElements - 1),
         "pipeline processes all inputs once") && bSuccess;

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
