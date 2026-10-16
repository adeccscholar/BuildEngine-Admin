#include <agg_basics.h>
#include <agg_trans_affine.h>
#include <iostream>

int main() {
   agg::trans_affine theTransform;
   theTransform.translate(5.0, 7.0);
   double flX = 1.0;
   double flY = 2.0;
   theTransform.transform(&flX, &flY);
   bool const bOk = flX == 6.0 && flY == 9.0;
   std::cout << "SMOKE|CHECK|affine-translation|" << (bOk ? "PASS" : "FAIL")
             << "|AGG transform translated coordinates\n";
   std::cout << "SMOKE|RESULT|" << (bOk ? "PASS" : "FAIL")
             << "|AGG consumer\n";
   return bOk ? 0 : 1;
}
