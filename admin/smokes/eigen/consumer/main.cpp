#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <print>
#include <cmath>

int main() {
   Eigen::Matrix3d theMatrix;
   theMatrix << 4., 1., 0., 1., 3., 0., 0., 0., 2.;
   Eigen::Vector3d theExpected { 1., 2., 3. };
   Eigen::Vector3d const theRhs = theMatrix * theExpected;
   Eigen::Vector3d const theSolved = theMatrix.ldlt().solve(theRhs);
   bool const bDense = (theSolved - theExpected).norm() < 1e-10;
   std::println("SMOKE|CHECK|dense-solve|{}|Eigen dense LDLT solve", bDense ? "PASS" : "FAIL");

   Eigen::SparseMatrix<double> theSparse(3, 3);
   theSparse.insert(0, 0) = 4.;
   theSparse.insert(1, 1) = 3.;
   theSparse.insert(2, 2) = 2.;
   theSparse.makeCompressed();
   bool const bSparse = (theSparse * Eigen::Vector3d::Ones() -
                         Eigen::Vector3d(4., 3., 2.)).norm() < 1e-10;
   std::println("SMOKE|CHECK|sparse-multiply|{}|Eigen sparse multiplication", bSparse ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Eigen 5.0.1 header-only BCC64X C++23 consumer",
                (bDense && bSparse) ? "PASS" : "FAIL");
   return (bDense && bSparse) ? 0 : 1;
}
