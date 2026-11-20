#include <igl/per_vertex_normals.h>
#include <Eigen/Core>

#include <cmath>
#include <cstdio>

int main() {
   Eigen::Matrix<double,3,3> theVertices;
   theVertices << 0.0,0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0;
   Eigen::Matrix<int,1,3> theFaces;
   theFaces << 0,1,2;
   Eigen::Matrix<double,Eigen::Dynamic,3> theNormals;
   igl::per_vertex_normals(theVertices,theFaces,theNormals);
   if(theNormals.rows()!=3 || std::abs(theNormals(0,2)-1.0)>1.0e-9) {
      std::puts("SMOKE|RESULT|FAIL");
      return 1;
   }
   std::puts("SMOKE|CHECK|libigl+Eigen mesh normals");
   std::puts("SMOKE|RESULT|PASS");
   return 0;
}
