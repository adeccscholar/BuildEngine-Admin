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

   std::println("SMOKE|CHECK|solution-usable|{}|Nonlinear least-squares solution is usable",
      bUsable ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|optimized-parameter|{}|Final parameter {:.8f}; expected 3",
      bCorrect ? "PASS" : "FAIL", flParameter);

   if(bUsable && bCorrect) {
      std::println("SMOKE|RESULT|PASS|Ceres nonlinear least-squares solver validated");
      return 0;
   }
   std::println("SMOKE|RESULT|FAIL|Ceres optimization failed numerical validation");
   return 1;
}
