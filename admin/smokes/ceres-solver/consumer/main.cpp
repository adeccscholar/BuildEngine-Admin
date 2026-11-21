#include <ceres/ceres.h>
#include <cmath>
#include <print>

struct LinearResidual {
   template<typename T>
   bool operator()(T const* pParameter, T* pResidual) const {
      pResidual[0] = pParameter[0] - T{3};
      return true;
   }
};

int main() {
   double flParameter = 0.0;
   ceres::Problem theProblem;
   theProblem.AddResidualBlock(
      new ceres::AutoDiffCostFunction<LinearResidual, 1, 1>(new LinearResidual),
      nullptr, &flParameter);

   ceres::Solver::Options theOptions;
   theOptions.max_num_iterations = 25;
   theOptions.minimizer_progress_to_stdout = false;
   ceres::Solver::Summary theSummary;
   ceres::Solve(theOptions, &theProblem, &theSummary);

   bool const bUsable = theSummary.IsSolutionUsable();
   bool const bCorrect = std::abs(flParameter - 3.0) < 1e-6;

   // Exercise the public Bessel wrapper declarations in the installed package,
   // including the integer-order parity identities. No compiler-only shims.
   double const flArgument = 1.0;
   bool const bBesselZero = std::abs(ceres::BesselJ0(0.0) - 1.0) < 1e-12 &&
      std::abs(ceres::BesselJ1(0.0)) < 1e-12 &&
      std::abs(ceres::BesselJn(2, 0.0)) < 1e-12;
   bool const bBesselParity =
      std::abs(ceres::BesselJn(-1, flArgument) +
               ceres::BesselJn(1, flArgument)) < 1e-12 &&
      std::abs(ceres::BesselJ1(-flArgument) +
               ceres::BesselJ1(flArgument)) < 1e-12;

   std::println("SMOKE|CHECK|solution-usable|{}|Nonlinear least-squares solution is usable",
      bUsable ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|optimized-parameter|{}|Final parameter {:.8f}; expected 3",
      bCorrect ? "PASS" : "FAIL", flParameter);

   std::println("SMOKE|CHECK|bessel-origin|{}|J0(0)=1, J1(0)=0, J2(0)=0",
      bBesselZero ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|bessel-parity|{}|Integer-order Bessel parity preserved",
      bBesselParity ? "PASS" : "FAIL");

   if(bUsable && bCorrect && bBesselZero && bBesselParity) {
      std::println("SMOKE|RESULT|PASS|Ceres nonlinear least-squares solver validated");
      return 0;
   }
   std::println("SMOKE|RESULT|FAIL|Ceres optimization failed numerical validation");
   return 1;
}
