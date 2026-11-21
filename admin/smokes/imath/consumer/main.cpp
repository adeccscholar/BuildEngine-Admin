#include <Imath/ImathVec.h>
#include <cmath>
int main() {
   Imath::V3f const aVector {3.0F,4.0F,0.0F};
   return std::abs(aVector.length()-5.0F)<0.0001F ? 0 : 1;
}
