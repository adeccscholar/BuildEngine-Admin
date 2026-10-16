#include <simdutf.h>
#include <string_view>

int main() {
   std::string_view const svValid = "C++ UTF-8";
   std::string_view const svInvalid = "\xC3\x28";
   if(!simdutf::validate_utf8(svValid.data(), svValid.size())) return 1;
   if(simdutf::validate_utf8(svInvalid.data(), svInvalid.size())) return 2;
   return 0;
}
