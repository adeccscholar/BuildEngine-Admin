#include <OpenEXR/ImfHeader.h>
int main() {
   Imf::Header const aHeader {32,16};
   return aHeader.dataWindow().max.x==31 && aHeader.dataWindow().max.y==15 ? 0 : 1;
}
