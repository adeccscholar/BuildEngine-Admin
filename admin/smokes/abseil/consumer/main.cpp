#include <absl/strings/str_cat.h>
#include <iostream>
#include <string>
int main() {
   bool const ok = absl::StrCat("PDFium-", 153) == std::string("PDFium-153");
   std::cout << "SMOKE|CHECK|str-cat|" << (ok ? "PASS" : "FAIL") << "|Abseil string concatenation\n";
   std::cout << "SMOKE|RESULT|" << (ok ? "PASS" : "FAIL") << "|Abseil consumer\n";
   return ok ? 0 : 1;
}
