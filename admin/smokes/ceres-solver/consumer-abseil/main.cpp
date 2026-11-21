#include <ceres/ceres.h>
#include <cmath>

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
   return theSummary.IsSolutionUsable() &&
      std::abs(flParameter - 3.0) < 1e-6 ? 0 : 1;
}
