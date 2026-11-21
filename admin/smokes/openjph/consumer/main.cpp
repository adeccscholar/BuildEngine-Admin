#include <openjph/ojph_codestream.h>
#include <openjph/ojph_version.h>
#include <print>

int main() {
   ojph::codestream theCodestream;
   bool const bConstructed = true;
   std::println("SMOKE|CHECK|codestream-construction|{}|OpenJPH native codestream instance", bConstructed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|OpenJPH shared native consumer", bConstructed ? "PASS" : "FAIL");
   return bConstructed ? 0 : 1;
}
