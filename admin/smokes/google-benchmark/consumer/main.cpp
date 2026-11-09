#include <benchmark/benchmark.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>

namespace {
   constexpr std::uint64_t uExpectedIterations = 32;
   std::atomic<std::uint64_t> uExecutedIterations {0};

   void BM_NativeConsumer(benchmark::State& aState) {
      std::uint64_t uIterations = 0;
      for(auto const _ : aState) {
         benchmark::DoNotOptimize(aState.iterations());
         ++uIterations;
      }
      uExecutedIterations.fetch_add(uIterations, std::memory_order_relaxed);
   }
}

BENCHMARK(BM_NativeConsumer)->Iterations(uExpectedIterations);

int main(int iArgc, char** pszArgv) {
   benchmark::Initialize(&iArgc, pszArgv);

   bool const bArgumentsValid = !benchmark::ReportUnrecognizedArguments(iArgc, pszArgv);
   std::println("SMOKE|CHECK|arguments|{}|Google Benchmark argument processing",
      bArgumentsValid ? "PASS" : "FAIL");
   if(!bArgumentsValid) {
      std::println("SMOKE|RESULT|FAIL|google benchmark installed consumer");
      return 1;
   }

   // Google Benchmark's default ConsoleReporter writes context diagnostics to
   // stderr. The BuildEngine smoke validator requires strict stdout-only output.
   benchmark::ConsoleReporter aReporter {benchmark::ConsoleReporter::OO_None};
   aReporter.SetOutputStream(&std::cout);
   aReporter.SetErrorStream(&std::cout);
   std::size_t const uRegisteredBenchmarks = benchmark::RunSpecifiedBenchmarks(&aReporter);
   std::uint64_t const uIterations = uExecutedIterations.load(std::memory_order_relaxed);
   bool const bBenchmarkExecuted = uRegisteredBenchmarks == 1 &&
      uIterations >= uExpectedIterations;
   std::println("SMOKE|CHECK|execution|{}|benchmark executed {} of at least {} iterations",
      bBenchmarkExecuted ? "PASS" : "FAIL", uIterations, uExpectedIterations);

   benchmark::Shutdown();
   std::println("SMOKE|RESULT|{}|google benchmark installed consumer",
      bBenchmarkExecuted ? "PASS" : "FAIL");
   return bBenchmarkExecuted ? 0 : 1;
}
