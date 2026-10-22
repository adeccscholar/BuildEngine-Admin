// SPDX-License-Identifier: MIT
#include <omp.h>
#include <atomic>
#include <array>
#include <cstdint>
#include <print>
#include <cwchar>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

namespace {
bool Check(char const* name, bool ok, char const* detail) {
   std::println("SMOKE|CHECK|{}|{}|{}", name, ok ? "PASS" : "FAIL", detail);
   return ok;
}
constexpr int N = 4096;
constexpr int Expected = N * (N - 1) / 2;

bool TestSchedule(omp_sched_t schedule, char const* name) {
   omp_set_schedule(schedule, 8);
   std::vector<std::atomic<int>> visits(N);
   int total = 0;
#pragma omp parallel for schedule(runtime) reduction(+:total)
   for(int i = 0; i < N; ++i) {
      visits[static_cast<std::size_t>(i)].fetch_add(1, std::memory_order_relaxed);
      total += i;
   }
   bool once = true;
   for(auto const& v : visits)
      once = once && (v.load(std::memory_order_relaxed) == 1);
   return Check(name, once && total == Expected, "runtime schedule, exact-once and reduction");
}
bool TestTasks() {
   std::array<std::atomic<int>, 128> done {};
#pragma omp parallel
   {
#pragma omp single
      {
#pragma omp taskgroup
         {
            for(int i = 0; i < static_cast<int>(done.size()); ++i) {
#pragma omp task firstprivate(i) shared(done)
               done[static_cast<std::size_t>(i)].fetch_add(1, std::memory_order_relaxed);
            }
         }
      }
   }
   bool ok = true;
   for(auto const& d : done) ok = ok && d.load() == 1;
   return Check("tasks-taskgroup", ok, "all tasks complete once before taskgroup exits");
}
bool TestSynchronization() {
   int criticalCount = 0;
   int atomicCount = 0;
   int barrierCount = 0;
   int singleCount = 0;
   int failed = 0;
#pragma omp parallel shared(criticalCount, atomicCount, barrierCount, singleCount, failed)
   {
      for(int i = 0; i < 128; ++i) {
#pragma omp critical(openmp_probe_counter)
         ++criticalCount;
#pragma omp atomic update
         ++atomicCount;
      }
#pragma omp atomic update
      ++barrierCount;
#pragma omp barrier
      if(barrierCount != omp_get_num_threads()) {
#pragma omp atomic update
         ++failed;
      }
#pragma omp single
      ++singleCount;
   }
   return Check("synchronization", failed == 0 && singleCount == 1 &&
      criticalCount == atomicCount && atomicCount >= 256,
      "critical, atomic, barrier and single");
}
bool TestDataScopes() {
   int sharedTotal = 0;
   int bad = 0;
   int seed = 29;
#pragma omp parallel firstprivate(seed) shared(sharedTotal, bad)
   {
      int privateValue = omp_get_thread_num();
      if(seed != 29 || privateValue < 0) {
#pragma omp atomic update
         ++bad;
      }
#pragma omp atomic update
      ++sharedTotal;
   }
   return Check("data-sharing", bad == 0 && sharedTotal >= 2,
      "firstprivate, private locals and shared state");
}
bool TestSimd() {
   std::vector<int> values(N);
   int sum = 0;
#pragma omp simd reduction(+:sum)
   for(int i = 0; i < N; ++i) {
      values[static_cast<std::size_t>(i)] = i;
      sum += i;
   }
   return Check("simd", sum == Expected && values.back() == N-1,
      "omp simd reduction and vector results");
}
bool TestRepeatedRegions() {
   int errors = 0;
   for(int round = 0; round < 16; ++round) {
      int count = 0;
#pragma omp parallel reduction(+:count)
      {
         count += 1;
      }
      if(count < 2) ++errors;
   }
   return Check("repeated-regions", errors == 0, "16 consecutive team lifecycles");
}
bool PrintRuntimeIdentity() {
#ifdef _WIN32
   HMODULE const module = GetModuleHandleW(L"libomp.dll");
   HMODULE const alternative = GetModuleHandleW(L"libiomp5md.dll");
   HMODULE const chosen = module ? module : alternative;
   if(chosen) {
      wchar_t path[32768] {};
      DWORD const size = GetModuleFileNameW(chosen, path, 32768);
      if(size > 0 && size < 32768)
         std::wprintf(L"SMOKE|INFO|openmp-runtime|%ls\n", path);
   } else {
      std::println("SMOKE|INFO|openmp-runtime|DLL name not identified");
   }
   return chosen != nullptr;
#else
   return false;
#endif
}
}

int main() {
   omp_set_dynamic(0);
   omp_set_num_threads(2);
   bool ok = true;
   int threads = 0;
#pragma omp parallel
   {
#pragma omp single
      threads = omp_get_num_threads();
   }
   ok = Check("parallel-region", threads >= 2, "two or more participating threads") && ok;
   ok = TestSchedule(omp_sched_static, "schedule-static") && ok;
   ok = TestSchedule(omp_sched_dynamic, "schedule-dynamic") && ok;
   ok = TestSchedule(omp_sched_guided, "schedule-guided") && ok;
   ok = TestTasks() && ok;
   ok = TestSynchronization() && ok;
   ok = TestDataScopes() && ok;
   ok = TestSimd() && ok;
   ok = TestRepeatedRegions() && ok;
   ok = Check("runtime-dll", PrintRuntimeIdentity(), "own libomp.dll must be loaded") && ok;
   std::println("SMOKE|RESULT|{}|BCC64X extended OpenMP probe", ok ? "PASS" : "FAIL");
   return ok ? 0 : 1;
}
