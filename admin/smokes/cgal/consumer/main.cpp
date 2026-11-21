#include <CGAL/Simple_cartesian.h>
#include <CGAL/squared_distance_2.h>

#include <cmath>
#include <print>

int main() {
   using kernel_ty = CGAL::Simple_cartesian<double>;
   kernel_ty::Point_2 const theA { 0.0, 0.0 };
   kernel_ty::Point_2 const theB { 3.0, 4.0 };

   double const flDistance = CGAL::squared_distance(theA, theB);
   bool const bFinite = std::isfinite(flDistance);
   std::println("SMOKE|CHECK|finite-distance|{}|CGAL squared distance is finite",
      bFinite ? "PASS" : "FAIL");

   bool const bDistance = bFinite && std::abs(flDistance - 25.0) < 1.0e-12;
   std::println("SMOKE|CHECK|squared-distance|{}|CGAL kernel preserves exact 3-4-5 geometry",
      bDistance ? "PASS" : "FAIL");

   bool const bPassed = bFinite && bDistance;
   std::println("SMOKE|RESULT|{}|CGAL kernel distance", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
