#include <igl/per_vertex_normals.h>
#include <Eigen/Core>

#include <cmath>
#include <print>

int main() {
   Eigen::Matrix<double,3,3> theVertices;
   theVertices << 0.0,0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0;
   Eigen::Matrix<int,1,3> theFaces;
   theFaces << 0,1,2;
   Eigen::Matrix<double,Eigen::Dynamic,3> theNormals;
   igl::per_vertex_normals(theVertices,theFaces,theNormals);
   bool const bPassed = theNormals.rows()==3 && std::abs(theNormals(0,2)-1.0)<=1.0e-9;
   std::println("SMOKE|CHECK|mesh-normals|{}|libigl and Eigen compute expected mesh normals", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|libigl consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
