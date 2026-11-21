#include <OpenEXR/ImfHeader.h>
#include <print>

int main() {
   Imf::Header const aHeader {32, 16};
   bool const bWindow = aHeader.dataWindow().min.x == 0 &&
      aHeader.dataWindow().min.y == 0 &&
      aHeader.dataWindow().max.x == 31 &&
      aHeader.dataWindow().max.y == 15;

   std::println("SMOKE|CHECK|header-data-window|{}|OpenEXR 32x16 header has the expected data window",
      bWindow ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|OpenEXR native header consumer",
      bWindow ? "PASS" : "FAIL");
   return bWindow ? 0 : 1;
}
