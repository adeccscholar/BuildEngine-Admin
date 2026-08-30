#include "runtime-uncaught-boundary.h"

#include <boost/core/uncaught_exceptions.hpp>

#include <exception>
#include <iostream>

namespace {

void PrintCheckpoint(char const* const pName) {
   std::cout << "[UNCAUGHT-BOUNDARY] " << pName << std::endl;
}

bool RunDiagnostics() {
#if defined(__cpp_lib_uncaught_exceptions)
   constexpr unsigned long uLocalFeature = static_cast<unsigned long>(__cpp_lib_uncaught_exceptions);
#else
   constexpr unsigned long uLocalFeature = 0UL;
#endif

#if defined(__cpp_lib_uncaught_exceptions) && __cpp_lib_uncaught_exceptions >= 201411
   constexpr int iLocalBoostUsesStd = 1;
#else
   constexpr int iLocalBoostUsesStd = 0;
#endif

   std::cout << "[UNCAUGHT-BOUNDARY] local feature=" << uLocalFeature
             << " boost-uses-std=" << iLocalBoostUsesStd << std::endl;

   PrintCheckpoint("BEGIN local-std");
   unsigned int const uLocalStd = static_cast<unsigned int>(std::uncaught_exceptions());
   std::cout << "[UNCAUGHT-BOUNDARY] local std=" << uLocalStd << std::endl;
   PrintCheckpoint("PASS local-std");

   PrintCheckpoint("BEGIN local-boost");
   unsigned int const uLocalBoost = boost::core::uncaught_exceptions();
   std::cout << "[UNCAUGHT-BOUNDARY] local boost=" << uLocalBoost << std::endl;
   PrintCheckpoint("PASS local-boost");

   PrintCheckpoint("BEGIN dll-contract");
   unsigned long const uDllFeature = AdeccBoundaryUncaughtExceptionsFeature();
   int const iDllBoostUsesStd = AdeccBoundaryBoostUsesStdUncaughtExceptions();
   std::cout << "[UNCAUGHT-BOUNDARY] dll feature=" << uDllFeature
             << " boost-uses-std=" << iDllBoostUsesStd << std::endl;
   PrintCheckpoint("PASS dll-contract");

   PrintCheckpoint("BEGIN dll-std");
   unsigned int const uDllStd = AdeccBoundaryStdUncaughtExceptions();
   std::cout << "[UNCAUGHT-BOUNDARY] dll std=" << uDllStd << std::endl;
   PrintCheckpoint("PASS dll-std");

   PrintCheckpoint("BEGIN dll-boost");
   unsigned int const uDllBoost = AdeccBoundaryBoostUncaughtExceptions();
   std::cout << "[UNCAUGHT-BOUNDARY] dll boost=" << uDllBoost << std::endl;
   PrintCheckpoint("PASS dll-boost");

   bool const bContractEqual = uLocalFeature == uDllFeature && iLocalBoostUsesStd == iDllBoostUsesStd;
   bool const bCountsZero = uLocalStd == 0U && uLocalBoost == 0U && uDllStd == 0U && uDllBoost == 0U;

   std::cout << "[UNCAUGHT-BOUNDARY] contract=" << (bContractEqual ? "PASS" : "FAIL")
             << " counts=" << (bCountsZero ? "PASS" : "FAIL") << std::endl;
   return bContractEqual && bCountsZero;
}

}

int main() {
   return RunDiagnostics() ? 0 : 1;
}
