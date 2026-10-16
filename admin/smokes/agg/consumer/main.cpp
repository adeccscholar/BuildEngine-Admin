#include <agg_basics.h>
#include <agg_trans_affine.h>

int main() {
   agg::trans_affine theTransform;
   theTransform.translate(5.0, 7.0);
   double flX = 1.0;
   double flY = 2.0;
   theTransform.transform(&flX, &flY);
   return (flX == 6.0 && flY == 9.0) ? 0 : 1;
}
