#include <simdutf.h>
#include <iostream>
#include <string_view>

int main() {
   std::string_view const svValid = "C++ UTF-8";
   std::string_view const svInvalid = "\xC3\x28";
   bool const bValid = simdutf::validate_utf8(svValid.data(), svValid.size());
   bool const bRejectsInvalid = !simdutf::validate_utf8(svInvalid.data(), svInvalid.size());
   std::cout << "SMOKE|CHECK|valid-utf8|" << (bValid ? "PASS" : "FAIL")
             << "|valid UTF-8 accepted\n";
   std::cout << "SMOKE|CHECK|invalid-utf8|" << (bRejectsInvalid ? "PASS" : "FAIL")
             << "|invalid UTF-8 rejected\n";
   bool const bOk = bValid && bRejectsInvalid;
   std::cout << "SMOKE|RESULT|" << (bOk ? "PASS" : "FAIL")
             << "|simdutf consumer\n";
   return bOk ? 0 : 1;
}
