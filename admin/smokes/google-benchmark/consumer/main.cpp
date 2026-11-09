#include <benchmark/benchmark.h>
#include <print>

namespace {
   void BM_NativeConsumer(benchmark::State& aState) {
      for(auto const _ : aState) {
         benchmark::DoNotOptimize(aState.iterations());
      }
   }
}

BENCHMARK(BM_NativeConsumer)->Iterations(32);

int main(int iArgc, char** pszArgv) {
   benchmark::Initialize(&iArgc, pszArgv);
   if(benchmark::ReportUnrecognizedArguments(iArgc, pszArgv)) {
      std::println("SMOKE|RESULT|FAIL|google benchmark arguments");
      return 1;
   }
   benchmark::RunSpecifiedBenchmarks();
   benchmark::Shutdown();
   std::println("SMOKE|RESULT|PASS|google benchmark installed consumer");
   return 0;
}
